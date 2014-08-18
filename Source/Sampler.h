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
#include "TransformSelector.h"
#include "TransformSelector.h"
#include "IIR_Filter.h"
#include <memory>
//#include <fftw3.h>

class NoteEvent{
public:
    NoteEvent() : noteNumber(0),
                  triggerNote(0),
                  velocity(1.0),
                  id(0),
                  volume(1.0),
                  groups(){}
    int getNoteNumber(){return noteNumber;}
    float getVelocity(){return velocity;}
    int getTriggerNote(){return triggerNote;}
    Array<int>& getGroups(){return groups;}
    
    void setNoteNumber(int n){noteNumber=n;}
    void setVelocity(float v){velocity=v;}
    void setTriggerNote(int n){triggerNote=n;}
    void setGroups(Array<int> g){groups=g;}
    void setVolume(double d){volume=d;}
    double getVolume(){return volume;}
    int getId(){return id;}
    void setId(int i){id=i;}
private:
    int noteNumber;
    int triggerNote;
    float velocity;
    float volume;
    int id;
    Array<int> groups;
};


    

class Sampler : public AudioSource
{
public:
    Sampler(SelectedItemSet<std::pair<int, int> >*);
    void addSample(String path, int root_note, int note_low, int note_high, Array<int>&, double, std::pair<int, int>);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void setMidiChannel(int i){midi_input_id = i;}
    void setFxSelector(FxSelector* f){fx_selector=f;}
    void setTransformSelector(TransformSelector* f){tf_selector=f;}
    FxSelector* getFxSelector(){return fx_selector;}
    TransformSelector* getTransformSelector(){return tf_selector;}
    IIR_Filter* getFilter(){return &filter1;}
    
    Synthesiser* getSynth(){return &synth;}
    MidiMessageCollector& getMidiCollector(){return midiCollector;}
    Array<std::shared_ptr<NoteEvent> >& getEvents(){return events;}
    Array<std::shared_ptr<NoteEvent> >& getIncomingEvents(){return incomingEvents;}
    std::shared_ptr<NoteEvent> getLastEvent(){return events[events.size()-1];}
    SelectedItemSet<std::pair<int, int> >* getNotesHeld(){return notesHeld;}
    int getIdCount(){return idCount;}
    void incIdCount(){idCount++;}
private:
    MidiMessageCollector midiCollector;
    Synthesiser synth;
    AudioFormatManager formatManager;
    int midi_input_id;
    IIR_Filter filter1;
    IIR_Filter filter2;
    FxSelector* fx_selector;
    TransformSelector* tf_selector;
    TransformSelector* transform_selector;
    Array<std::shared_ptr<NoteEvent> > events;
    Array<std::shared_ptr<NoteEvent> > incomingEvents;
    SelectedItemSet<std::pair<int, int> >* notesHeld;
    int idCount;
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
    
    std::shared_ptr<NoteEvent> noteEvent;
    double volume;
    double tf_volume;
    bool ringMod;
    double ringAmount;
    double currentAngle;
    double angleDelta;
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
                FxSelector* fx,
                TransformSelector* tf,
                Sampler* s,
                std::pair<int, int> v) : 
                    SamplerSound(name, source, midiNotes, midiNoteForNormalPitch, 
                                 attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds),
                                 groups(group), fx_selector(fx), tf_selector(tf),
                                 sampleStart(0.0), sampler(s),
                                 velocity(v)
    {
        sampleRate = source.sampleRate;
        rootNote = midiNoteForNormalPitch;
    }
    Array<int> getGroups(){return groups;}
    typedef ReferenceCountedObjectPtr<SampleSound> Ptr;
    double getSampleRate(){return sampleRate;}
    int getRootNote(){return rootNote;}
    std::pair<int, int> getVelocity(){return velocity;}
    FxSelector* getFxSelector(){return fx_selector;}
    TransformSelector* getTransformSelector(){return tf_selector;}
    void setSampleStart(double d){sampleStart=d;}
    double getSampleStart(){return sampleStart;}
    Sampler* getSampler(){return sampler;}
private:
    std::pair<int, int> velocity;
    Array<int> groups;
    double sampleRate;
    int rootNote;
    FxSelector* fx_selector;
    TransformSelector* tf_selector;
    double sampleStart;
    Sampler* sampler;
};

                    
                    
                                    
    
    
    





#endif  // SAMPLER_H_INCLUDED
