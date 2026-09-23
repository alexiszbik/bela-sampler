#include "ProgramEditorWindow.h"

#include "ProgramGridComponent.h"
#include "ProgramSamplesExporter.h"
#include "ProgramWriter.h"
#include "SamplerPreviewEngine.h"
#include "SamplerDesktopPaths.h"
#include "SamplerLog.h"

#include <juce_core/juce_core.h>

namespace {
int nextMidiNoteForNewLayer(const std::vector<ProgramSlotDesc>& slots) {
	if(slots.empty()) {
		return 0;
	}

	return juce::jmin(127, slots.back().midiNote + 1);
}
}

ProgramEditorWindow::ProgramEditorWindow() {
	addAndMakeVisible(programSelector);
	programSelector.addListener(this);

	addAndMakeVisible(saveButton);
	saveButton.onClick = [this] { saveAllPrograms(); };

	addAndMakeVisible(reloadButton);
	reloadButton.onClick = [this] { reloadAllPrograms(); };

	addAndMakeVisible(addLayerButton);
	addLayerButton.onClick = [this] {
		if(programGrid == nullptr) {
			return;
		}

		programGrid->addLayer(nextMidiNoteForNewLayer(editorBank.slotsForFile(currentProgramFileKey)));
		updateLayout();
	};

	addAndMakeVisible(addSampleButton);
	addSampleButton.onClick = [this] { addSampleWithDialog(); };

	addAndMakeVisible(exportHeaderButton);
	exportHeaderButton.onClick = [this] { exportSamplesHeader(); };

	viewport.setViewedComponent(&contentContainer, false);
	addAndMakeVisible(viewport);
}

ProgramEditorWindow::~ProgramEditorWindow() {
	programSelector.removeListener(this);
}

void ProgramEditorWindow::setPreviewPlayer(SamplerPreviewEngine* inPreviewPlayer) {
	previewPlayer = inPreviewPlayer;
}

