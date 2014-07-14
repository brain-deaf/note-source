/*
  ==============================================================================

    Sampler.cpp
    Created: 28 Jun 2014 2:03:28pm
    Author:  patrick

  ==============================================================================
*/

#include "Sampler.h"


Sampler::Sampler() : AudioSource(), synth(), formatManager(), filter1(), filter2() {
    for (int i=0; i<256; i++){
        synth.addVoice(new SampleVoice());
    }
    formatManager.registerBasicFormats();
    /*filter1.setCoefficients(IIRCoefficients::makeNothing(44100.0, 7000.0));
    filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));*/
    
    filter1.setCoefficients(IIRCoefficients::makeNothing());
    filter2.setCoefficients(IIRCoefficients::makeNothing());
}
    
void Sampler::addSample(String path, int root_note, int note_low, int note_high, Array<int>& groups){
    ScopedPointer<AudioFormatReader> audioReader(formatManager.createReaderFor(File(path)));
        
    BigInteger allNotes;
    for (int i=note_low; i<note_high; i++){
        allNotes.setBit(i);
    }
        
    synth.addSound(new SampleSound("demo sound", *audioReader,
                                    allNotes, root_note,
                                    0.0, 0.0, 10.0, groups));
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

SampleVoice::SampleVoice() : SamplerVoice(), filter1(), filter2(), samplePosition(0.0f),
                             attackTime(0.1), autoReleaseTime(0.1){
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 500.0));
    filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 500.0));
}

void SampleVoice::startNote(const int midiNoteNumber,
                            const float velocity,
                            SynthesiserSound* s,
                            const int pitchWheelPosition){
    if (SampleSound* sound = (SampleSound*)s){
        const double a = pow(2.0, 1.0/12.0);
        double old_frequency = 440.0 * pow(a, (double)sound->getRootNote() - 69.0);
        double new_frequency = old_frequency * pow(a, (float)midiNoteNumber - sound->getRootNote());
        pitchRatio = new_frequency/old_frequency;
    }
}

void SampleVoice::renderNextBlock(AudioSampleBuffer& buffer, int startSample, int numSamples){
    SampleSound::Ptr s = (SampleSound::Ptr)getCurrentlyPlayingSound();
    if (s != nullptr){
        Array<int> groups_for_note = s->getGroups();
        //example of how to not process a sound for a particular Group
        //if (groups_for_note[0] == 0){return;}
    
    
    //SamplerVoice::renderNextBlock(buffer, startSample, numSamples);
    
    //process filter if group 0
    /*if (s != nullptr){
        Array<int> groups_for_note = s->getGroups();
        if (groups_for_note[0] == 0){
            filter1.processSamples(buffer.getWritePointer(0), buffer.getNumSamples());
            filter2.processSamples(buffer.getWritePointer(1), buffer.getNumSamples());
        }
    }*/
        float gain = 1.0f;
        double sample_length = s->getAudioData()->getNumSamples();
        int num_channels = s->getAudioData()->getNumChannels();
        if (getCurrentlyPlayingSound().get()){
            const float* const inL = s->getAudioData()->getReadPointer(0);
            const float* const inR = num_channels>1 ? s->getAudioData()->getReadPointer(1) : nullptr;
            float* outL = buffer.getWritePointer(0, startSample);
            float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1, startSample) : nullptr;
    
            
            int start = samplePosition;
            for (int i= start; i<numSamples+start; i++){
                double attack_multiplier = samplePosition/s->getSampleRate()<attackTime ? samplePosition/(attackTime*s->getSampleRate()) : 1.0;
                double samples_left = sample_length - samplePosition;
                double release_multiplier = samples_left <= autoReleaseTime*s->getSampleRate() ? samples_left/(autoReleaseTime*s->getSampleRate()) : 1.0;
            
                const int pos = (int) samplePosition;
                const float alpha = (float) (samplePosition - pos);
                const float invAlpha = 1.0f - alpha;
                
                // just using a very simple linear interpolation here..
                //std::cout<<inL[pos]<<std::endl;
                float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
                
                float r = inR != nullptr ? inR[pos] * invAlpha + inR[pos + 1] * alpha : l;
                
                //std::cout<<l<<std::endl;

                *outL += l*attack_multiplier*release_multiplier;
                *outR += r*attack_multiplier*release_multiplier;
                outL++;
                outR++;
            
                samplePosition += pitchRatio;
                //std::cout<<pitchRatio<<std::endl;
                //samplePosition++;
                if (samplePosition > sample_length || sample_length - samplePosition < numSamples){
                    samplePosition = 0.0;
                    stopNote(false);
                    break;
                }
            }
        }
    }
}