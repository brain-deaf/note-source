/*
  ==============================================================================

    Sampler.cpp
    Created: 28 Jun 2014 2:03:28pm
    Author:  patrick

  ==============================================================================
*/

#include "Sampler.h"


Sampler::Sampler() : AudioSource(), synth(), formatManager(), filter1(), filter2() {
    for (int i=0; i<16; i++){
        synth.addVoice(new SampleVoice());
    }
    formatManager.registerBasicFormats();
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
    filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
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
    
    //add filter processing to the output channels 1 and 2
    filter1.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getNumSamples());
    filter2.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.buffer->getNumSamples());
}

SampleVoice::SampleVoice() : SamplerVoice(), filter1(), filter2(){
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 50.0));
    filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 50.0));
}

void SampleVoice::renderNextBlock(AudioSampleBuffer& buffer, int startSample, int numSamples){
    SamplerVoice::renderNextBlock(buffer, startSample, numSamples);
}