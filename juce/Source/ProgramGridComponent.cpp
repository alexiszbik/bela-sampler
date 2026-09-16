#include "ProgramGridComponent.h"

#include "MixBusNames.h"
#include "SamplerDesktopPaths.h"
#include "SamplerOptions.h"

#include <juce_core/juce_core.h>

#include <algorithm>
#include <cmath>

int ProgramGridComponent::modeToIndex(ProgramSlotMode mode) {
	switch(mode) {
		case ProgramSlotMode::Poly: return 0;
		case ProgramSlotMode::Mono: return 1;
		case ProgramSlotMode::Gate: return 2;
	}

	return 0;
}

ProgramSlotMode ProgramGridComponent::indexToMode(int index) {
	switch(index) {
		case 1: return ProgramSlotMode::Mono;
		case 2: return ProgramSlotMode::Gate;
		default: return ProgramSlotMode::Poly;
	}
}

int ProgramGridComponent::playModeToIndex(ProgramSlotPlayMode mode) {
	switch(mode) {
		case ProgramSlotPlayMode::Normal: return 0;
		case ProgramSlotPlayMode::Granular: return 1;
	}

	return 0;
}

ProgramSlotPlayMode ProgramGridComponent::indexToPlayMode(int index) {
	return index == 1 ? ProgramSlotPlayMode::Granular : ProgramSlotPlayMode::Normal;
}

int ProgramGridComponent::muteGroupToIndex(MuteGroup group) {
	switch(group) {
		case MuteGroup::None: return 0;
		case MuteGroup::A: return 1;
		case MuteGroup::B: return 2;
		case MuteGroup::C: return 3;
		case MuteGroup::D: return 4;
	}

	return 0;
}

MuteGroup ProgramGridComponent::indexToMuteGroup(int index) {
	switch(index) {
		case 1: return MuteGroup::A;
		case 2: return MuteGroup::B;
		case 3: return MuteGroup::C;
		case 4: return MuteGroup::D;
		default: return MuteGroup::None;
	}
}

int ProgramGridComponent::busToIndex(MixBusIndex bus) {
	return static_cast<int>(bus);
}

MixBusIndex ProgramGridComponent::indexToBus(int index) {
	return static_cast<MixBusIndex>(index);
}