void ProgramEditorWindow::loadPrograms(const std::string& inProgramFolder) {
	programFolder = inProgramFolder;

	const std::string mapPath = programFolder + "/program_map.json";
	ProgramMapJson parser;
	ProgramMap programMap;
	if(!parser.parseFile(mapPath, programMap)) {
		SAMPLER_LOG("Editor: could not load program_map.json\n");
		return;
	}

	if(!editorBank.loadFromMap(programFolder, programMap)) {
		SAMPLER_LOG("Editor: could not load program files\n");
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
	const ProgramMap& programMap = editorBank.getProgramMap();
	if(selectedIndex < 0 || selectedIndex >= static_cast<int>(programMap.entries.size())) {
		return;
	}

	currentProgramFileKey = programMap.entries[static_cast<size_t>(selectedIndex)].file;

	if(previewPlayer == nullptr) {
		SAMPLER_LOG("Editor: preview player not set\n");
		return;
	}

	programGrid = std::make_unique<ProgramGridComponent>(
		editorBank.slotsForFile(currentProgramFileKey),
		*previewPlayer,
		editorBank);
	programGrid->onModified = [this] {
		editorBank.markDirty(currentProgramFileKey);
		markDirty();
		if(programGrid != nullptr) {
			programGrid->refreshRefRowPreviews();
		}
	};
	contentContainer.addAndMakeVisible(*programGrid);

	updateLayout();
}

void ProgramEditorWindow::saveAllPrograms() {
	const ProgramMap& programMap = editorBank.getProgramMap();

	for(const ProgramMapEntry& entry : programMap.entries) {
		const std::string filepath = editorBank.filepathForFile(entry.file);
		if(!ProgramWriter::writeProgram(filepath, editorBank.slotsForFile(entry.file))) {
			SAMPLER_LOG("Editor: could not save %s\n", filepath.c_str());
			return;
		}

		editorBank.clearDirty(entry.file);
		SAMPLER_LOG("Editor: saved %s\n", filepath.c_str());
	}

	dirty = false;
}

void ProgramEditorWindow::reloadAllPrograms() {
	const int selectedId = programSelector.getSelectedId();
	const ProgramMap& programMap = editorBank.getProgramMap();
	if(!editorBank.loadFromMap(programFolder, programMap)) {
		SAMPLER_LOG("Editor: reload failed\n");
		return;
	}

	dirty = false;
	programSelector.setSelectedId(selectedId, juce::dontSendNotification);
	loadSelectedProgram();
}

void ProgramEditorWindow::exportSamplesHeader() {
	if(currentProgramFileKey.empty()) {
		return;
	}

	const int selectedIndex = programSelector.getSelectedId() - 1;
	const ProgramMap& programMap = editorBank.getProgramMap();
	if(selectedIndex < 0 || selectedIndex >= static_cast<int>(programMap.entries.size())) {
		return;
	}

	const std::string programJsonFile = programMap.entries[static_cast<size_t>(selectedIndex)].file;
	const std::string suggestedFilename = ProgramSamplesExporter::programFileToHeaderFilename(programJsonFile);

	juce::File suggestedDirectory(SamplerDesktopPaths::getGeneratedSamplesFolder());
	if(!suggestedDirectory.isDirectory()) {
		suggestedDirectory = juce::File(programFolder);
	}

	const juce::File suggestedFile = suggestedDirectory.getChildFile(suggestedFilename);

	exportHeaderFileChooser = std::make_unique<juce::FileChooser>(
		"Export samples header",
		suggestedFile,
		"*.h");

	constexpr auto flags = juce::FileBrowserComponent::saveMode
		| juce::FileBrowserComponent::warnAboutOverwriting
		| juce::FileBrowserComponent::canSelectFiles;

	exportHeaderFileChooser->launchAsync(flags, [this, programJsonFile](const juce::FileChooser& chooser) {
		juce::File outputFile = chooser.getResult();
		exportHeaderFileChooser.reset();

		if(outputFile == juce::File{}) {
			return;
		}

		if(!outputFile.hasFileExtension("h")) {
			outputFile = outputFile.withFileExtension("h");
		}

		const std::vector<ProgramSlotDesc> exportSlots = editorBank.materializedLayersForFile(programJsonFile);
		if(!ProgramSamplesExporter::writeHeader(outputFile.getFullPathName().toStdString(),
				programJsonFile,
				exportSlots)) {
			SAMPLER_LOG("Editor: no samples to export for %s\n", programJsonFile.c_str());
			return;
		}

		SAMPLER_LOG("Editor: exported %s\n", outputFile.getFullPathName().toRawUTF8());
	});
}

void ProgramEditorWindow::addSampleWithDialog() {
	if(programGrid == nullptr) {
		return;
	}

	const juce::File samplesFolder(SamplerDesktopPaths::getSamplesFolder());

	addSampleFileChooser = std::make_unique<juce::FileChooser>(
		"Select sample",
		samplesFolder,
		"*.wav;*.WAV;*.aif;*.aiff;*.flac;*.mp3");

	constexpr auto flags = juce::FileBrowserComponent::openMode
		| juce::FileBrowserComponent::canSelectFiles;

	addSampleFileChooser->launchAsync(flags, [this, samplesFolder](const juce::FileChooser& chooser) {
		const juce::File sampleFile = chooser.getResult();
		addSampleFileChooser.reset();

		if(sampleFile == juce::File{} || programGrid == nullptr) {
			return;
		}

		if(!sampleFile.isAChildOf(samplesFolder)) {
			SAMPLER_LOG("Editor: sample must be inside %s\n", samplesFolder.getFullPathName().toRawUTF8());
			return;
		}

		const std::string relativeSamplePath = sampleFile.getRelativePathFrom(samplesFolder)
			.replaceCharacter('\\', '/')
			.toStdString();

		programGrid->addLayer(nextMidiNoteForNewLayer(editorBank.slotsForFile(currentProgramFileKey)),
			relativeSamplePath);
		updateLayout();
	});
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
	addSampleButton.setBounds(topBar.removeFromLeft(90).reduced(2));
	exportHeaderButton.setBounds(topBar.removeFromLeft(80).reduced(2));

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
