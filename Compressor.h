/*
  ==============================================================================

    Compressor.h
    Created: 27 Jan 2020 1:06:06pm
    Author:  Music

	A stereo compressor

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MyKnob.h"

class Compressor    : public Component, private Slider::Listener
{
public:
    Compressor(float sampleRate, int bufferSize, float threshold, float ratio, float tauAttack, float tauRelease, float makeUpGain);
    ~Compressor();

    void paint (Graphics&) override;
    void resized() override;
	void processBlock(AudioBuffer<float>& buffer, int totalNumOutputChannels);
	void setSampleRate(float sampleRate);
	void setBufferSize(int bufferSize);

private:
	std::unique_ptr<AudioBuffer<float>> inputBuffer;	// Separate buffer to analyse the input signal
	float sampleRate;
	int bufferSize;
	float yLPrev;					// Previous sample used for gain smoothing
	float* xg, * xl, * yg, * yl;	// Buffers used in control voltage calculation
	float* c;						// Output control voltage used for compression

	float threshold;
	float ratio;
	float tauAttack, tauRelease;	// Attack and release
	float makeUpGain;

	// Knobs
	std::unique_ptr<MyKnob> knobThreshold, knobRatio, knobAttack, knobRelease, knobMakeUpGain;

	void sliderValueChanged(Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Compressor)
};