bool ProgramGridComponent::samplePathExists(const std::string& relativePath) const {
	if(relativePath.empty()) {
		return true;
	}

	juce::StringArray parts;
	parts.addTokens(relativePath, "/", "");
	parts.removeEmptyStrings();
	if(parts.isEmpty()) {
		return false;
	}

	juce::File current = juce::File(SamplerDesktopPaths::getSamplesFolder());
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

std::unique_ptr<juce::Label> ProgramGridComponent::makeEditableLabel(const juce::String& text) {
	auto label = std::make_unique<juce::Label>();
	label->setEditable(true, false, false);
	label->setText(text, juce::dontSendNotification);
	return label;
}

void ProgramGridComponent::addRowWidget(juce::Component& component) {
	addAndMakeVisible(component);
}

void ProgramGridComponent::bindComboBox(juce::ComboBox& combo, const juce::StringArray& options, int selectedIndex, std::function<void(int)> onSelected) {
	combo.addItemList(options, 1);
	combo.setSelectedId(selectedIndex + 1, juce::dontSendNotification);
	combo.onChange = [onSelected = std::move(onSelected), &combo] {
		onSelected(combo.getSelectedId() - 1);
	};
}

void ProgramGridComponent::setupNoteLabel(RowComponents& row, size_t rowIndex) {
	row.noteLabel = makeEditableLabel(juce::String(slots[rowIndex].midiNote));
	row.noteLabel->onEditorHide = [this, rowIndex, notePtr = row.noteLabel.get()] {
		const int newNote = juce::jlimit(0, 127, notePtr->getText().getIntValue());
		if(slots[rowIndex].midiNote == newNote) {
			return;
		}

		slots[rowIndex].midiNote = newNote;
		notePtr->setText(juce::String(newNote), juce::dontSendNotification);
		sortSlotsByNote();
		rebuildRows();
		repaint();
		if(onModified) {
			onModified();
		}
	};
	addRowWidget(*row.noteLabel);
}

void ProgramGridComponent::setupSampleLabel(RowComponents& row, size_t rowIndex) {
	row.sampleLabel = makeEditableLabel(juce::String(slots[rowIndex].sample));
	row.sampleLabel->onTextChange = [this, rowIndex, samplePtr = row.sampleLabel.get()] {
		slots[rowIndex].sample = samplePtr->getText().toStdString();
		applyRowAppearance(rowIndex);
		onRowModified(rowIndex);
	};
	addRowWidget(*row.sampleLabel);
}

void ProgramGridComponent::setupModeCombo(RowComponents& row, size_t rowIndex) {
	row.modeCombo = std::make_unique<juce::ComboBox>();
	bindComboBox(*row.modeCombo, SamplerOptions::modeOptions(), modeToIndex(slots[rowIndex].mode),
		[this, rowIndex](int index) {
			slots[rowIndex].mode = indexToMode(index);
			onRowModified(rowIndex);
		});
	addRowWidget(*row.modeCombo);
}

void ProgramGridComponent::setupBusCombo(RowComponents& row, size_t rowIndex) {
	row.busCombo = std::make_unique<juce::ComboBox>();
	bindComboBox(*row.busCombo, SamplerOptions::busOptions(), busToIndex(slots[rowIndex].bus),
		[this, rowIndex](int index) {
			slots[rowIndex].bus = indexToBus(index);
			onRowModified(rowIndex);
		});
	addRowWidget(*row.busCombo);
}

void ProgramGridComponent::setupVolumeLabel(RowComponents& row, size_t rowIndex) {
	row.volumeLabel = makeEditableLabel(juce::String(slots[rowIndex].volumeDb, 2));
	row.volumeLabel->onTextChange = [this, rowIndex] {
		slots[rowIndex].volumeDb = static_cast<float>(rows[rowIndex].volumeLabel->getText().getDoubleValue());
		onRowModified(rowIndex);
	};
	row.volumeLabel->onEditorHide = [this, rowIndex, volumePtr = row.volumeLabel.get()] {
		const float value = std::round(static_cast<float>(volumePtr->getText().getDoubleValue()) * 100.f) / 100.f;
		slots[rowIndex].volumeDb = value;
		volumePtr->setText(juce::String(value, 2), juce::dontSendNotification);
	};
	addRowWidget(*row.volumeLabel);
}

void ProgramGridComponent::setupPitchLabel(RowComponents& row, size_t rowIndex) {
	row.pitchLabel = makeEditableLabel(juce::String(slots[rowIndex].pitchSemitones, 2));
	row.pitchLabel->onTextChange = [this, rowIndex] {
		slots[rowIndex].pitchSemitones = static_cast<float>(rows[rowIndex].pitchLabel->getText().getDoubleValue());
		onRowModified(rowIndex);
	};
	row.pitchLabel->onEditorHide = [this, rowIndex, pitchPtr = row.pitchLabel.get()] {
		const float value = std::round(static_cast<float>(pitchPtr->getText().getDoubleValue()) * 100.f) / 100.f;
		slots[rowIndex].pitchSemitones = value;
		pitchPtr->setText(juce::String(value, 2), juce::dontSendNotification);
	};
	addRowWidget(*row.pitchLabel);
}

void ProgramGridComponent::setupPanLabel(RowComponents& row, size_t rowIndex) {
	row.panLabel = makeEditableLabel(juce::String(slots[rowIndex].pan, 0));
	row.panLabel->onTextChange = [this, rowIndex] {
		slots[rowIndex].pan = static_cast<float>(rows[rowIndex].panLabel->getText().getDoubleValue());
		onRowModified(rowIndex);
	};
	row.panLabel->onEditorHide = [this, rowIndex, panPtr = row.panLabel.get()] {
		const float value = static_cast<float>(panPtr->getText().getDoubleValue());
		slots[rowIndex].pan = value;
		panPtr->setText(juce::String(value, 0), juce::dontSendNotification);
	};
	addRowWidget(*row.panLabel);
}

void ProgramGridComponent::setupMuteGroupCombo(RowComponents& row, size_t rowIndex) {
	row.muteGroupCombo = std::make_unique<juce::ComboBox>();
	bindComboBox(*row.muteGroupCombo, SamplerOptions::muteGroupOptions(), muteGroupToIndex(slots[rowIndex].muteGroup),
		[this, rowIndex](int index) {
			slots[rowIndex].muteGroup = indexToMuteGroup(index);
			onRowModified(rowIndex);
		});
	addRowWidget(*row.muteGroupCombo);
}

void ProgramGridComponent::setupReversedToggle(RowComponents& row, size_t rowIndex) {
	row.reversedToggle = std::make_unique<juce::ToggleButton>();
	row.reversedToggle->setToggleState(slots[rowIndex].reversed, juce::dontSendNotification);
	row.reversedToggle->onClick = [this, rowIndex] {
		slots[rowIndex].reversed = rows[rowIndex].reversedToggle->getToggleState();
		onRowModified(rowIndex);
	};
	addRowWidget(*row.reversedToggle);
}

void ProgramGridComponent::setupPlayModeCombo(RowComponents& row, size_t rowIndex) {
	row.playModeCombo = std::make_unique<juce::ComboBox>();
	bindComboBox(*row.playModeCombo, SamplerOptions::playModeOptions(), playModeToIndex(slots[rowIndex].playMode),
		[this, rowIndex](int index) {
			slots[rowIndex].playMode = indexToPlayMode(index);
			onRowModified(rowIndex);
		});
	addRowWidget(*row.playModeCombo);
}

void ProgramGridComponent::setupGranularSpeedLabel(RowComponents& row, size_t rowIndex) {
	row.granularSpeedLabel = makeEditableLabel(juce::String(slots[rowIndex].granularSpeed, 2));
	row.granularSpeedLabel->onTextChange = [this, rowIndex] {
		slots[rowIndex].granularSpeed = static_cast<float>(rows[rowIndex].granularSpeedLabel->getText().getDoubleValue());
		onRowModified(rowIndex);
	};
	addRowWidget(*row.granularSpeedLabel);
}

void ProgramGridComponent::setupDeleteButton(RowComponents& row, size_t rowIndex) {
	row.deleteButton = std::make_unique<juce::TextButton>("X");
	row.deleteButton->onClick = [this, rowIndex] { deleteLayer(rowIndex); };
	addRowWidget(*row.deleteButton);
}

void ProgramGridComponent::setupShowButton(RowComponents& row, size_t rowIndex) {
	row.showButton = std::make_unique<juce::TextButton>("Show");
	row.showButton->onClick = [this, rowIndex] {
		if(slots[rowIndex].sample.empty()) {
			return;
		}

		const juce::File sampleFile = juce::File(SamplerDesktopPaths::getSamplesFolder())
			.getChildFile(slots[rowIndex].sample);
		if(!sampleFile.existsAsFile()) {
			sampleFile.getParentDirectory().revealToUser();
			return;
		}

		sampleFile.revealToUser();
	};
	addRowWidget(*row.showButton);
}

void ProgramGridComponent::setupPlayButton(RowComponents& row, size_t rowIndex) {
	row.playButton = std::make_unique<juce::TextButton>(juce::String::fromUTF8("▶"));
	row.playButton->onClick = [this, rowIndex] {
		if(slots[rowIndex].sample.empty()) {
			return;
		}

		const juce::File sampleFile = juce::File(SamplerDesktopPaths::getSamplesFolder())
			.getChildFile(slots[rowIndex].sample);
		previewPlayer.playSlot(slots[rowIndex], sampleFile);
	};
	addRowWidget(*row.playButton);
}

ProgramGridComponent::RowComponents ProgramGridComponent::buildRow(size_t rowIndex) {
	RowComponents row;
	setupNoteLabel(row, rowIndex);
	setupSampleLabel(row, rowIndex);
	setupModeCombo(row, rowIndex);
	setupBusCombo(row, rowIndex);
	setupVolumeLabel(row, rowIndex);
	setupPitchLabel(row, rowIndex);
	setupPanLabel(row, rowIndex);
	setupMuteGroupCombo(row, rowIndex);
	setupReversedToggle(row, rowIndex);
	setupPlayModeCombo(row, rowIndex);
	setupGranularSpeedLabel(row, rowIndex);
	setupDeleteButton(row, rowIndex);
	setupShowButton(row, rowIndex);
	setupPlayButton(row, rowIndex);
	collectRowCells(row);
	return row;
}

ProgramGridComponent::ProgramGridComponent(std::vector<ProgramSlotDesc>& inSlots, SamplerPreviewEngine& inPreviewPlayer)
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
		rows.push_back(buildRow(i));
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

	return !samplePathExists(sample);
}

