/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Sampler.h"
#include "MainComponent.h"
#include "InstrumentMappingEditor.h"
#include "RoundRobin.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter();


const float defaultGain = 1.0f;
const float defaultDelay = 0.5f;

//==============================================================================
SamplerProcessor::SamplerProcessor() : synth(), idCount(0), formatManager(),
															   events(), incomingEvents(), wavFormat(nullptr), 
															   wavWriter(nullptr),/*wavOutput(new FileOutputStream(File(File::getCurrentWorkingDirectory()).getFullPathName() + "/temp.wav")),*/
															   filter1(), filter2(), fx_selector(nullptr),
															   wavSampleCount(0.0), wavOutput(nullptr), samplerProcessor(new SamplerEventProcessor()),
															   instrumentVolume(1.0), groups(), midiCallback(nullptr)
{
    // Set up some default values..

    lastUIWidth = 400;
    lastUIHeight = 200;

    lastPosInfo.resetToDefault();

	for (int i = 0; i<256; i++){
		synth.addVoice(new SampleVoice());
	}
	formatManager.registerBasicFormats();
	filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 6000.0));
	filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 10000.0));

	groups.add(new SampleGroup());
}

SamplerProcessor::~SamplerProcessor()
{
	synth.clearSounds();
	synth.clearVoices();
	//int y = synth.getNumSounds();
	groups.clear(true);
}


void SamplerProcessor::setupRendering(){
	File f = File(File::getCurrentWorkingDirectory().getFullPathName() + "/temp.wav");
	if (f.exists())
		f.deleteFile();
}

bool SamplerProcessor::addSample(String path, int root_note, int note_low, int note_high, Array<int>& group, PlaySettings* p, std::pair<int, int> v, RoundRobinPlayback* r, int rr){
	ScopedPointer<AudioFormatReader> audioReader(formatManager.createReaderFor(File(path)));
	if (audioReader == nullptr){
		return false;
	}
	BigInteger allNotes;
	for (int i = note_low; i<note_high; i++){
		allNotes.setBit(i);
	}
	SampleSound* ss;
	synth.addSound(ss = new SampleSound("demo sound", *audioReader,
		allNotes, root_note,
		0.0, 0.0, 10.0, group, fx_selector, tf_selector, this, v));
	ss->setSampleStart(p->getSampleStart());
	ss->setReleaseStart(p->getReleaseStart());
	ss->setReleaseTime(p->getReleaseTime());
	ss->setReleaseCurve(p->getReleaseCurve());
	ss->setLoopMode(p->getLoopMode());
	ss->setReleaseMode(p->getReleaseMode());
	ss->setLoopStart(p->getLoopStart());
	ss->setLoopEnd(p->getLoopEnd());
	ss->setXfadeLength(p->getXfadeLength());
	ss->setXfadeCurve(p->getXfadeCurve());
	ss->setRoundRobinProcessor(r);

	if (rr != -1){
		ss->setRoundRobin(rr);
	}

	groups[group[0]]->add(ss);
	audioReader = nullptr;
	return true;
}

/*void Sampler::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
	bufferToFill.clearActiveBufferRegion();
	MidiBuffer incomingMidi;
	midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

	synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);

	peak = 0.0;
	for (int i = 0; i<bufferToFill.numSamples; i++){
		if (bufferToFill.buffer->getWritePointer(0)[i] > peak){
			peak = bufferToFill.buffer->getWritePointer(0)[i];
		}
	}
}*/

//==============================================================================
int SamplerProcessor::getNumParameters()
{
    return totalNumParams;
}

float SamplerProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    /*switch (index)
    {
        case gainParam:     return gain;
        case delayParam:    return delay;
        default:            return 0.0f;
    }*/

	return 0.0f;
}

void SamplerProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    /*switch (index)
    {
        case gainParam:     gain = newValue;  break;
        case delayParam:    delay = newValue;  break;
        default:            break;
    }*/
}

float SamplerProcessor::getParameterDefaultValue (int index)
{
    /*switch (index)
    {
        case gainParam:     return defaultGain;
        case delayParam:    return defaultDelay;
        default:            break;
    }*/

    return 0.0f;
}

const String SamplerProcessor::getParameterName (int index)
{
    switch (index)
    {
        case gainParam:     return "gain";
        case delayParam:    return "delay";
        default:            break;
    }

    return String::empty;
}

const String SamplerProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

//==============================================================================
void SamplerProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
	midiCollector.reset(sampleRate);
	synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SamplerProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void SamplerProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
}

void SamplerProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	midiBuffer.clear();
	tfMidiBuffer.clear();
	midiBuffer = midiMessages;
	tfMidiBuffer = midiMessages;
	//buffer.clear();
	//MidiBuffer incomingMidi;
	//midiCollector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());
	if (midiCallback != nullptr){
		if (!midiMessages.isEmpty()){
			midiCallback->handleMidiBuffer(midiMessages);
			tf_selector->setMidiBuffer(tfMidiBuffer);
		}
	}
	int x = midiBuffer.getNumEvents();
	synth.renderNextBlock(buffer, midiBuffer, 0, buffer.getNumSamples());

	peak = 0.0;
	for (int i = 0; i<buffer.getNumSamples(); i++){
		if (buffer.getWritePointer(0)[i] > peak){
			peak = buffer.getWritePointer(0)[i];
		}
	}
}

//==============================================================================
AudioProcessorEditor* SamplerProcessor::createEditor()
{
    return new JuceDemoPluginAudioProcessorEditor (*this);
}

//==============================================================================
void SamplerProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");

    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);

    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void SamplerProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth  = xmlState->getIntAttribute ("uiWidth", lastUIWidth);
            lastUIHeight = xmlState->getIntAttribute ("uiHeight", lastUIHeight);
        }
    }
}

const String SamplerProcessor::getInputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

const String SamplerProcessor::getOutputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

bool SamplerProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool SamplerProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool SamplerProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SamplerProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SamplerProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double SamplerProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SamplerProcessor();
}