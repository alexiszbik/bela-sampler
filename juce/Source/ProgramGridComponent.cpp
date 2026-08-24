#include "ProgramGridComponent.h"

#include "MixBusNames.h"
#include "SamplerDesktopPaths.h"
#include "SamplerOptions.h"

#include <juce_core/juce_core.h>

#include <algorithm>
#include <cmath>

namespace {
int modeToIndex(ProgramSlotMode mode) {
	switch(mode) {
		case ProgramSlotMode::Poly: return 0;
		case ProgramSlotMode::Mono: return 1;
		case ProgramSlotMode::Gate: return 2;
	}

	return 0;
}

ProgramSlotMode indexToMode(int index) {
	switch(index) {
		case 1: return ProgramSlotMode::Mono;
		case 2: return ProgramSlotMode::Gate;
		default: return ProgramSlotMode::Poly;
	}
}

int playModeToIndex(ProgramSlotPlayMode mode) {
	switch(mode) {
		case ProgramSlotPlayMode::Normal: return 0;
		case ProgramSlotPlayMode::Granular: return 1;
	}

	return 0;
}

ProgramSlotPlayMode indexToPlayMode(int index) {
	return index == 1 ? ProgramSlotPlayMode::Granular : ProgramSlotPlayMode::Normal;
}

int muteGroupToIndex(MuteGroup group) {
	switch(group) {
		case MuteGroup::None: return 0;
		case MuteGroup::A: return 1;
		case MuteGroup::B: return 2;
		case MuteGroup::C: return 3;
		case MuteGroup::D: return 4;
	}

	return 0;
}

MuteGroup indexToMuteGroup(int index) {
	switch(index) {
		case 1: return MuteGroup::A;
		case 2: return MuteGroup::B;
		case 3: return MuteGroup::C;
		case 4: return MuteGroup::D;
		default: return MuteGroup::None;
	}
}

int busToIndex(MixBusIndex bus) {
	switch(bus) {
		case kBusMaster: return 0;
		case kBusSample: return 1;
		case kBusKick: return 2;
		case kBusSnare: return 3;
		case kBusToms: return 4;
		case kBusHats: return 5;
		default: return 0;
	}
}

MixBusIndex indexToBus(int index) {
	switch(index) {
		case 1: return kBusSample;
		case 2: return kBusKick;
		case 3: return kBusSnare;
		case 4: return kBusToms;
		case 5: return kBusHats;
		default: return kBusMaster;
	}
}

bool samplePathExistsCaseSensitive(const juce::File& samplesRoot, const std::string& relativePath) {
	if(relativePath.empty()) {
		return true;
	}

	juce::StringArray parts;
	parts.addTokens(relativePath, "/", "");
	parts.removeEmptyStrings();
	if(parts.isEmpty()) {
		return false;
	}

	juce::File current = samplesRoot;
	for(int partIndex = 0; partIndex < parts.size(); ++partIndex) {
		if(!current.isDirectory()) {
			return false;
		}

		const juce::String& part = parts[partIndex];
		const bool isLastPart = partIndex == parts.size() - 1;
		const int searchFlags = isLastPart
			? juce::File::findFiles
			: juce::File::findDirectories;

		bool found = false;
		for(const juce::File& child : current.findChildFiles(searchFlags, false)) {
			if(child.getFileName() == part) {
				current = child;
				found = true;
				break;
			}
		}

		if(!found) {
			return false;
		}
	}

	return current.existsAsFile();
}

juce::Colour normalRowBandColour(size_t rowIndex) {
	static constexpr juce::uint32 colours[4] = {
		0xff3d4f63,
		0xff3d634f,
		0xff634f3d,
		0xff4f3d63,
	};

	return juce::Colour(colours[rowIndex % 4]);
}

void applyLabelRowColour(juce::Label& label, const juce::Colour& background) {
	label.setOpaque(true);
	label.setColour(juce::Label::backgroundColourId, background);
	label.setColour(juce::Label::outlineColourId, background);
	label.setColour(juce::Label::textColourId, juce::Colours::white);
}

void applyComboRowColour(juce::ComboBox& combo, const juce::Colour& background) {
	combo.setOpaque(true);
	combo.setColour(juce::ComboBox::backgroundColourId, background);
	combo.setColour(juce::ComboBox::outlineColourId, background.darker(0.2f));
	combo.setColour(juce::ComboBox::buttonColourId, background.brighter(0.12f));
	combo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
}

void applyTextButtonRowColour(juce::Button& button, const juce::Colour& background) {
	button.setOpaque(true);
	button.setColour(juce::TextButton::buttonColourId, background.brighter(0.15f));
	button.setColour(juce::TextButton::buttonOnColourId, background.brighter(0.25f));
	button.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
	button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
}

void applyToggleRowColour(juce::ToggleButton& toggle, const juce::Colour& background) {
	toggle.setOpaque(false);
	toggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
	toggle.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggle.setColour(juce::ToggleButton::tickDisabledColourId, background.brighter(0.35f));
	toggle.setColour(juce::TextButton::buttonColourId, background.brighter(0.15f));
	toggle.setColour(juce::TextButton::buttonOnColourId, background.brighter(0.25f));
}
}

