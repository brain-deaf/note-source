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
#include "IIR_Filter.h"
#include <memory>
//#include <fftw3.h>
class RoundRobinPlayback;
class SamplerProcessor;
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
	bool releaseTriggered;
    
    std::shared_ptr<NoteEvent> noteEvent;
    double volume;
    double tf_volume;
    bool ringMod;
    double ringAmount;
    double currentAngle;
    double angleDelta;
    bool bb;
	int rr;
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
                SamplerProcessor* s,
                std::pair<int, int> v) : 
                    SamplerSound(name, source, midiNotes, midiNoteForNormalPitch, 
                                 attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds),
                                 groups(group), fx_selector(fx), tf_selector(tf),
								 sampleStart(0.0), loopStart(0.0), loopEnd(0.0), releaseMode(false),
                                 xfadeLength(0.0), xfadeCurve(0.01), loopMode(false), tuning(0.0),
								 sampler(s), velocity(v), releaseStart(0.0), releaseTime(44100.0 / 8.0), 
								 releaseCurve(0.01), roundRobin(-1), roundRobinProcessor(nullptr)
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
	void setReleaseStart(double d){ releaseStart = d; }
	double getReleaseStart(){ return releaseStart; }
	void setReleaseTime(double d){ releaseTime = d; }
	double getReleaseTime(){ return releaseTime; }
	void setReleaseCurve(double d){ releaseCurve = d; }
	double getReleaseCurve(){ return releaseCurve; }

    
    void setLoopStart(double d){loopStart=d;}
    void setLoopEnd(double d){loopEnd=d;}
    void setLoopMode(bool b){loopMode = b;}
	void setReleaseMode(bool b){ releaseMode = b; }
    void setXfadeLength(double d){xfadeLength = d;}
	void setXfadeCurve(double d){ xfadeCurve= d; }
    
    bool getLoopMode(){return loopMode;}
	bool getReleaseMode(){ return releaseMode; }
    double getLoopStart(){return loopStart;}
    double getLoopEnd(){return loopEnd;}
    double getXfadeLength(){return xfadeLength;}
	double getXfadeCurve(){ return xfadeCurve; }
    void setTuning(double d){tuning=d;}
    double getTuning(){return tuning;}
    
    //Array<int>& getGroups(){return groups;}
    void setGroup(int index, int value){
        groups.set(index, value);
    }
    
    SamplerProcessor* getSampler(){return sampler;}
	void setRoundRobin(int i){ roundRobin = i; }
	int getRoundRobin(){ return roundRobin; }
	void setRoundRobinProcessor(RoundRobinPlayback* r){ roundRobinProcessor = r; }
	RoundRobinPlayback* getRoundRobinProcessor(){ return roundRobinProcessor; }
private:
    std::pair<int, int> velocity;
    Array<int> groups;
    double sampleRate;
    int rootNote;
    FxSelector* fx_selector;
    TransformSelector* tf_selector;
    double sampleStart;
	double releaseStart;
	double releaseTime;
	double releaseCurve;
    double loopStart;
    double loopEnd;
    bool loopMode;
	bool releaseMode;
    double xfadeLength;
	double xfadeCurve;
    SamplerProcessor* sampler;
    double tuning;
	int roundRobin;
	RoundRobinPlayback* roundRobinProcessor;
};

                    
                    
                                    
    
    
    





#endif  // SAMPLER_H_INCLUDED
