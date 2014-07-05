/*
  ==============================================================================

    Sampler.cpp
    Created: 28 Jun 2014 2:03:28pm
    Author:  patrick

  ==============================================================================
*/

#include "Sampler.h"


Sampler::Sampler() : AudioSource(), synth(), formatManager(), filter() {
    for (int i=0; i<16; i++){
        synth.addVoice(new SampleVoice());
    }
    formatManager.registerBasicFormats();
    filter.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 1000.0));
}
    
void Sampler::addSample(String path, int root_note, int note_low, int note_high){
    ScopedPointer<AudioFormatReader> audioReader(formatManager.createReaderFor(File(path)));
        
    BigInteger allNotes;
    for (int i=note_low; i<note_high; i++){
        allNotes.setBit(i);
    }
        
    synth.addSound(new SamplerSound("demo sound", *audioReader,
                                    allNotes, root_note,
                                    0.0, 0.0, 10.0));
}
    
void Sampler::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    midiCollector.reset(sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
}
    
void Sampler::releaseResources() {}
    
void Sampler::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    
    /*for (int i=0; i<bufferToFill.buffer->getNumChannels(); i++){
        filter.processSamples(bufferToFill.buffer->getWritePointer(i), bufferToFill.buffer->getNumSamples());
    }*/
    
}

SampleVoice::SampleVoice() : SamplerVoice(), filter(){
    filter.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 500.0));
}

void SampleVoice::renderNextBlock(AudioSampleBuffer& buffer, int startSample, int numSamples){
    SamplerVoice::renderNextBlock(buffer, startSample, numSamples);
    
    //filter.processSamples(buffer.getWritePointer(0), numSamples);
    //filter.processSamples(buffer.getWritePointer(1), numSamples);
    
    /*for (int i=0; i<buffer.getNumChannels(); i++){
        for (int j=startSample; j<numSamples; j++){
            //filter.processSingleSampleRaw(buffer.getWritePointer(i)[j]);
        }
    }*/
}

AudioPlayer::AudioPlayer(){
    filter.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 500.0));
}

void AudioPlayer::audioDeviceIOCallback(const float** inputChannelData, int totalNumInputChannels, 
                                        float** outputChannelData, int totalNumOutputChannels, int numSamples)
{
    AudioSourcePlayer::audioDeviceIOCallback(inputChannelData, totalNumInputChannels, outputChannelData, totalNumOutputChannels, numSamples);
    /*for (int i=0; i<totalNumOutputChannels; i++){
        for (int j=0; j<numSamples; j++){
            filter.processSingleSampleRaw(outputChannelData[i][j]);
        }
    }*/
    AudioSampleBuffer buffer(outputChannelData, totalNumOutputChannels, numSamples);
    filter.processSamples(buffer.getWritePointer(0), numSamples);
    filter.processSamples(buffer.getWritePointer(1), numSamples);
    filter.processSamples(buffer.getWritePointer(2), numSamples);
    filter.processSamples(buffer.getWritePointer(3), numSamples);
    filter.processSamples(buffer.getWritePointer(4), numSamples);
    filter.processSamples(buffer.getWritePointer(5), numSamples);
}