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
	xg = new float[bufferSize];
	xl = new float[bufferSize];
	yg = new float[bufferSize];
	yl = new float[bufferSize];
	c = new float[bufferSize];

	this->sampleRate = sampleRate;
	this->bufferSize = bufferSize;
	this->threshold = threshold;
	this->ratio = ratio;
	this->tauAttack = tauAttack;
	this->tauRelease = tauRelease;
	this->makeUpGain = makeUpGain;

	// Initialise knobs
	knobThreshold = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, -60.0, 0.0, 1.0, " Threshold (dB)", -12.0, this));
	knobRatio = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 1.0, 20.0, 1.0, " : 1 (ratio)", 2.0, this));
	knobAttack = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 1.0, 1000.0, 1.0, " Attack Time (ms)", 10.0, this));
	knobRelease = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 1.0, 2000.0, 1.0, " Release Time (ms)", 70.0, this));
	knobMakeUpGain = std::unique_ptr<MyKnob>(
		new MyKnob(MyKnob::WIDTH, MyKnob::HEIGHT, 0.0, 10.0, 1.0, " Make-up Gain (dB)", 0.0, this));

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
	delete xg;
	delete xl;
	delete yg;
	delete yl;
	delete c;
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

	for (int i = 0; i < bufferSize; ++i) {
		currentSample = inputBuffer->getReadPointer(0)[i];
		// Level detection- estimate level using peak detector 
		// I.e. look at the amplitude of the current sample
		if (fabs(currentSample) < 0.000001) 
			xg[i] = -120; 
		else 
			xg[i] = 20 * log10(fabs(currentSample));
		
		// Gain computer - static apply input/output curve 
		if (xg[i] >= threshold) // Attenuate
			yg[i] = threshold + (xg[i] - threshold) / ratio; 
		else // Leave unchanged
			yg[i] = xg[i]; 
		
		xl[i] = xg[i] - yg[i];
		
		// Ballistics- smoothing of the gain 
		if (xl[0] > yLPrev) 
			yl[i] = alphaAttack * yLPrev + (1 - alphaAttack) * xl[i]; 
		else 
			yl[i] = alphaRelease * yLPrev + (1 - alphaRelease) * xl[i] ;
		
		// find control 
		c[i] = pow(10,(makeUpGain - yl[i]) / 20);
		yLPrev=yl[i];
	}

	for (int channel = 0; channel < totalNumOutputChannels; ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);
		for (int i = 0; i < bufferSize; i++) {
			channelData[i] *= c[channel];
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
