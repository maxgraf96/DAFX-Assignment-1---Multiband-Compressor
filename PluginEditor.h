#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"
#include "MyKnob.h"

/**
*/
class Dafx_assignment_1AudioProcessorEditor  : public AudioProcessorEditor,
	private Slider::Listener
{
public:
    Dafx_assignment_1AudioProcessorEditor (Dafx_assignment_1AudioProcessor&);
    ~Dafx_assignment_1AudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
	const int knobWidth = 120;
	const int knobHeight = 120;
    Dafx_assignment_1AudioProcessor& processor;

	// General look and feel
	// MyLookAndFeel lookAndFeel;

	// Knobs
	MyKnob *knob_lowpass_cutoff;
	MyKnob *knob_bandpass_centre;
	MyKnob *knob_highpass_cutoff;

	// Buttons
	ToggleButton toggleLowSolo;
	ToggleButton toggleMidSolo;
	ToggleButton toggleHighSolo;

	void sliderValueChanged(Slider* slider) override;
	void updateToggleState(Button* button, String name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_1AudioProcessorEditor)
};