ProgramGridComponent::ProgramGridComponent(std::vector<ProgramSlotDesc>& inSlots, SamplePreviewPlayer& inPreviewPlayer)
	: slots(inSlots),
	  previewPlayer(inPreviewPlayer) {
	sortSlotsByNote();
	rebuildRows();
}

ProgramGridComponent::~ProgramGridComponent() = default;

void ProgramGridComponent::rebuildRows() {
	rows.clear();
	rows.reserve(slots.size());

	for(size_t i = 0; i < slots.size(); ++i) {
		RowComponents row;
		row.noteLabel = std::make_unique<juce::Label>();
		row.noteLabel->setEditable(true, false, false);
		row.noteLabel->setText(juce::String(slots[i].midiNote), juce::dontSendNotification);
		row.noteLabel->onEditorHide = [this, i, notePtr = row.noteLabel.get()] {
			const int newNote = juce::jlimit(0, 127, notePtr->getText().getIntValue());
			if(slots[i].midiNote == newNote) {
				return;
			}

			slots[i].midiNote = newNote;
			notePtr->setText(juce::String(newNote), juce::dontSendNotification);
			sortSlotsByNote();
			rebuildRows();
			repaint();
			if(onModified) {
				onModified();
			}
		};
		addAndMakeVisible(*row.noteLabel);

		row.sampleLabel = std::make_unique<juce::Label>();
		row.sampleLabel->setEditable(true, false, false);
		row.sampleLabel->setText(juce::String(slots[i].sample), juce::dontSendNotification);
		row.sampleLabel->onTextChange = [this, i, samplePtr = row.sampleLabel.get()] {
			slots[i].sample = samplePtr->getText().toStdString();
			applyRowAppearance(i);
			onRowModified(i);
		};
		addAndMakeVisible(*row.sampleLabel);

		row.modeCombo = std::make_unique<juce::ComboBox>();
		row.modeCombo->addItemList(SamplerOptions::modeOptions(), 1);
		row.modeCombo->setSelectedId(modeToIndex(slots[i].mode) + 1, juce::dontSendNotification);
		row.modeCombo->onChange = [this, i] {
			slots[i].mode = indexToMode(rows[i].modeCombo->getSelectedId() - 1);
			onRowModified(i);
		};
		addAndMakeVisible(*row.modeCombo);

		row.busCombo = std::make_unique<juce::ComboBox>();
		row.busCombo->addItemList(SamplerOptions::busOptions(), 1);
		row.busCombo->setSelectedId(busToIndex(slots[i].bus) + 1, juce::dontSendNotification);
		row.busCombo->onChange = [this, i] {
			slots[i].bus = indexToBus(rows[i].busCombo->getSelectedId() - 1);
			onRowModified(i);
		};
		addAndMakeVisible(*row.busCombo);

		row.volumeLabel = std::make_unique<juce::Label>();
		row.volumeLabel->setEditable(true, false, false);
		row.volumeLabel->setText(juce::String(slots[i].volumeDb, 2), juce::dontSendNotification);
		row.volumeLabel->onTextChange = [this, i] {
			slots[i].volumeDb = static_cast<float>(rows[i].volumeLabel->getText().getDoubleValue());
			onRowModified(i);
		};
		row.volumeLabel->onEditorHide = [this, i, volumePtr = row.volumeLabel.get()] {
			const float value = std::round(static_cast<float>(volumePtr->getText().getDoubleValue()) * 100.f) / 100.f;
			slots[i].volumeDb = value;
			volumePtr->setText(juce::String(value, 2), juce::dontSendNotification);
		};
		addAndMakeVisible(*row.volumeLabel);

		row.pitchLabel = std::make_unique<juce::Label>();
		row.pitchLabel->setEditable(true, false, false);
		row.pitchLabel->setText(juce::String(slots[i].pitchSemitones, 2), juce::dontSendNotification);
		row.pitchLabel->onTextChange = [this, i] {
			slots[i].pitchSemitones = static_cast<float>(rows[i].pitchLabel->getText().getDoubleValue());
			onRowModified(i);
		};
		row.pitchLabel->onEditorHide = [this, i, pitchPtr = row.pitchLabel.get()] {
			const float value = std::round(static_cast<float>(pitchPtr->getText().getDoubleValue()) * 100.f) / 100.f;
			slots[i].pitchSemitones = value;
			pitchPtr->setText(juce::String(value, 2), juce::dontSendNotification);
		};
		addAndMakeVisible(*row.pitchLabel);

		row.muteGroupCombo = std::make_unique<juce::ComboBox>();
		row.muteGroupCombo->addItemList(SamplerOptions::muteGroupOptions(), 1);
		row.muteGroupCombo->setSelectedId(muteGroupToIndex(slots[i].muteGroup) + 1, juce::dontSendNotification);
		row.muteGroupCombo->onChange = [this, i] {
			slots[i].muteGroup = indexToMuteGroup(rows[i].muteGroupCombo->getSelectedId() - 1);
			onRowModified(i);
		};
		addAndMakeVisible(*row.muteGroupCombo);

		row.reversedToggle = std::make_unique<juce::ToggleButton>();
		row.reversedToggle->setToggleState(slots[i].reversed, juce::dontSendNotification);
		row.reversedToggle->onClick = [this, i] {
			slots[i].reversed = rows[i].reversedToggle->getToggleState();
			onRowModified(i);
		};
		addAndMakeVisible(*row.reversedToggle);

		row.playModeCombo = std::make_unique<juce::ComboBox>();
		row.playModeCombo->addItemList(SamplerOptions::playModeOptions(), 1);
		row.playModeCombo->setSelectedId(playModeToIndex(slots[i].playMode) + 1, juce::dontSendNotification);
		row.playModeCombo->onChange = [this, i] {
			slots[i].playMode = indexToPlayMode(rows[i].playModeCombo->getSelectedId() - 1);
			onRowModified(i);
		};
		addAndMakeVisible(*row.playModeCombo);

		row.granularSpeedLabel = std::make_unique<juce::Label>();
		row.granularSpeedLabel->setEditable(true, false, false);
		row.granularSpeedLabel->setText(juce::String(slots[i].granularSpeed, 2), juce::dontSendNotification);
		row.granularSpeedLabel->onTextChange = [this, i] {
			slots[i].granularSpeed = static_cast<float>(rows[i].granularSpeedLabel->getText().getDoubleValue());
			onRowModified(i);
		};
		addAndMakeVisible(*row.granularSpeedLabel);

		row.deleteButton = std::make_unique<juce::TextButton>("X");
		row.deleteButton->onClick = [this, i] { deleteLayer(i); };
		addAndMakeVisible(*row.deleteButton);

		row.showButton = std::make_unique<juce::TextButton>("Show");
		row.showButton->onClick = [this, i] {
			if(slots[i].sample.empty()) {
				return;
			}

			const juce::File sampleFile = juce::File(SamplerDesktopPaths::getSamplesFolder())
				.getChildFile(slots[i].sample);
			if(!sampleFile.existsAsFile()) {
				sampleFile.getParentDirectory().revealToUser();
				return;
			}

			sampleFile.revealToUser();
		};
		addAndMakeVisible(*row.showButton);

		row.playButton = std::make_unique<juce::TextButton>(juce::String::fromUTF8("▶"));
		row.playButton->onClick = [this, i] {
			if(slots[i].sample.empty()) {
				return;
			}

			const juce::File sampleFile = juce::File(SamplerDesktopPaths::getSamplesFolder())
				.getChildFile(slots[i].sample);
			previewPlayer.play(sampleFile);
		};
		addAndMakeVisible(*row.playButton);

		rows.push_back(std::move(row));
		applyRowAppearance(i);
	}

	resized();
}