juce::Colour ProgramGridComponent::normalRowBandColour(size_t rowIndex) const {
	static constexpr juce::uint32 colours[4] = {
		0xff3d4f63,
		0xff3d634f,
		0xff634f3d,
		0xff4f3d63,
	};

	return juce::Colour(colours[rowIndex % 4]);
}

juce::Colour ProgramGridComponent::rowBackgroundColour(size_t rowIndex) const {
	if(isSampleMissing(rowIndex)) {
		return juce::Colour(0xff8b2a2a);
	}

	return normalRowBandColour(rowIndex);
}

void ProgramGridComponent::collectRowCells(RowComponents& row) {
	row.cells = {
		{row.playButton.get(), kColPlay},
		{row.noteLabel.get(), kColNote},
		{row.sampleLabel.get(), kColSample},
		{row.modeCombo.get(), kColMode},
		{row.busCombo.get(), kColBus},
		{row.volumeLabel.get(), kColVolume},
		{row.pitchLabel.get(), kColPitch},
		{row.panLabel.get(), kColPan},
		{row.muteGroupCombo.get(), kColMute},
		{row.reversedToggle.get(), kColReversed},
		{row.playModeCombo.get(), kColPlayMode},
		{row.granularSpeedLabel.get(), kColGranular},
		{row.deleteButton.get(), kColDelete},
		{row.showButton.get(), kColShow},
	};
}

