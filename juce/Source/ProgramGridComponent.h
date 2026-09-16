#pragma once

#include "ProgramJson.h"
#include "SamplerPreviewEngine.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <vector>

class ProgramGridComponent : public juce::Component
{
public:
	explicit ProgramGridComponent(std::vector<ProgramSlotDesc>& slots, SamplerPreviewEngine& previewPlayer);
	~ProgramGridComponent() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	int getNeededHeight() const;

	std::function<void()> onModified;
    
	void addLayer(int midiNote, const std::string& sample = {});

private:
	enum Column {
		kColPlay = 0,
		kColNote,
		kColSample,
		kColMode,
		kColBus,
		kColDispatch,
		kColVolume,
		kColPitch,
		kColPan,
		kColMute,
		kColReversed,
		kColPlayMode,
		kColGranular,
		kColDelete,
		kColShow,
		kColCount
	};

	struct RowComponents {
		std::unique_ptr<juce::Label> noteLabel;
		std::unique_ptr<juce::Label> sampleLabel;
		std::unique_ptr<juce::ComboBox> modeCombo;
		std::unique_ptr<juce::ComboBox> busCombo;
		std::unique_ptr<juce::ComboBox> dispatchCombo;
		std::unique_ptr<juce::Label> volumeLabel;
		std::unique_ptr<juce::Label> pitchLabel;
		std::unique_ptr<juce::Label> panLabel;
		std::unique_ptr<juce::ComboBox> muteGroupCombo;
		std::unique_ptr<juce::ToggleButton> reversedToggle;
		std::unique_ptr<juce::ComboBox> playModeCombo;
		std::unique_ptr<juce::Label> granularSpeedLabel;
		std::unique_ptr<juce::TextButton> deleteButton;
		std::unique_ptr<juce::TextButton> showButton;
		std::unique_ptr<juce::TextButton> playButton;

		struct Cell {
			juce::Component* component = nullptr;
			Column column = kColPlay;
		};

		std::vector<Cell> cells;
	};

	RowComponents buildRow(size_t rowIndex);
	void rebuildRows();
	void sortSlotsByNote();
	void onRowModified(size_t row);
	bool samplePathExists(const std::string& relativePath) const;
	bool isSampleMissing(size_t row) const;
	std::unique_ptr<juce::Label> makeEditableLabel(const juce::String& text);
	void addRowWidget(juce::Component& component);
	void bindComboBox(juce::ComboBox& combo, const juce::StringArray& options, int selectedIndex, std::function<void(int)> onSelected);
	void setupNoteLabel(RowComponents& row, size_t rowIndex);
	void setupSampleLabel(RowComponents& row, size_t rowIndex);
	void setupModeCombo(RowComponents& row, size_t rowIndex);
	void setupBusCombo(RowComponents& row, size_t rowIndex);
	void setupDispatchCombo(RowComponents& row, size_t rowIndex);
	void setupVolumeLabel(RowComponents& row, size_t rowIndex);
	void setupPitchLabel(RowComponents& row, size_t rowIndex);
	void setupPanLabel(RowComponents& row, size_t rowIndex);
	void setupMuteGroupCombo(RowComponents& row, size_t rowIndex);
	void setupReversedToggle(RowComponents& row, size_t rowIndex);
	void setupPlayModeCombo(RowComponents& row, size_t rowIndex);
	void setupGranularSpeedLabel(RowComponents& row, size_t rowIndex);
	void setupDeleteButton(RowComponents& row, size_t rowIndex);
	void setupShowButton(RowComponents& row, size_t rowIndex);
	void setupPlayButton(RowComponents& row, size_t rowIndex);
	static int modeToIndex(ProgramSlotMode mode);
	static ProgramSlotMode indexToMode(int index);
	static int playModeToIndex(ProgramSlotPlayMode mode);
	static ProgramSlotPlayMode indexToPlayMode(int index);
	static int muteGroupToIndex(MuteGroup group);
	static MuteGroup indexToMuteGroup(int index);
	static int busToIndex(MixBusIndex bus);
	static MixBusIndex indexToBus(int index);
	static int dispatchToIndex(SlotDispatch dispatch);
	static SlotDispatch indexToDispatch(int index);
	juce::Colour normalRowBandColour(size_t rowIndex) const;
	juce::Colour rowBackgroundColour(size_t rowIndex) const;
	void collectRowCells(RowComponents& row);
	void applyRowAppearance(size_t rowIndex);
	void applyComponentRowColour(juce::Component* component, const juce::Colour& background) const;
	void applyLabelRowColour(juce::Label& label, const juce::Colour& background) const;
	void applyComboRowColour(juce::ComboBox& combo, const juce::Colour& background) const;
	void applyTextButtonRowColour(juce::Button& button, const juce::Colour& background) const;
	void applyToggleRowColour(juce::ToggleButton& toggle, const juce::Colour& background) const;
	void deleteLayer(size_t row);

	std::vector<ProgramSlotDesc>& slots;
	std::vector<RowComponents> rows;
	std::vector<int> sampleNoteNumbers;
	SamplerPreviewEngine& previewPlayer;

	static constexpr int kRowHeight = 28;
	static constexpr int kHeaderHeight = 24;
	static constexpr int kColumnCount = kColCount;

	int columnX(int col) const;
	int columnWidth(int col) const;
	void layoutCell(juce::Component* component, int rowY, int column) const;
};
