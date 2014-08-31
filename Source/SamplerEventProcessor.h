/*
  ==============================================================================

    SamplerEventProcessor.h
    Created: 30 Aug 2014 1:32:11pm
    Author:  patrick

  ==============================================================================
*/

#ifndef SAMPLEREVENTPROCESSOR_H_INCLUDED
#define SAMPLEREVENTPROCESSOR_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "Sampler.h"

class SamplerEvent
{
public:
    SamplerEvent() : noteNumber(0),
                  velocity(1.0),
                  id(0),
                  volume(1.0),
                  start(0),
                  end(120000),
                  groups(){}
    int getNoteNumber(){return noteNumber;}
    float getVelocity(){return velocity;}
    Array<int>& getGroups(){return groups;}
    
    void setNoteNumber(int n){noteNumber=n;}
    void setVelocity(float v){velocity=v;}
    void setGroups(Array<int> g){groups=g;}
    void setVolume(double d){volume=d;}
    void setStart(long long l){start=l;}
    void setEnd(long long l){end=l;}
    long long getStart(){return start;}
    long long getEnd(){return end;}
    double getVolume(){return volume;}
    int getId(){return id;}
    void setId(int i){id=i;}
private:
    int noteNumber;
    int triggerNote;
    float velocity;
    float volume;
    int id;
    long long start;
    long long end;
    Sampler* sampler;
    Array<int> groups;
};


typedef std::vector<std::vector<float> > Data;

class SamplerEventProcessor
{
public:
    SamplerEventProcessor(Sampler* s) : sampler(s), sampleCount(0), events(),samplePosition(0.0f),
                          attackTime(10.0), attackCurve(0.05), releaseTime(50.0), 
                          sampleStart(0.0), releaseCurve(0.01), volume(1.0), ringMod(false),
                          tf_volume(1.0), ringAmount(1.0), angleDelta(0.0),
                          currentAngle(0.0), sound(nullptr), maxSampleCount(0), data()
    {
        s->setSamplerProcessor(this);
    }
    ~SamplerEventProcessor(){/*delete data[0]; delete data[1]; delete data;*/}
    void renderSamplerEvent(SamplerEvent, Data&);
    void startSamplerEvent(SamplerEvent);
    void addSamplerEvent(SamplerEvent s){events.add(s);}
    void clearAllSamplerEvents(){events.clear();}
    void renderAllEvents();
    long long getDataSize(){return data[0].size();}
    int getNumEvents(){return events.size();}
private:
    Sampler* sampler;
    Array<SamplerEvent> events;
    long long sampleCount;
    long long maxSampleCount;
    Data data;
    SampleSound* sound;
    
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
    double volume;
    double tf_volume;
    bool ringMod;
    double ringAmount;
    double currentAngle;
    double angleDelta;
};




#endif  // SAMPLEREVENTPROCESSOR_H_INCLUDED
