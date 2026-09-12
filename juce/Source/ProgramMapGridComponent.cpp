#include "ProgramMapGridComponent.h"

ProgramMapGridComponent::ProgramMapGridComponent(ProgramMap& inMap)
	: programMap(inMap) {
	rebuildRows();
}

ProgramMapGridComponent::~ProgramMapGridComponent() = default;

void ProgramMapGridComponent::rebuildRows() {
	rows.clear();
	rows.reserve(programMap.entries.size());

	for(size_t i = 0; i < programMap.entries.size(); ++i) {
		RowComponents row;
		row.pcLabel = std::make_unique<juce::Label>();
		row.pcLabel->setEditable(true, false, false);
		row.pcLabel->setText(juce::String(programMap.entries[i].pc), juce::dontSendNotification);
		row.pcLabel->onTextChange = [this, i] {
			programMap.entries[i].pc = rows[i].pcLabel->getText().getIntValue();
			onRowModified(i);
		};
		addAndMakeVisible(*row.pcLabel);

		row.fileLabel = std::make_unique<juce::Label>();
		row.fileLabel->setEditable(true, false, false);
		row.fileLabel->setText(juce::String(programMap.entries[i].file), juce::dontSendNotification);
		row.fileLabel->onTextChange = [this, i] {
			programMap.entries[i].file = rows[i].fileLabel->getText().toStdString();
			onRowModified(i);
		};
		addAndMakeVisible(*row.fileLabel);

		row.deleteButton = std::make_unique<juce::TextButton>("X");
		row.deleteButton->onClick = [this, i] { deleteEntry(i); };
		addAndMakeVisible(*row.deleteButton);

		rows.push_back(std::move(row));
	}

	resized();
}

void ProgramMapGridComponent::onRowModified(size_t row) {
	(void)row;
	if(onModified) {
		onModified();
	}
}

void ProgramMapGridComponent::addEntry() {
	programMap.entries.push_back({0, "new_program.json"});
	rebuildRows();
	if(onModified) {
		onModified();
	}
}

void ProgramMapGridComponent::deleteEntry(size_t row) {
	if(row >= programMap.entries.size()) {
		return;
	}

	programMap.entries.erase(programMap.entries.begin() + static_cast<ptrdiff_t>(row));
	rebuildRows();
	if(onModified) {
		onModified();
	}
}

void ProgramMapGridComponent::paint(juce::Graphics& g) {
	g.fillAll(juce::Colour(0xff2a2a2a));

	g.setColour(juce::Colour(0xff3a3a3a));
	g.fillRect(0, 0, getWidth(), kHeaderHeight);

	g.setColour(juce::Colour(0xffcccccc));
	g.setFont(juce::Font(13.f, juce::Font::bold));
	g.drawText("PC", 4, 0, 60, kHeaderHeight, juce::Justification::left);
	g.drawText("File", 70, 0, getWidth() - 100, kHeaderHeight, juce::Justification::left);

	for(size_t i = 0; i < rows.size(); ++i) {
		if(i % 2 == 1) {
			g.setColour(juce::Colour(0xff262626));
			g.fillRect(0, kHeaderHeight + static_cast<int>(i) * kRowHeight, getWidth(), kRowHeight);
		}
	}
}

void ProgramMapGridComponent::resized() {
	const int w = getWidth();
	for(size_t i = 0; i < rows.size(); ++i) {
		const int y = kHeaderHeight + static_cast<int>(i) * kRowHeight;
		RowComponents& r = rows[i];

		if(r.pcLabel != nullptr) r.pcLabel->setBounds(2, y + 2, 60, kRowHeight - 4);
		if(r.fileLabel != nullptr) r.fileLabel->setBounds(66, y + 2, w - 100, kRowHeight - 4);
		if(r.deleteButton != nullptr) r.deleteButton->setBounds(w - 30, y + 2, 28, kRowHeight - 4);
	}

	setSize(w, kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight);
}

int ProgramMapGridComponent::getNeededHeight() const {
	return kHeaderHeight + static_cast<int>(rows.size()) * kRowHeight;
}
