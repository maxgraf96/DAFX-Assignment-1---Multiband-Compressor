/*
  ==============================================================================

    Compressor.cpp
    Created: 27 Jan 2020 1:06:06pm
    Author:  Music

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Compressor.h"

Compressor::Compressor(float sampleRate, int bufferSize, float threshold, float ratio, float tauAttack, float tauRelease, float makeUpGain)
{
	inputBuffer = std::unique_ptr<AudioBuffer<float>>(new AudioBuffer<float>(2, bufferSize));
	inputGain = new float[bufferSize];
	gainDelta = new float[bufferSize];
	outputGain = new float[bufferSize];
	outputDelta = new float[bufferSize];
	control = new float[bufferSize];

	this->sampleRate = sampleRate;
	this->bufferSize = bufferSize;
	this->threshold = threshold;
	this->ratio = ratio;
	this->tauAttack = tauAttack;
	this->tauRelease = tauRelease;
	this->makeUpGain = makeUpGain;

	// Initialise knobs
	knobThreshold = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, -40.0, 0.0, 1.0, " Threshold (dB)", threshold, this));
	knobRatio = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 1.0, 20.0, 1.0, " : 1 (ratio)", ratio, this));
	knobAttack = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 20.0, 1000.0, 1.0, " Attack Time (ms)", tauAttack, this));
	knobRelease = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 20.0, 2000.0, 1.0, " Release Time (ms)", tauRelease, this));
	knobMakeUpGain = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 0.0, 10.0, 1.0, " Make-up Gain (dB)", makeUpGain, this));

	addAndMakeVisible(knobThreshold.get());
	addAndMakeVisible(knobRatio.get());
	addAndMakeVisible(knobAttack.get());
	addAndMakeVisible(knobRelease.get());
	addAndMakeVisible(knobMakeUpGain.get());

	setSize(5 * MyKnob::WIDTH + 4 * 24, MyKnob::HEIGHT + MyKnob::LABEL_HEIGHT);
	setOpaque(true);
}

Compressor::~Compressor()
{
	delete inputGain;
	delete gainDelta;
	delete outputGain;
	delete outputDelta;
	delete control;
	inputBuffer.reset();
}

void Compressor::processBlock(AudioBuffer<float>& buffer, int totalNumOutputChannels) {
	// Clear buffer
	// NB: Only need to clear channel 0 ("left") because we only write to channel 0
	inputBuffer->clear(0, 0, inputBuffer->getNumSamples());

	// Mix down left-right to analyse the input 
	// That means that both input channels run into one compressor
	inputBuffer->addFrom(0, 0, buffer, 0, 0, buffer.getNumSamples(), 0.5);
	inputBuffer->addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples(), 0.5);

	// Compression : calculates the control voltage 
	float alphaAttack = exp(-1 / (0.001 * sampleRate * tauAttack));
	float alphaRelease = exp(-1 / (0.001 * sampleRate * tauRelease));
	float currentSample = 0.0f; // Placeholder for current sample

	// Go through buffer and calculate the (non-)attenuation for each sample
	for (int i = 0; i < bufferSize; ++i) {
		currentSample = inputBuffer->getReadPointer(0)[i];
		// Level detection using peak detector 
		// I.e. look at the amplitude of the current sample
		if (fabs(currentSample) < 0.000001) 
			inputGain[i] = -120; 
		else 
			inputGain[i] = 20 * log10(fabs(currentSample));

		// Do some kneet exercises
		float slope = 1 / ratio - 1;
		float overshoot = inputGain[i] - threshold;
		if (overshoot <= -kneeWidth / 2)
			outputGain[i] = inputGain[i];
		if (overshoot > -kneeWidth / 2 && overshoot < kneeWidth / 2)
			outputGain[i] = inputGain[i] + slope * pow((overshoot + kneeWidth / 2), 2) / (2 * kneeWidth);
		if (overshoot >= -kneeWidth / 2)
			outputGain[i] = inputGain[i] + slope * overshoot;
		
		gainDelta[i] = inputGain[i] - outputGain[i];

		// Ballistics - smoothing of the gain
		if (gainDelta[i] > previousSample)
			outputDelta[i] = alphaAttack * previousSample + (1 - alphaAttack) * gainDelta[i]; 
		else 
			outputDelta[i] = alphaRelease * previousSample + (1 - alphaRelease) * gainDelta[i];
		
		// find control 
		// Conversion back to linear in order to apply to sample
		control[i] = pow(10, (makeUpGain - outputDelta[i]) / 20);
		previousSample = outputDelta[i];
	}

	for (int channel = 0; channel < totalNumOutputChannels; ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);
		for (int i = 0; i < bufferSize; i++) {
			// (Non-)attenuate
			channelData[i] *= control[channel];
		}
	}
}

void Compressor::paint (Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	g.setColour (Colours::white);
    g.setFont (13.0f);
}

void Compressor::resized()
{
	int marginTop = 24;
	int marginRight = 12;

	knobThreshold->setBounds(0, marginTop, MyKnob::WIDTH, MyKnob::HEIGHT);
	knobRatio->setBounds(MyKnob::WIDTH + marginRight, marginTop, MyKnob::WIDTH, MyKnob::HEIGHT);
	knobAttack->setBounds(MyKnob::WIDTH * 2 + marginRight * 2, marginTop, MyKnob::WIDTH, MyKnob::HEIGHT);
	knobRelease->setBounds(MyKnob::WIDTH * 3 + marginRight * 3, marginTop, MyKnob::WIDTH, MyKnob::HEIGHT);
	knobMakeUpGain->setBounds(MyKnob::WIDTH * 4 + marginRight * 4, marginTop, MyKnob::WIDTH, MyKnob::HEIGHT);
}

void Compressor::sliderValueChanged(Slider* slider) {
	threshold = knobThreshold->getValue();
	ratio = knobRatio->getValue();
	tauAttack = knobAttack->getValue();
	tauRelease = knobRelease->getValue();
	makeUpGain = knobMakeUpGain->getValue();
}

void Compressor::setSampleRate(float sampleRate) {
	this->sampleRate = sampleRate;
}

void Compressor::setBufferSize(int bufferSize) {
	this->bufferSize = bufferSize;
}
