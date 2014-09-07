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

class LuaScript;

class MetronomeSound : public SynthesiserSound
{
public:
    MetronomeSound();
    ~MetronomeSound(){}
    bool appliesToNote(const int){return true;}
    bool appliesToChannel(const int){return true;}
    Array<double> getData(){return data;}
private:
    Array<double> data;
};

class MetronomeVoice : public SynthesiserVoice
{
public: 
    MetronomeVoice();
    ~MetronomeVoice();
    void aftertouchChanged(int){}
    bool canPlaySound(SynthesiserSound*){return true;}
    void startNote(int, float, SynthesiserSound*, int){}
    void stopNote(bool ){}
    void pitchWheelMoved(int ){}
    void controllerMoved(int , int ){}
    void setClick(bool b){clickOn=b;}
    void setTransport(bool b){transportRunning=b;}
    void setTempo(double d){tempo=d;}
    void setVolume(double v){volume=v;}
    double getSamplesPerBeat(){return samples_per_beat;}
    int getSampleCount(){return sampleCount;}
    int getMeasureCount(){return measureCount;}
    void reset(){sampleCount=0; measureCount=4; firstFlag = true;}
    void setLuaScript(LuaScript* l){luaScript=l;}
    void renderNextBlock(AudioSampleBuffer& outputBuffer, int start, int numSamples) override;
private:
    int sampleCount;
    double samples_per_beat;
    int measureCount;
    double currentAngle;
    double angleDelta;
    int beepLength;
    double releaseMultiplier;
    double tempo;
    bool transportRunning;
    bool clickOn;
    double volume;
    LuaScript* luaScript;
    bool firstFlag;
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
    bool clickOn;
    bool transportRunning;
    double tempo;
    Synthesiser* synth;
    MidiMessageCollector midiCollector;
};


    
        


#endif  // METRONOME_H_INCLUDED
