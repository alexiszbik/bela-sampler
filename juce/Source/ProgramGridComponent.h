#pragma once

#include "ProgramJson.h"
#include "SamplePreviewPlayer.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <vector>

class ProgramGridComponent : public juce::Component
{
public:
	explicit ProgramGridComponent(std::vector<ProgramSlotDesc>& slots, SamplePreviewPlayer& previewPlayer);
	~ProgramGridComponent() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	int getNeededHeight() const;

	std::function<void()> onModified;
    
    void addLayer(int midiNote, const std::string& sample = {});
private:
	struct RowComponents {
		std::unique_ptr<juce::Label> noteLabel;
		std::unique_ptr<juce::Label> sampleLabel;
		std::unique_ptr<juce::ComboBox> modeCombo;
		std::unique_ptr<juce::ComboBox> busCombo;
		std::unique_ptr<juce::Label> volumeLabel;
		std::unique_ptr<juce::Label> pitchLabel;
		std::unique_ptr<juce::ComboBox> muteGroupCombo;
		std::unique_ptr<juce::ToggleButton> reversedToggle;
		std::unique_ptr<juce::ComboBox> playModeCombo;
		std::unique_ptr<juce::Label> granularSpeedLabel;
		std::unique_ptr<juce::TextButton> deleteButton;
		std::unique_ptr<juce::TextButton> showButton;
		std::unique_ptr<juce::TextButton> playButton;
	};

	enum Column {
		kColPlay = 0,
		kColNote,
		kColSample,
		kColMode,
		kColBus,
		kColVolume,
		kColPitch,
		kColMute,
		kColReversed,
		kColPlayMode,
		kColGranular,
		kColDelete,
		kColShow,
		kColCount
	};

	void rebuildRows();
	void sortSlotsByNote();
	void onRowModified(size_t row);
	bool isSampleMissing(size_t row) const;
	juce::Colour rowBackgroundColour(size_t rowIndex) const;
	void applyRowAppearance(size_t rowIndex);
	
	void deleteLayer(size_t row);

	std::vector<ProgramSlotDesc>& slots;
	std::vector<RowComponents> rows;
	std::vector<int> sampleNoteNumbers;
	SamplePreviewPlayer& previewPlayer;

	static constexpr int kRowHeight = 28;
	static constexpr int kHeaderHeight = 24;
	static constexpr int kColumnCount = kColCount;

	int columnX(int col) const;
	int columnWidth(int col) const;
};
