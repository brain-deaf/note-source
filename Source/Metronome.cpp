/*
  ==============================================================================

    Metronome.cpp
    Created: 5 Aug 2014 12:49:03pm
    Author:  patrick

  ==============================================================================
*/

#include "Metronome.h"
#include <math.h>

Metronome::Metronome() : AudioSource(), synth(new Synthesiser), midiCollector(),
                         isOn(false)
{
    MetronomeVoice* v;
    synth->addVoice(v = new MetronomeVoice());
    v->setMetronome(isOn);
    synth->addSound(new MetronomeSound());
}

void Metronome::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    midiCollector.reset(sampleRate);
    synth->setCurrentPlaybackSampleRate(sampleRate);
}

void Metronome::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

    synth->renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
}

MetronomeSound::MetronomeSound() : SynthesiserSound(), data()
{
    //y = sin(2PI*frequency*x)
    //samples_per_cycle = samples/sec[srate] / cyles/sec[hz] = samples/cycle
    /*double frequency = 440.0;
    double samples_per_cycle = 44100.0 / frequency;
    double sound_length = 0.2; //seconds
    double total_samples = 44100.0 * sound_length;
    
    for (int i=0; i<total_samples; i++){
        data.add(sin(2.0*M_PI*frequency*(double((i)/44100.0))));
    }*/
}

MetronomeVoice::MetronomeVoice() : SynthesiserVoice(), sampleCount(0), 
                         beepLength(4410), measureCount(0), releaseMultiplier(1.0) 
{
    currentAngle = 0.0;
    double cyclesPerSecond = 440.0;
    double cyclesPerSample = cyclesPerSecond/44100.0;
    angleDelta = cyclesPerSample*2.0*M_PI*double_Pi;
}
MetronomeVoice::~MetronomeVoice(){}

void MetronomeVoice::renderNextBlock(AudioSampleBuffer& outputBuffer, int start, int numSamples){
    MetronomeSound::Ptr s = (MetronomeSound::Ptr)getCurrentlyPlayingSound();
    if (s != nullptr){
        //samples_per_cycle = samples/sec[srate] / cyles/sec[hz] = samples/cycle
    }
    float* outL = outputBuffer.getWritePointer(0, start);
    float* outR = outputBuffer.getWritePointer(1, start);
    
    if (isOn){
        for (int i=0; i<numSamples; i++){
            if (sampleCount >= beepLength - 100 && sampleCount < beepLength){
                releaseMultiplier = 1.0 - (100.0-(beepLength - sampleCount)) / 100.0;
            }else{
                releaseMultiplier = 1.0;
            }
            //if (releaseMultiplier != 1.0)
            //std::cout<<releaseMultiplier<<" beep length: "<<beepLength<<" sample count: "<<sampleCount<<std::endl;
        
            if (sampleCount < beepLength){
                *outL += (float)(sin(currentAngle))*releaseMultiplier;
                *outR += (float)(sin(currentAngle))*releaseMultiplier;
                currentAngle += angleDelta;
            }
        
            sampleCount++;
            measureCount++;
        
            if (measureCount >= 44100*4){ 
                sampleCount = 0;
                measureCount = 0;
                currentAngle = 0.0;
                double cyclesPerSecond = 440.0;
                double cyclesPerSample = cyclesPerSecond/44100.0;
                angleDelta = cyclesPerSample*2.0*M_PI*double_Pi;
            }else if (sampleCount >= 44100*1){ 
                sampleCount = 0;
                currentAngle = 0.0;
                double cyclesPerSecond = 220.0;
                double cyclesPerSample = cyclesPerSecond/44100.0;
                angleDelta = cyclesPerSample*2.0*M_PI*double_Pi;
            }
            outL++;
            outR++;
        }
    }
}