void ProgramGridComponent::applyLabelRowColour(juce::Label& label, const juce::Colour& background) const {
	label.setOpaque(true);
	label.setColour(juce::Label::backgroundColourId, background);
	label.setColour(juce::Label::outlineColourId, background);
	label.setColour(juce::Label::textColourId, juce::Colours::white);
}

void ProgramGridComponent::applyComboRowColour(juce::ComboBox& combo, const juce::Colour& background) const {
	combo.setOpaque(true);
	combo.setColour(juce::ComboBox::backgroundColourId, background);
	combo.setColour(juce::ComboBox::outlineColourId, background.darker(0.2f));
	combo.setColour(juce::ComboBox::buttonColourId, background.brighter(0.12f));
	combo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
}

void ProgramGridComponent::applyTextButtonRowColour(juce::Button& button, const juce::Colour& background) const {
	button.setOpaque(true);
	button.setColour(juce::TextButton::buttonColourId, background.brighter(0.15f));
	button.setColour(juce::TextButton::buttonOnColourId, background.brighter(0.25f));
	button.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
	button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
}

void ProgramGridComponent::applyToggleRowColour(juce::ToggleButton& toggle, const juce::Colour& background) const {
	toggle.setOpaque(false);
	toggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
	toggle.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggle.setColour(juce::ToggleButton::tickDisabledColourId, background.brighter(0.35f));
	toggle.setColour(juce::TextButton::buttonColourId, background.brighter(0.15f));
	toggle.setColour(juce::TextButton::buttonOnColourId, background.brighter(0.25f));
}

void ProgramGridComponent::applyComponentRowColour(juce::Component* component, const juce::Colour& background) const {
	if(component == nullptr) {
		return;
	}

	if(auto* label = dynamic_cast<juce::Label*>(component)) {
		applyLabelRowColour(*label, background);
		return;
	}

	if(auto* combo = dynamic_cast<juce::ComboBox*>(component)) {
		applyComboRowColour(*combo, background);
		return;
	}

	if(auto* toggle = dynamic_cast<juce::ToggleButton*>(component)) {
		applyToggleRowColour(*toggle, background);
		return;
	}

	if(auto* button = dynamic_cast<juce::TextButton*>(component)) {
		applyTextButtonRowColour(*button, background);
	}
}

void ProgramGridComponent::applyRowAppearance(size_t rowIndex) {
	if(rowIndex >= rows.size()) {
		return;
	}

	const juce::Colour background = rowBackgroundColour(rowIndex);
	const RowComponents& row = rows[rowIndex];

	for(const RowComponents::Cell& cell : row.cells) {
		applyComponentRowColour(cell.component, background);
	}

	repaint();
}

void ProgramGridComponent::addLayer(int midiNote, const std::string& sample) {
	ProgramSlotDesc newLayer;
	newLayer.midiNote = midiNote;
	newLayer.mode = ProgramSlotMode::Mono;
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

	const char* headers[kColumnCount] = {"", "Note", "Sample", "Mode", "Bus", "Vol", "Pitch", "Pan", "Mute", "Rev", "Play", "Gran", "", "Show"};
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
	switch(col) {
		case kColPlay: return 30;
		case kColNote: return 50;
		case kColSample: return 190;
		case kColMode: return 80;
		case kColBus: return 110;
		case kColVolume: return 55;
		case kColPitch: return 55;
		case kColPan: return 55;
		case kColMute: return 60;
		case kColReversed: return 35;
		case kColPlayMode: return 100;
		case kColGranular: return 50;
		case kColDelete: return 30;
		case kColShow: return 44;
		default: return 50;
	}
}

void ProgramGridComponent::layoutCell(juce::Component* component, int rowY, int column) const {
	if(component == nullptr) {
		return;
	}

	component->setBounds(columnX(column) + 2, rowY + 2, columnWidth(column) - 4, kRowHeight - 4);
}

void ProgramGridComponent::resized() {
	for(size_t i = 0; i < rows.size(); ++i) {
		const int y = kHeaderHeight + static_cast<int>(i) * kRowHeight;

		for(const RowComponents::Cell& cell : rows[i].cells) {
			layoutCell(cell.component, y, cell.column);
		}
	}

	setSize(getWidth(), kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight);
}

int ProgramGridComponent::getNeededHeight() const {
	return kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight;
}
