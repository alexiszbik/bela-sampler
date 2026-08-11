#include "ProgramGridComponent.h"

#include "MixBusNames.h"
#include "SamplerOptions.h"

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
}

ProgramGridComponent::ProgramGridComponent(std::vector<ProgramSlotDesc>& inSlots)
	: slots(inSlots) {
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
		row.noteLabel->onTextChange = [this, i, notePtr = row.noteLabel.get()] {
			slots[i].midiNote = notePtr->getText().getIntValue();
			onRowModified(i);
		};
		addAndMakeVisible(*row.noteLabel);

		row.sampleLabel = std::make_unique<juce::Label>();
		row.sampleLabel->setEditable(true, false, false);
		row.sampleLabel->setText(juce::String(slots[i].sample), juce::dontSendNotification);
		row.sampleLabel->onTextChange = [this, i, samplePtr = row.sampleLabel.get()] {
			slots[i].sample = samplePtr->getText().toStdString();
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
		row.volumeLabel->setText(juce::String(slots[i].volumeDb, 1), juce::dontSendNotification);
		row.volumeLabel->onTextChange = [this, i] {
			slots[i].volumeDb = static_cast<float>(rows[i].volumeLabel->getText().getDoubleValue());
			onRowModified(i);
		};
		addAndMakeVisible(*row.volumeLabel);

		row.pitchLabel = std::make_unique<juce::Label>();
		row.pitchLabel->setEditable(true, false, false);
		row.pitchLabel->setText(juce::String(slots[i].pitchSemitones, 1), juce::dontSendNotification);
		row.pitchLabel->onTextChange = [this, i] {
			slots[i].pitchSemitones = static_cast<float>(rows[i].pitchLabel->getText().getDoubleValue());
			onRowModified(i);
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

		rows.push_back(std::move(row));
	}

	resized();
}

void ProgramGridComponent::onRowModified(size_t row) {
	(void)row;
	if(onModified) {
		onModified();
	}
}

void ProgramGridComponent::addLayer(int midiNote) {
	ProgramSlotDesc newLayer;
	newLayer.midiNote = midiNote;
	newLayer.mode = ProgramSlotMode::Poly;
	slots.push_back(newLayer);
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

	const char* headers[kColumnCount] = {"Note", "Sample", "Mode", "Bus", "Vol", "Pitch", "Mute", "Rev", "Play", "Gran", ""};
	for(int col = 0; col < kColumnCount; ++col) {
		g.drawText(headers[col], columnX(col) + 4, 0, columnWidth(col) - 8, kHeaderHeight, juce::Justification::left);
	}

	for(size_t i = 0; i < rows.size(); ++i) {
		if(i % 2 == 1) {
			g.setColour(juce::Colour(0xff262626));
			g.fillRect(0, kHeaderHeight + static_cast<int>(i) * kRowHeight, getWidth(), kRowHeight);
		}
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
		case 0: return 50;
		case 1: return w * 25 / 100;
		case 2: return 100;
		case 3: return 100;
		case 4: return 45;
		case 5: return 45;
		case 6: return 100;
		case 7: return 35;
		case 8: return 100;
		case 9: return 50;
		case 10: return 30;
		default: return 50;
	}
}

void ProgramGridComponent::resized() {
	for(size_t i = 0; i < rows.size(); ++i) {
		const int y = kHeaderHeight + static_cast<int>(i) * kRowHeight;
		RowComponents& r = rows[i];

		if(r.noteLabel != nullptr) r.noteLabel->setBounds(columnX(0) + 2, y + 2, columnWidth(0) - 4, kRowHeight - 4);
		if(r.sampleLabel != nullptr) r.sampleLabel->setBounds(columnX(1) + 2, y + 2, columnWidth(1) - 4, kRowHeight - 4);
		if(r.modeCombo != nullptr) r.modeCombo->setBounds(columnX(2) + 2, y + 2, columnWidth(2) - 4, kRowHeight - 4);
		if(r.busCombo != nullptr) r.busCombo->setBounds(columnX(3) + 2, y + 2, columnWidth(3) - 4, kRowHeight - 4);
		if(r.volumeLabel != nullptr) r.volumeLabel->setBounds(columnX(4) + 2, y + 2, columnWidth(4) - 4, kRowHeight - 4);
		if(r.pitchLabel != nullptr) r.pitchLabel->setBounds(columnX(5) + 2, y + 2, columnWidth(5) - 4, kRowHeight - 4);
		if(r.muteGroupCombo != nullptr) r.muteGroupCombo->setBounds(columnX(6) + 2, y + 2, columnWidth(6) - 4, kRowHeight - 4);
		if(r.reversedToggle != nullptr) r.reversedToggle->setBounds(columnX(7) + 2, y + 2, columnWidth(7) - 4, kRowHeight - 4);
		if(r.playModeCombo != nullptr) r.playModeCombo->setBounds(columnX(8) + 2, y + 2, columnWidth(8) - 4, kRowHeight - 4);
		if(r.granularSpeedLabel != nullptr) r.granularSpeedLabel->setBounds(columnX(9) + 2, y + 2, columnWidth(9) - 4, kRowHeight - 4);
		if(r.deleteButton != nullptr) r.deleteButton->setBounds(columnX(10) + 2, y + 2, columnWidth(10) - 4, kRowHeight - 4);
	}

	setSize(getWidth(), kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight);
}

int ProgramGridComponent::getNeededHeight() const {
	return kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight;
}
