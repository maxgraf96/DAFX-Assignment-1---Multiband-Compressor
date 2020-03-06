#include "PluginProcessor.h"
#include "PluginEditor.h"

Dafx_assignment_1AudioProcessor::Dafx_assignment_1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	// Create buffers for low, mid and high frequency bands
	int blockSize = pow(2, 12);
	// This is just pro forma, the value is overwritten in prepareToPlay()
	float sampleRate = 48000.0;
	lowBuffer = std::unique_ptr<AudioBuffer<float>>(new AudioBuffer<float>(2, blockSize));
	midBuffer = std::unique_ptr<AudioBuffer<float>>(new AudioBuffer<float>(2, blockSize));
	highBuffer = std::unique_ptr<AudioBuffer<float>>(new AudioBuffer<float>(2, blockSize));

	// Create the filter objects (currently limited to 3 bands)
	// NB: One filter per channel is required
	for (int i = 0; i < lowpassFilters.size(); i++) {
		lowpassFilters[i] = std::unique_ptr<IIRFilter>(new IIRFilter());
		highpassFilters[i] = std::unique_ptr<IIRFilter>(new IIRFilter());
	}

	float threshold = -8.0f;
	float ratio = 6.0f / 1.0f;
	float tauAttack = 10.0;
	float tauRelease = 200.0f;
	float makeUpGain = 5.0f;

	compLow = std::unique_ptr<Compressor>(new Compressor(
		sampleRate,
		blockSize,
		threshold,
		ratio,
		tauAttack,
		tauRelease,
		makeUpGain
	));

	compMid = std::unique_ptr<Compressor>(new Compressor(
		sampleRate,
		blockSize,
		threshold,
		ratio,
		tauAttack,
		tauRelease,
		makeUpGain
	));

	compHigh = std::unique_ptr<Compressor>(new Compressor(
		sampleRate,
		blockSize,
		threshold,
		ratio,
		tauAttack,
		tauRelease,
		makeUpGain
	));
}

Dafx_assignment_1AudioProcessor::~Dafx_assignment_1AudioProcessor()
{
	lowBuffer.reset();
	midBuffer.reset();
	highBuffer.reset();
	compLow.reset();
	compMid.reset();
	compHigh.reset();
	for (int i = 0; i < lowpassFilters.size(); i++) {
		lowpassFilters[i].reset();
		highpassFilters[i].reset();
	}
}

const String Dafx_assignment_1AudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool Dafx_assignment_1AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool Dafx_assignment_1AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool Dafx_assignment_1AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double Dafx_assignment_1AudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int Dafx_assignment_1AudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int Dafx_assignment_1AudioProcessor::getCurrentProgram()
{
	return 0;
}

void Dafx_assignment_1AudioProcessor::setCurrentProgram(int index)
{
}

const String Dafx_assignment_1AudioProcessor::getProgramName(int index)
{
	return {};
}

void Dafx_assignment_1AudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void Dafx_assignment_1AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	compLow->setSampleRate(sampleRate);
	compLow->setBufferSize(samplesPerBlock);
	compMid->setSampleRate(sampleRate);
	compMid->setBufferSize(samplesPerBlock);
	compHigh->setSampleRate(sampleRate);
	compHigh->setBufferSize(samplesPerBlock);

	lowBuffer->setSize(getNumOutputChannels(), samplesPerBlock);
	midBuffer->setSize(getNumOutputChannels(), samplesPerBlock);
	highBuffer->setSize(getNumOutputChannels(), samplesPerBlock);
}

void Dafx_assignment_1AudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Dafx_assignment_1AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void Dafx_assignment_1AudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;

	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	float numSamples = 0.0;

	// Calculate and set current filter coefficients (based on cutoff frequencies)
	for (int i = 0; i < lowpassFilters.size(); i++) {
		lowpassFilters[i]->setCoefficients(IIRCoefficients::makeLowPass(getSampleRate(), lowpassCutoff, FILTER_Q));
		highpassFilters[i]->setCoefficients(IIRCoefficients::makeHighPass(getSampleRate(), highpassCutoff, FILTER_Q));
	}

	// Clear band buffers
	lowBuffer->clear();
	midBuffer->clear();
	highBuffer->clear();

	// Loop over channels and perform filtering
	for (int channel = 0; channel < totalNumOutputChannels; ++channel)
	{
		// Copy samples to buffers
		numSamples = buffer.getNumSamples();
		lowBuffer->copyFrom(channel, 0, buffer, channel, 0, numSamples);
		midBuffer->copyFrom(channel, 0, buffer, channel, 0, numSamples);
		highBuffer->copyFrom(channel, 0, buffer, channel, 0, numSamples);

		// Perform filtering
		lowpassFilters[channel]->processSamples(lowBuffer->getWritePointer(channel), numSamples);
		highpassFilters[channel]->processSamples(highBuffer->getWritePointer(channel), numSamples);

		// Calculate mid band by subtracting low and high band from input signal
		midBuffer->addFrom(channel, 0, lowBuffer->getReadPointer(channel), numSamples, -1.0F);
		midBuffer->addFrom(channel, 0, highBuffer->getReadPointer(channel), numSamples, -1.0F);
	}

	// Compress the three separate bands
	compLow->processBlock(*lowBuffer.get(), totalNumOutputChannels);
	compMid->processBlock(*midBuffer.get(), totalNumOutputChannels);
	compHigh->processBlock(*highBuffer.get(), totalNumOutputChannels);

	// Clear main buffer and send compressed signal back
	buffer.clear();

	// If all channels are soloed, play all streams back
	if (noSolo()) {
		for (int i = 0; i < soloStates.size(); i++) {
			auto currentBuffer = i == 0 ? *lowBuffer : (i == 1 ? *midBuffer : *highBuffer);
			buffer.addFrom(0, 0, currentBuffer, 0, 0, numSamples);
			buffer.addFrom(1, 0, currentBuffer, 1, 0, numSamples);
		}
	}
	else {
		for (int i = 0; i < soloStates.size(); i++) {
			auto currentBuffer = i == 0 ? *lowBuffer : (i == 1 ? *midBuffer : *highBuffer);
			if (soloStates[i]) {
				buffer.addFrom(0, 0, currentBuffer, 0, 0, numSamples);
				buffer.addFrom(1, 0, currentBuffer, 1, 0, numSamples);
			}
		}
	}
}

void Dafx_assignment_1AudioProcessor::setSoloState(bool states[3]) {
	for (int i = 0; i < soloStates.size(); i++) {
		soloStates[i] = states[i];
	}
}

bool Dafx_assignment_1AudioProcessor::noSolo() {
	for (int i = 0; i < soloStates.size(); i++) {
		if (soloStates[i]) {
			return false;
		}
	}
	return true;
}

Compressor* Dafx_assignment_1AudioProcessor::getCompLow() {
	return compLow.get();
}

Compressor* Dafx_assignment_1AudioProcessor::getCompMid() {
	return compMid.get();
}

Compressor* Dafx_assignment_1AudioProcessor::getCompHigh() {
	return compHigh.get();
}

//==============================================================================
bool Dafx_assignment_1AudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Dafx_assignment_1AudioProcessor::createEditor()
{
	return new Dafx_assignment_1AudioProcessorEditor(*this);
}

//==============================================================================
void Dafx_assignment_1AudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void Dafx_assignment_1AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new Dafx_assignment_1AudioProcessor();
}
