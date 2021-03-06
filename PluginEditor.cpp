#include "PluginProcessor.h"
#include "PluginEditor.h"

Dafx_assignment_1AudioProcessorEditor::Dafx_assignment_1AudioProcessorEditor (Dafx_assignment_1AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	// Initialise knobs
	knobLowpassCutoff = new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 1.0, 10050.0, 1.0, " Lowpass Cutoff", 2000.0, this);
	knobHighpassCutoff = new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 10050.0, 20000.0, 1.0, " Highpass Cutoff", 8000.0, this);
	
	// Get the compressor instances if they are not yet set
	if (compLow == nullptr) {
		compLow = processor.getCompLow();
	}
	if (compMid == nullptr) {
		compMid = processor.getCompMid();
	}
	if (compHigh == nullptr) {
		compHigh = processor.getCompHigh();
	}
	
	// Initialise labels
	addAndMakeVisible(lowLabel);
	addAndMakeVisible(midLabel);
	addAndMakeVisible(highLabel);
	lowLabel.setText("Low band", dontSendNotification);
	midLabel.setText("Mid band", dontSendNotification);
	highLabel.setText("High band", dontSendNotification);

	// Add solo buttons
	// Define a striking colour for when a solo button is active
	// So users don't forget they have it on
	auto soloColour = Colours::darkred;
	toggleLowSolo.setColour(toggleLowSolo.buttonOnColourId, soloColour);
	toggleMidSolo.setColour(toggleMidSolo.buttonOnColourId, soloColour);
	toggleHighSolo.setColour(toggleHighSolo.buttonOnColourId, soloColour);
	addAndMakeVisible(toggleLowSolo);
	addAndMakeVisible(toggleMidSolo);
	addAndMakeVisible(toggleHighSolo);
	toggleLowSolo.setClickingTogglesState(true);
	toggleMidSolo.setClickingTogglesState(true);
	toggleHighSolo.setClickingTogglesState(true);
	toggleLowSolo.onClick = [this] { updateToggleState(&toggleLowSolo, "ON"); };
	toggleMidSolo.onClick = [this] { updateToggleState(&toggleMidSolo, "ON"); };
	toggleHighSolo.onClick = [this] { updateToggleState(&toggleHighSolo, "ON"); };

	// Add knobs
	addAndMakeVisible(knobLowpassCutoff);
	addAndMakeVisible(knobHighpassCutoff);
	addAndMakeVisible(compLow);
	addAndMakeVisible(compMid);
	addAndMakeVisible(compHigh);


	// Set size last, as it makes a call to resize()
	// I.e. make sure to initialise everything before calling setSize()
	setSize(WIDTH, HEIGHT);
}

Dafx_assignment_1AudioProcessorEditor::~Dafx_assignment_1AudioProcessorEditor()
{
	delete knobLowpassCutoff;
	delete knobHighpassCutoff;
}

void Dafx_assignment_1AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

	// Draw band divider lines
	g.setColour(Colours::lightgrey);
	g.drawHorizontalLine(compLow->getBottom(), 0, WIDTH);
	g.drawHorizontalLine(compMid->getBottom(), 0, WIDTH);

    g.setColour (Colours::white);
    g.setFont (15.0f);
}

/*
Per definition the position of all UI elements is done in the resized() method
*/
void Dafx_assignment_1AudioProcessorEditor::resized()
{
	lowLabel.setBounds(24, 24, 72, 24);
	midLabel.setBounds(24, MyKnob::HEIGHT + 24 * 3 + 12, 72, 24);
	highLabel.setBounds(24, MyKnob::HEIGHT * 2 + 24 * 5 + 24, 72, 24);

	toggleLowSolo.setBounds(lowLabel.getX() + lowLabel.getWidth() + 24, lowLabel.getY(), 24, 24);
	toggleMidSolo.setBounds(midLabel.getX() + midLabel.getWidth() + 24, midLabel.getY(), 24, 24);
	toggleHighSolo.setBounds(highLabel.getX() + highLabel.getWidth() + 24, highLabel.getY(), 24, 24);

	knobLowpassCutoff->setBounds(24, lowLabel.getY() + 24, MyKnob::WIDTH, MyKnob::HEIGHT);
	knobHighpassCutoff->setBounds(24, highLabel.getY() + 24, MyKnob::WIDTH, MyKnob::HEIGHT);
	
	compLow->setBounds(
		toggleLowSolo.getBounds().getTopRight().getX() + 24,
		toggleLowSolo.getBounds().getTopRight().getY(),
		compLow->getWidth(),
		compLow->getHeight());

	compMid->setBounds(
		toggleMidSolo.getBounds().getTopRight().getX() + 24,
		toggleMidSolo.getBounds().getTopRight().getY(),
		compMid->getWidth(),
		compMid->getHeight());

	compHigh->setBounds(
		toggleHighSolo.getBounds().getTopRight().getX() + 24,
		toggleHighSolo.getBounds().getTopRight().getY(),
		compHigh->getWidth(),
		compHigh->getHeight());
}

/*
Update values for lowpass and highpass on knob change
*/
void Dafx_assignment_1AudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	processor.lowpassCutoff = knobLowpassCutoff->getValue();
	processor.highpassCutoff = knobHighpassCutoff->getValue();
}

/*
Update the set of toggle states on a solo button click
*/
void Dafx_assignment_1AudioProcessorEditor::updateToggleState(Button* button, String name)
{
	auto state = button->getToggleState();
	bool states[] = { toggleLowSolo.getToggleState(), toggleMidSolo.getToggleState(), toggleHighSolo.getToggleState() };
	processor.setSoloState(states);
}