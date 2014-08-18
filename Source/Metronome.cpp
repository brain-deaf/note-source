/*
  ==============================================================================

    Metronome.cpp
    Created: 5 Aug 2014 12:49:03pm
    Author:  patrick

  ==============================================================================
*/

#include "Metronome.h"
#include <math.h>
#include "LuaScript.h"

Metronome::Metronome() : AudioSource(), synth(new Synthesiser), midiCollector(),
                         clickOn(false), transportRunning(false), tempo(120.0)
{
    MetronomeVoice* v;
    synth->addVoice(v = new MetronomeVoice());
    v->setClick(clickOn);
    v->setTransport(transportRunning);
    v->setTempo(tempo);
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

}

MetronomeVoice::MetronomeVoice() : SynthesiserVoice(), sampleCount(0), volume(0.5),
                         beepLength(4410), measureCount(4), releaseMultiplier(1.0),
                         luaScript(nullptr), firstFlag(true)
{
    currentAngle = 0.0;
    double cyclesPerSecond = 440.0;
    double cyclesPerSample = cyclesPerSecond/44100.0;
    angleDelta = cyclesPerSample*2.0*M_PI*double_Pi;
    //y = sin(2PI*frequency*x)
}
MetronomeVoice::~MetronomeVoice(){}

void MetronomeVoice::renderNextBlock(AudioSampleBuffer& outputBuffer, int start, int numSamples){
    MetronomeSound::Ptr s = (MetronomeSound::Ptr)getCurrentlyPlayingSound();
    if (s != nullptr){
        //samples_per_cycle = samples/sec[srate] / cyles/sec[hz] = samples/cycle
    }
    float* outL = outputBuffer.getWritePointer(0, start);
    float* outR = outputBuffer.getWritePointer(1, start);
    
    
    
    double samples_per_beat = 44100.0 / (tempo/60.0);
    if (transportRunning){
        for (int i=0; i<numSamples; i++){
            if (sampleCount >= beepLength - 100 && sampleCount < beepLength){
                releaseMultiplier = 1.0 - (100.0-(beepLength - sampleCount)) / 100.0;
            }else{
                releaseMultiplier = 1.0;
            }
        
            if (sampleCount < beepLength){
                if (clickOn){
                    *outL += (float)(sin(currentAngle))*releaseMultiplier*volume;
                    *outR += (float)(sin(currentAngle))*releaseMultiplier*volume;
                    currentAngle += angleDelta;
                }
            }
        
            sampleCount++;
            
        
            if (measureCount >= 4){         
                sampleCount = 0;
                measureCount = 0;
                currentAngle = 0.0;
                
                if (luaScript != nullptr && firstFlag){
                    luaScript->onBeat();
                    firstFlag = false;
                }
                
                double cyclesPerSecond = 440.0;
                double cyclesPerSample = cyclesPerSecond/44100.0;
                angleDelta = cyclesPerSample*2.0*M_PI*double_Pi;
                
            }else if (sampleCount >= samples_per_beat){ 
                if (luaScript != nullptr){
                    luaScript->onBeat();
                }
            
                measureCount++;
                sampleCount = 0;
                currentAngle = 0.0;
                double cyclesPerSecond = 220.0;
                double cyclesPerSample = cyclesPerSecond/44100.0;
                angleDelta = cyclesPerSample*2.0*M_PI*double_Pi;
            }
            outL++;
            outR++;
        }
    }else{
    }
}