void ProgramGridComponent::sortSlotsByNote() {
	std::stable_sort(slots.begin(), slots.end(),
		[](const ProgramSlotDesc& a, const ProgramSlotDesc& b) {
			return a.midiNote < b.midiNote;
		});
}

void ProgramGridComponent::onRowModified(size_t row) {
	(void)row;
	if(onModified) {
		onModified();
	}
}

bool ProgramGridComponent::isSampleMissing(size_t row) const {
	if(row >= slots.size()) {
		return false;
	}

	const std::string& sample = slots[row].sample;
	if(sample.empty()) {
		return false;
	}

	return !samplePathExistsCaseSensitive(juce::File(SamplerDesktopPaths::getSamplesFolder()), sample);
}

juce::Colour ProgramGridComponent::rowBackgroundColour(size_t rowIndex) const {
	if(isSampleMissing(rowIndex)) {
		return juce::Colour(0xff8b2a2a);
	}

	return normalRowBandColour(rowIndex);
}

void ProgramGridComponent::applyRowAppearance(size_t rowIndex) {
	if(rowIndex >= rows.size()) {
		return;
	}

	const juce::Colour background = rowBackgroundColour(rowIndex);
	RowComponents& row = rows[rowIndex];

	if(row.noteLabel != nullptr) applyLabelRowColour(*row.noteLabel, background);
	if(row.sampleLabel != nullptr) applyLabelRowColour(*row.sampleLabel, background);
	if(row.modeCombo != nullptr) applyComboRowColour(*row.modeCombo, background);
	if(row.busCombo != nullptr) applyComboRowColour(*row.busCombo, background);
	if(row.volumeLabel != nullptr) applyLabelRowColour(*row.volumeLabel, background);
	if(row.pitchLabel != nullptr) applyLabelRowColour(*row.pitchLabel, background);
	if(row.muteGroupCombo != nullptr) applyComboRowColour(*row.muteGroupCombo, background);
	if(row.reversedToggle != nullptr) applyToggleRowColour(*row.reversedToggle, background);
	if(row.playModeCombo != nullptr) applyComboRowColour(*row.playModeCombo, background);
	if(row.granularSpeedLabel != nullptr) applyLabelRowColour(*row.granularSpeedLabel, background);
	if(row.deleteButton != nullptr) applyTextButtonRowColour(*row.deleteButton, background);
	if(row.showButton != nullptr) applyTextButtonRowColour(*row.showButton, background);
	if(row.playButton != nullptr) applyTextButtonRowColour(*row.playButton, background);

	repaint();
}

