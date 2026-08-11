#pragma once

#include "ProgramMapJson.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

class ProgramMapGridComponent : public juce::Component
{
public:
	explicit ProgramMapGridComponent(ProgramMap& programMap);
	~ProgramMapGridComponent() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	int getNeededHeight() const;

	std::function<void()> onModified;

private:
	struct RowComponents {
		std::unique_ptr<juce::Label> pcLabel;
		std::unique_ptr<juce::Label> fileLabel;
		std::unique_ptr<juce::TextButton> deleteButton;
	};

	void rebuildRows();
	void onRowModified(size_t row);
	void addEntry();
	void deleteEntry(size_t row);

	ProgramMap& programMap;
	std::vector<RowComponents> rows;

	static constexpr int kRowHeight = 28;
	static constexpr int kHeaderHeight = 24;
};
