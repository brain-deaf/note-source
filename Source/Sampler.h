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
    double volume;
    int id;
    Array<int> groups;
};



class SamplerEventProcessor;
class PlaySettings;
class SampleSound;

class SampleGroup
{
public:
    SampleGroup() : sounds(){}
    void add(SampleSound* s){sounds.add(s);}
    Array<SampleSound*>& getSounds(){return sounds;}
private:
    Array<SampleSound*> sounds;
};

class Sampler : public AudioSource
{
public:
    Sampler(SelectedItemSet<std::pair<int, int> >*);
    ~Sampler(){
		synth.clearSounds();
		synth.clearVoices();
		int y = synth.getNumSounds();
		groups.clear(true);
	}
    bool addSample(String path, int root_note, int note_low, int note_high, 
                   Array<int>&, PlaySettings*, std::pair<int, int>);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void setMidiChannel(int i){midi_input_id = i;}
    void setFxSelector(FxSelector* f){fx_selector=f;}
    void setTransformSelector(TransformSelector* f){tf_selector=f;}
    void setupRendering();
    void setSamplerProcessor(SamplerEventProcessor* s){samplerProcessor=s;}
    SamplerEventProcessor* getSamplerProcessor(){return samplerProcessor;}
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
    void incWavSampleCount(){wavSampleCount++;}
    void setWavSampleCount(){wavSampleCount=0;}
    long long getWavSampleCount(){return wavSampleCount;}
    AudioFormatWriter* getWavWriter(){return wavWriter;}
    float getPeak(){return peak;}
    void setInstrumentVolume(double d){instrumentVolume=d;}
    double getInstrumentVolume(){return instrumentVolume;}
    OwnedArray<SampleGroup>& getGroups(){return groups;}
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
    WavAudioFormat* wavFormat;
    FileOutputStream* wavOutput;
    AudioFormatWriter* wavWriter;
    SamplerEventProcessor* samplerProcessor;
    long long wavSampleCount;
    float peak;
    double instrumentVolume;
    OwnedArray<SampleGroup> groups;
};

class SampleVoice : public SamplerVoice
{
public:
    SampleVoice();
    void renderNextBlock(AudioSampleBuffer&, int startSample, int numSamples) override;
    void startNote(int, float, SynthesiserSound*, int);
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
    bool bb;
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
                                 sampleStart(0.0), loopStart(0.0), loopEnd(0.0), 
                                 xfadeLength(0.0), loopMode(false), tuning(0.0),
                                 sampler(s), velocity(v)
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
    
    void setLoopStart(double d){loopStart=d;}
    void setLoopEnd(double d){loopEnd=d;}
    void setLoopMode(bool b){loopMode = b;}
    void setXfadeLength(double d){xfadeLength = d;}
    
    bool getLoopMode(){return loopMode;}
    double getLoopStart(){return loopStart;}
    double getLoopEnd(){return loopEnd;}
    double getXfadeLength(){return xfadeLength;}
    void setTuning(double d){tuning=d;}
    double getTuning(){return tuning;}
    
    //Array<int>& getGroups(){return groups;}
    void setGroup(int index, int value){
        groups.set(index, value);
    }
    
    Sampler* getSampler(){return sampler;}
private:
    std::pair<int, int> velocity;
    Array<int> groups;
    double sampleRate;
    int rootNote;
    FxSelector* fx_selector;
    TransformSelector* tf_selector;
    double sampleStart;
    double loopStart;
    double loopEnd;
    bool loopMode;
    double xfadeLength;
    Sampler* sampler;
    double tuning;
};

                    
                    
                                    
    
    
    





#endif  // SAMPLER_H_INCLUDED