void ProgramGridComponent::addLayer(int midiNote, const std::string& sample) {
	ProgramSlotDesc newLayer;
	newLayer.midiNote = midiNote;
	newLayer.mode = ProgramSlotMode::Poly;
	newLayer.volumeDb = -6.f;
	newLayer.sample = sample;
	slots.push_back(newLayer);
	sortSlotsByNote();
	rebuildRows();
	if(onModified) {
		onModified();
	}
}

void ProgramGridComponent::deleteLayer(size_t row) {
	if(row >= slots.size()) {
		return;
	}

	slots.erase(slots.begin() + static_cast<ptrdiff_t>(row));
	rebuildRows();
	if(onModified) {
		onModified();
	}
}

void ProgramGridComponent::paint(juce::Graphics& g) {
	g.fillAll(juce::Colour(0xff2a2a2a));

	g.setColour(juce::Colour(0xff3a3a3a));
	for(int col = 0; col < kColumnCount; ++col) {
		g.fillRect(columnX(col), 0, columnWidth(col), kHeaderHeight);
	}

	g.setColour(juce::Colour(0xffcccccc));
	g.setFont(juce::Font(13.f, juce::Font::bold));

	const char* headers[kColumnCount] = {"", "Note", "Sample", "Mode", "Bus", "Vol", "Pitch", "Mute", "Rev", "Play", "Gran", "", "Show"};
	for(int col = 0; col < kColumnCount; ++col) {
		g.drawText(headers[col], columnX(col) + 4, 0, columnWidth(col) - 8, kHeaderHeight, juce::Justification::left);
	}

	for(size_t i = 0; i < rows.size(); ++i) {
		const int y = kHeaderHeight + static_cast<int>(i) * kRowHeight;
		g.setColour(rowBackgroundColour(i));
		g.fillRect(0, y, getWidth(), kRowHeight);
	}

	if(slots.size() > 1) {
		g.setColour(juce::Colour(0xff505050));
		for(size_t i = 1; i < slots.size(); ++i) {
			if(slots[i].midiNote != slots[i - 1].midiNote) {
				const int y = kHeaderHeight + static_cast<int>(i) * kRowHeight;
				g.drawHorizontalLine(y, 0, static_cast<float>(getWidth()));
			}
		}
	}
}

