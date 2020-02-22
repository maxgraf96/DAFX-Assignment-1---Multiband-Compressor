/*
  ==============================================================================

    MyKnob.h
    Created: 22 Feb 2020 4:45:41pm
    Author:  Music

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
    MyKnob(int width, int height, float rangeBegin, float rangeEnd, float rangeStep, const juce::String text, float value, 
		juce::Slider::Listener *listener)
    {
		Slider::setSliderStyle(Slider::Rotary);
		Slider::setSize(width, height);
		Slider::setTextBoxStyle(Slider::TextBoxBelow, false, 120, 50);
		Slider::setRange(rangeBegin, rangeEnd, rangeStep);
		Slider::setPopupDisplayEnabled(true, false, this);
		Slider::setTextValueSuffix(text);
		Slider::setValue(value);
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

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyKnob)
};
