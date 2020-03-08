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
    Compressor(float sampleRate, int bufferSize, float threshold, float ratio, float tauAttack, float tauRelease, float makeUpGain, float kneeWidth);
    ~Compressor();

    void paint (Graphics&) override;
    void resized() override;
	void processBlock(AudioBuffer<float>& buffer, int totalNumOutputChannels);
	void setSampleRate(float sampleRate);
	void setBufferSize(int bufferSize);

private:
	// Separate buffer to analyse the input signal
	std::unique_ptr<AudioBuffer<float>> inputBuffer;
	// Sample rate coming from VST host
	float sampleRate;
	// Buffer size coming from VST host
	int bufferSize;
	// Previous sample used for gain smoothing
	float previousSample;
	// Buffers used in control voltage calculation
	float* inputGain, * gainDelta, * outputGain, * outputDelta;
	// Buffer of output control voltages used for compression - applied to each sample individually
	float* control;

	// Compressor properties
	// Threshold in dB
	float threshold;
	// Ratio
	float ratio;
	// Attack and release time in ms
	float tauAttack, tauRelease;
	// Make up gain and knee width in dB
	float makeUpGain;
	float kneeWidth;

	// Knobs
	std::unique_ptr<MyKnob> knobThreshold, knobRatio, knobAttack, knobRelease, knobMakeUpGain, knobKneeWidth;

	// Callback method for knob updates
	void sliderValueChanged(Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Compressor)
};