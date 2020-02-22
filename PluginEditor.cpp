#include "PluginProcessor.h"
#include "PluginEditor.h"

Dafx_assignment_1AudioProcessorEditor::Dafx_assignment_1AudioProcessorEditor (Dafx_assignment_1AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (600, 300);

	// Initialise knobs
	knob_lowpass_cutoff = new MyKnob(knobWidth, knobHeight, 1.0, 4000.0, 1.0, " Lowpass Cutoff", 2000.0, this);
	knob_bandpass_centre = new MyKnob(knobWidth, knobHeight, 4000.0, 6000.0, 1.0, " Midrange centre", 5000.0, this);
	knob_highpass_cutoff = new MyKnob(knobWidth, knobHeight, 6000.0, 20000.0, 1.0, " Highpass Cutoff", 8000.0, this);

	addAndMakeVisible(knob_lowpass_cutoff);
	addAndMakeVisible(knob_bandpass_centre);
	addAndMakeVisible(knob_highpass_cutoff);
	
	knob_lowpass_cutoff->setBounds(24, 24, knobWidth, knobHeight);
	knob_bandpass_centre->setBounds(24 * 2 + knobWidth, 24, knobWidth, knobHeight);
	knob_highpass_cutoff->setBounds(24 * 3 + 2 * knobWidth, 24, knobWidth, knobHeight);

	// Add solo buttons
	addAndMakeVisible(toggleLowSolo);
	addAndMakeVisible(toggleMidSolo);
	addAndMakeVisible(toggleHighSolo);
	toggleLowSolo.setBounds(knob_lowpass_cutoff->getX(), knob_lowpass_cutoff->getY() + 24, 24, 24);
	toggleMidSolo.setBounds(knob_bandpass_centre->getX(), knob_bandpass_centre->getY() + 24, 24, 24);
	toggleHighSolo.setBounds(knob_highpass_cutoff->getX(), knob_highpass_cutoff->getY() + 24, 24, 24);
	toggleLowSolo.onClick = [this] { updateToggleState(&toggleLowSolo, "ON"); };
	toggleMidSolo.onClick = [this] { updateToggleState(&toggleMidSolo, "ON"); };
	toggleHighSolo.onClick = [this] { updateToggleState(&toggleHighSolo, "ON"); };

}

Dafx_assignment_1AudioProcessorEditor::~Dafx_assignment_1AudioProcessorEditor()
{
	delete knob_lowpass_cutoff;
	delete knob_bandpass_centre;
	delete knob_highpass_cutoff;
}

void Dafx_assignment_1AudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void Dafx_assignment_1AudioProcessorEditor::resized()
{
}

void Dafx_assignment_1AudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	processor.lowpassCutoff = knob_lowpass_cutoff->getValue();
	processor.bandpassCentre = knob_bandpass_centre->getValue();
	processor.highpassCutoff = knob_highpass_cutoff->getValue();
}

void Dafx_assignment_1AudioProcessorEditor::updateToggleState(Button* button, String name)
{
	auto state = button->getToggleState();
	bool states[] = { toggleLowSolo.getToggleState(), toggleMidSolo.getToggleState(), toggleHighSolo.getToggleState() };
	processor.setSoloState(states);
}