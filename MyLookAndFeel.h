/*
  ==============================================================================

    MyLookAndFeel.h
    Created: 22 Feb 2020 4:13:13pm
    Author:  Music

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#pragma once

class MyLookAndFeel : public LookAndFeel_V4
{
public:
	MyLookAndFeel();
	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
};