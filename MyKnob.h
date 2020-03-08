/*
  ==============================================================================

    MyKnob.h
    Created: 22 Feb 2020 4:45:41pm
    Author:  Music

	Custom Slider class (turns it into a knob) used in the UI.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MyKnob    : public Slider
{
public:
    MyKnob(const int width, const int height, float rangeBegin, float rangeEnd, float rangeStep, const juce::String text, float value, 
		juce::Slider::Listener *listener)
    {
		// Set slider to knob look
		Slider::setSliderStyle(Slider::Rotary);
		// Set width and height
		Slider::setSize(width, height);
		// Position the text box and remove the border around it
		Slider::setTextBoxStyle(Slider::TextBoxBelow, true, 120, LABEL_HEIGHT);
		Slider::setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
		// Set range
		Slider::setRange(rangeBegin, rangeEnd, rangeStep);
		// Set popup to show only on edit
		Slider::setPopupDisplayEnabled(true, false, this);
		// Set suffix for text (description, like "Lowpass Cutoff")
		Slider::setTextValueSuffix(text);
		// Initialise value
		Slider::setValue(value);
		// Make it listen to changes in the UI
		Slider::addListener(listener);
    }

    ~MyKnob()
    {
		
    }

    void paint (Graphics& g) override
    {
		Slider::paint(g);
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
		Slider::resized();
    }

	/*
	Return total height of component (knob height plus text box height)
	*/
	int getTotalHeight() {
		return Slider::getHeight() + Slider::getTextBoxHeight();
	}

	// Constant width, height and height of label
	static const int WIDTH = 120;
	static const int HEIGHT = 120;
	static const int LABEL_HEIGHT = 36;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyKnob)
};