int ProgramGridComponent::columnX(int col) const {
	const int totalWidth = getWidth();
	int x = 0;
	for(int c = 0; c < col; ++c) {
		x += columnWidth(c);
	}
	(void)totalWidth;
	return x;
}

int ProgramGridComponent::columnWidth(int col) const {
	const int w = getWidth();
	switch(col) {
		case kColPlay: return 30;
		case kColNote: return 50;
		case kColSample: return w * 25 / 100;
		case kColMode: return 100;
		case kColBus: return 100;
		case kColVolume: return 55;
		case kColPitch: return 55;
		case kColMute: return 100;
		case kColReversed: return 35;
		case kColPlayMode: return 100;
		case kColGranular: return 50;
		case kColDelete: return 30;
		case kColShow: return 44;
		default: return 50;
	}
}

void ProgramGridComponent::resized() {
	for(size_t i = 0; i < rows.size(); ++i) {
		const int y = kHeaderHeight + static_cast<int>(i) * kRowHeight;
		RowComponents& r = rows[i];

		if(r.playButton != nullptr) r.playButton->setBounds(columnX(kColPlay) + 2, y + 2, columnWidth(kColPlay) - 4, kRowHeight - 4);
		if(r.noteLabel != nullptr) r.noteLabel->setBounds(columnX(kColNote) + 2, y + 2, columnWidth(kColNote) - 4, kRowHeight - 4);
		if(r.sampleLabel != nullptr) r.sampleLabel->setBounds(columnX(kColSample) + 2, y + 2, columnWidth(kColSample) - 4, kRowHeight - 4);
		if(r.modeCombo != nullptr) r.modeCombo->setBounds(columnX(kColMode) + 2, y + 2, columnWidth(kColMode) - 4, kRowHeight - 4);
		if(r.busCombo != nullptr) r.busCombo->setBounds(columnX(kColBus) + 2, y + 2, columnWidth(kColBus) - 4, kRowHeight - 4);
		if(r.volumeLabel != nullptr) r.volumeLabel->setBounds(columnX(kColVolume) + 2, y + 2, columnWidth(kColVolume) - 4, kRowHeight - 4);
		if(r.pitchLabel != nullptr) r.pitchLabel->setBounds(columnX(kColPitch) + 2, y + 2, columnWidth(kColPitch) - 4, kRowHeight - 4);
		if(r.muteGroupCombo != nullptr) r.muteGroupCombo->setBounds(columnX(kColMute) + 2, y + 2, columnWidth(kColMute) - 4, kRowHeight - 4);
		if(r.reversedToggle != nullptr) r.reversedToggle->setBounds(columnX(kColReversed) + 2, y + 2, columnWidth(kColReversed) - 4, kRowHeight - 4);
		if(r.playModeCombo != nullptr) r.playModeCombo->setBounds(columnX(kColPlayMode) + 2, y + 2, columnWidth(kColPlayMode) - 4, kRowHeight - 4);
		if(r.granularSpeedLabel != nullptr) r.granularSpeedLabel->setBounds(columnX(kColGranular) + 2, y + 2, columnWidth(kColGranular) - 4, kRowHeight - 4);
		if(r.deleteButton != nullptr) r.deleteButton->setBounds(columnX(kColDelete) + 2, y + 2, columnWidth(kColDelete) - 4, kRowHeight - 4);
		if(r.showButton != nullptr) r.showButton->setBounds(columnX(kColShow) + 2, y + 2, columnWidth(kColShow) - 4, kRowHeight - 4);
	}

	setSize(getWidth(), kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight);
}

int ProgramGridComponent::getNeededHeight() const {
	return kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight;
}
