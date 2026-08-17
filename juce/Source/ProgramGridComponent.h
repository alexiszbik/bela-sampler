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

	void rebuildRows();
	void sortSlotsByNote();
	void setupRow(size_t row, int midiNote, bool isFirstLayer);
	void onRowModified(size_t row);
	void addLayer(int midiNote);
	void deleteLayer(size_t row);

	std::vector<ProgramSlotDesc>& slots;
	std::vector<RowComponents> rows;
	std::vector<int> sampleNoteNumbers;
	SamplePreviewPlayer& previewPlayer;

	static constexpr int kRowHeight = 28;
	static constexpr int kHeaderHeight = 24;
	static constexpr int kColumnCount = 13;

	int columnX(int col) const;
	int columnWidth(int col) const;
};
