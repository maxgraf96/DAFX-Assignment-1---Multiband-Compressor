/*
  ==============================================================================

    MyLookAndFeel.cpp
    Created: 22 Feb 2020 4:13:13pm
    Author:  Music

  ==============================================================================
*/

#include "MyLookAndFeel.h"
MyLookAndFeel::MyLookAndFeel() {
	setColour(Slider::thumbColourId, Colours::azure);
}

void MyLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
	const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
	auto radius = jmin(width / 2, height / 2) - 4.0f;
	auto centreX = x + width * 0.5f;
	auto centreY = y + height * 0.5f;
	auto rx = centreX - radius;
	auto ry = centreY - radius;
	auto rw = radius * 2.0f;
	auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

	// fill
	g.setColour(Colours::orange);
	g.fillEllipse(rx, ry, rw, rw);

	// outline
	g.setColour(Colours::red);
	g.drawEllipse(rx, ry, rw, rw, 1.0f);

	Path p;
	auto pointerLength = radius * 0.33f;
	auto pointerThickness = 2.0f;
	p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
	p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

	// pointer
	g.setColour(Colours::yellow);
	g.fillPath(p);
}