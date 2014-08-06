/*
  ==============================================================================

    Metronome.h
    Created: 5 Aug 2014 12:49:11pm
    Author:  patrick

  ==============================================================================
*/

#ifndef METRONOME_H_INCLUDED
#define METRONOME_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

class MetronomeSound : public SynthesiserSound
{
public:
    MetronomeSound();
    ~MetronomeSound(){}
    bool appliesToNote(int n){return true;}
    bool appliesToChannel(int n){return true;}
    Array<double> getData(){return data;}
private:
    Array<double> data;
};

class MetronomeVoice : public SynthesiserVoice
{
public: 
    MetronomeVoice();
    ~MetronomeVoice();
    void aftertouchChanged(int i){}
    bool canPlaySound(SynthesiserSound* s){return true;}
    void startNote(int i, float f, SynthesiserSound* s, int d){}
    void stopNote(bool t){}
    void pitchWheelMoved(int i){}
    void controllerMoved(int i, int d){}
    void setClick(bool b){clickOn=b;}
    void setTransport(bool b){transportRunning=b;}
    void setTempo(double d){tempo=d;}
    void renderNextBlock(AudioSampleBuffer& outputBuffer, int start, int numSamples) override;
private:
    int sampleCount;
    int measureCount;
    double currentAngle;
    double angleDelta;
    int beepLength;
    double releaseMultiplier;
    double tempo;
    bool transportRunning;
    bool clickOn;
};

class Metronome : public AudioSource
{
public:
    Metronome();
    ~Metronome(){delete synth;}
    void releaseResources(){}
    void getNextAudioBlock(const AudioSourceChannelInfo&) override;
    void prepareToPlay(int, double) override;
    Synthesiser* getSynth(){return synth;}
private:
    double sampleRate;
    double sampleCount;
    bool clickOn;
    bool transportRunning;
    double tempo;
    Synthesiser* synth;
    MidiMessageCollector midiCollector;
};


    
        


#endif  // METRONOME_H_INCLUDED
