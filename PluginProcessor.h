#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Compressor.h"

//==============================================================================
/**
*/
class Dafx_assignment_1AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Dafx_assignment_1AudioProcessor();
    ~Dafx_assignment_1AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	void setSoloState(bool states[3]);
	bool noSolo();

	// Filter parameters
	float lowpassCutoff = 1000.0;
	float bandpassCentre = 5000.0;
	float highpassCutoff = 8000.0;

private:
	// Audio buffers for low, mid and high band
	std::unique_ptr<AudioBuffer<float>> lowBuffer;
	std::unique_ptr <AudioBuffer<float>> midBuffer;
	std::unique_ptr<AudioBuffer<float>> highBuffer;

	// Placeholder for solo states of frequency bands
	std::array<bool, 3> soloStates;

	std::unique_ptr<Compressor> compLow;
	std::unique_ptr<IIRFilter> lowpassFilterL;
	std::unique_ptr<IIRFilter> lowpassFilterR;
	std::unique_ptr<IIRFilter> midFilterL;
	std::unique_ptr<IIRFilter> midFilterR;
	std::unique_ptr<IIRFilter> highpassFilterL;
	std::unique_ptr<IIRFilter> highpassFilterR;

	const double FILTER_Q = sqrt(2) / 2;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dafx_assignment_1AudioProcessor)
};