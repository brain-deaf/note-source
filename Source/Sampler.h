/*
  ==============================================================================

    Sampler.h
    Created: 28 Jun 2014 2:03:47pm
    Author:  patrick

  ==============================================================================
*/

#ifndef SAMPLER_H_INCLUDED
#define SAMPLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


class Sampler : public AudioSource
{
public:
    Sampler() : AudioSource(), synth() {
        for (int i=0; i<4; i++){
            synth.addVoice(new SamplerVoice());
        }
    }
    
    void setSample(){
        WavAudioFormat wavFormat;
        ScopedPointer<AudioFormatReader> audioReader
            (wavFormat.createReaderFor(new FileInputStream(File("/home/patrick/Juce/projects/bowl1.wav")), true));;
        BigInteger allNotes;
        allNotes.setRange(0, 128, true);
        
        synth.clearSounds();
        synth.addSound(new SamplerSound("demo sound",
                                        *audioReader,
                                        allNotes,
                                        74,
                                        0.1,
                                        0.1,
                                        10.0));
    }
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        midiCollector.reset(sampleRate);
        synth.setCurrentPlaybackSampleRate(sampleRate);
    }
    
    void releaseResources() override {}
    
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override {
        bufferToFill.clearActiveBufferRegion();
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }
    
    Synthesiser* getSynth(){return &synth;}
    MidiMessageCollector& getMidiCollector(){return midiCollector;}
        
private:
    MidiMessageCollector midiCollector;
    Synthesiser synth;
};



#endif  // SAMPLER_H_INCLUDED
