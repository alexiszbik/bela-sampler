#include "ProgramEditorWindow.h"

#include "ProgramGridComponent.h"
#include "ProgramWriter.h"
#include "SamplePreviewPlayer.h"
#include "SamplerLog.h"

#include <juce_core/juce_core.h>

ProgramEditorWindow::ProgramEditorWindow() {
	addAndMakeVisible(programSelector);
	programSelector.addListener(this);

	addAndMakeVisible(saveButton);
	saveButton.onClick = [this] { saveCurrentProgram(); };

	addAndMakeVisible(reloadButton);
	reloadButton.onClick = [this] {
		if(onReload) {
			onReload();
		}
	};

	addAndMakeVisible(addLayerButton);
	addLayerButton.onClick = [this] {
		if(!currentSlots.empty()) {
			const int lastNote = currentSlots.back().midiNote;
			ProgramSlotDesc newLayer;
			newLayer.midiNote = lastNote;
			newLayer.mode = ProgramSlotMode::Poly;
			currentSlots.push_back(newLayer);
			loadSelectedProgram();
			markDirty();
		}
	};

	viewport.setViewedComponent(&contentContainer, false);
	addAndMakeVisible(viewport);
}

ProgramEditorWindow::~ProgramEditorWindow() {
	programSelector.removeListener(this);
}

void ProgramEditorWindow::setPreviewPlayer(SamplePreviewPlayer* inPreviewPlayer) {
	previewPlayer = inPreviewPlayer;
}

void ProgramEditorWindow::loadPrograms(const std::string& inProgramFolder) {
	programFolder = inProgramFolder;

	const std::string mapPath = programFolder + "/program_map.json";
	ProgramMapJson parser;
	if(!parser.parseFile(mapPath, programMap)) {
		SAMPLER_LOG("Editor: could not load program_map.json\n");
		return;
	}

	programSelector.clear();

	int itemId = 1;
	for(const ProgramMapEntry& entry : programMap.entries) {
		const juce::String label = "PC " + juce::String(entry.pc) + " - " + juce::String(entry.file);
		programSelector.addItem(label, itemId++);
	}

	if(!programMap.entries.empty()) {
		programSelector.setSelectedId(1, juce::dontSendNotification);
		loadSelectedProgram();
	}
}

void ProgramEditorWindow::comboBoxChanged(juce::ComboBox* comboBox) {
	if(comboBox == &programSelector) {
		loadSelectedProgram();
	}
}

void ProgramEditorWindow::loadSelectedProgram() {
	contentContainer.removeAllChildren();
	programGrid.reset();

	const int selectedIndex = programSelector.getSelectedId() - 1;
	if(selectedIndex < 0 || selectedIndex >= static_cast<int>(programMap.entries.size())) {
		return;
	}

	currentFilepath = programFolder + "/" + programMap.entries[selectedIndex].file;
	currentSlots.clear();

	ProgramJson parser;
	if(!parser.parseFile(currentFilepath, currentSlots)) {
		SAMPLER_LOG("Editor: could not load %s\n", currentFilepath.c_str());
		return;
	}

	if(previewPlayer == nullptr) {
		SAMPLER_LOG("Editor: preview player not set\n");
		return;
	}

	programGrid = std::make_unique<ProgramGridComponent>(currentSlots, *previewPlayer);
	programGrid->onModified = [this] { markDirty(); };
	contentContainer.addAndMakeVisible(*programGrid);

	updateLayout();
}

void ProgramEditorWindow::saveCurrentProgram() {
	if(currentFilepath.empty()) {
		return;
	}

	if(!ProgramWriter::writeProgram(currentFilepath, currentSlots)) {
		SAMPLER_LOG("Editor: could not save %s\n", currentFilepath.c_str());
		return;
	}

	SAMPLER_LOG("Editor: saved %s\n", currentFilepath.c_str());
	dirty = false;
}

void ProgramEditorWindow::markDirty() {
	dirty = true;
}

void ProgramEditorWindow::paint(juce::Graphics& g) {
	g.fillAll(juce::Colour(0xff1e1e1e));
}

void ProgramEditorWindow::resized() {
	auto bounds = getLocalBounds();

	auto topBar = bounds.removeFromTop(36);
	programSelector.setBounds(topBar.removeFromLeft(300).reduced(2));
	saveButton.setBounds(topBar.removeFromLeft(70).reduced(2));
	reloadButton.setBounds(topBar.removeFromLeft(90).reduced(2));
	addLayerButton.setBounds(topBar.removeFromLeft(80).reduced(2));

	viewport.setBounds(bounds);
	updateLayout();
}

void ProgramEditorWindow::updateLayout() {
	const int viewportWidth = viewport.getWidth();
	if(viewportWidth <= 0) {
		return;
	}

	int neededHeight = 0;

	if(programGrid != nullptr) {
		programGrid->setSize(viewportWidth, programGrid->getNeededHeight());
		neededHeight = programGrid->getHeight();
	}

	contentContainer.setSize(viewportWidth, neededHeight);
}
