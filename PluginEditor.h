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
    Dafx_assignment_1AudioProcessor& processor;

	// General look and feel
	// MyLookAndFeel lookAndFeel;

	// Labels
	Label highLabel, midLabel, lowLabel;

	// Knobs
	MyKnob *knobLowpassCutoff;
	MyKnob *knobHighpassCutoff;

	// Compressor - Frontend
	Compressor* compLow = nullptr;
	Compressor* compMid = nullptr;
	Compressor* compHigh = nullptr;

	// Buttons
	ToggleButton toggleLowSolo;
	ToggleButton toggleMidSolo;
	ToggleButton toggleHighSolo;

	void sliderValueChanged(Slider* slider) override;
	void updateToggleState(Button* button, String name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_1AudioProcessorEditor)
};
