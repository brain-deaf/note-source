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
#include "FxSelector.h"
#include "IIR_Filter.h"
//#include <fftw3.h>

class NoteEvent{
public:
    int getNoteNumber(){return noteNumber;}
    float getVelocity(){return velocity;}
    Array<int> getGroups(){return groups;}
    
    void setNoteNumber(int n){noteNumber=n;}
    void setVelocity(float v){velocity=v;}
    void setGroups(Array<int> g){groups=g;}
private:
    int noteNumber;
    float velocity;
    Array<int> groups;
};

class Sampler : public AudioSource
{
public:
    Sampler();
    void addSample(String path, int root_note, int note_low, int note_high, Array<int>&, double);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void setMidiChannel(int i){midi_input_id = i;}
    void setFxSelector(FxSelector* f){fx_selector=f;}
    FxSelector* getFxSelector(){return fx_selector;}
    IIR_Filter* getFilter(){return &filter1;}
    
    Synthesiser* getSynth(){return &synth;}
    MidiMessageCollector& getMidiCollector(){return midiCollector;}
    Array<NoteEvent>& getEvents(){return events;}
    NoteEvent getLastEvent(){return events[-1];}
private:
    MidiMessageCollector midiCollector;
    Synthesiser synth;
    AudioFormatManager formatManager;
    int midi_input_id;
    IIR_Filter filter1;
    IIR_Filter filter2;
    FxSelector* fx_selector;
    Array<NoteEvent> events;
    //Array<InstrumentMappingEditor::MappingEditorGraph::Zone*> zones;
};



class SampleVoice : public SamplerVoice
{
public:
    SampleVoice();
    void renderNextBlock(AudioSampleBuffer&, int startSample, int numSamples) override;
    void startNote(const int, const float, SynthesiserSound*, const int);
private:
    IIR_Filter filter1;
    IIR_Filter filter2;
    float samplePosition;
    float sampleStart;
    float pitchRatio;
    
    float attackTime;
    float attackCurve;
    float decayTime;
    float decayCurve;
    float sustain;
    float releaseTime;
    float releaseCurve;
    float releaseStart;
};

class SampleSound : public SamplerSound
{
public:
    SampleSound(const String& name, 
                AudioFormatReader& source,
                const BigInteger& midiNotes,
                int midiNoteForNormalPitch,
                double attackTimeSecs,
                double releaseTimeSecs,
                double maxSampleLengthSeconds,
                Array<int> group,
                FxSelector* fx) : 
                    SamplerSound(name, source, midiNotes, midiNoteForNormalPitch, 
                                 attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds),
                    groups(group), fx_selector(fx), sampleStart(0.0)
    {
        sampleRate = source.sampleRate;
        rootNote = midiNoteForNormalPitch;
    }
    Array<int> getGroups(){return groups;}
    typedef ReferenceCountedObjectPtr<SampleSound> Ptr;
    double getSampleRate(){return sampleRate;}
    int getRootNote(){return rootNote;}
    FxSelector* getFxSelector(){return fx_selector;}
    void setSampleStart(double d){sampleStart=d;}
    double getSampleStart(){return sampleStart;}
private:
    Array<int> groups;
    double sampleRate;
    int rootNote;
    FxSelector* fx_selector;
    double sampleStart;
};

                    
                    
                                    
    
    
    





#endif  // SAMPLER_H_INCLUDED
