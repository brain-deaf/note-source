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

class Sampler : public AudioSource
{
public:
    Sampler();
    void addSample(String path, int root_note, int note_low, int note_high, Array<int>&);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void setMidiChannel(int i){midi_input_id = i;}
    void setFxSelector(FxSelector* f){fx_selector=f;}
    FxSelector* getFxSelector(){return fx_selector;}
    
    Synthesiser* getSynth(){return &synth;}
    MidiMessageCollector& getMidiCollector(){return midiCollector;}
private:
    MidiMessageCollector midiCollector;
    Synthesiser synth;
    AudioFormatManager formatManager;
    int midi_input_id;
    IIRFilter filter1;
    IIRFilter filter2;
    FxSelector* fx_selector;
    //Array<InstrumentMappingEditor::MappingEditorGraph::Zone*> zones;
};

class SampleVoice : public SamplerVoice
{
public:
    SampleVoice();
    void renderNextBlock(AudioSampleBuffer&, int startSample, int numSamples) override;
    void startNote(const int, const float, SynthesiserSound*, const int);
private:
    IIRFilter filter1;
    IIRFilter filter2;
    float samplePosition;
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
                    groups(group), fx_selector(fx){
        sampleRate = source.sampleRate;
        rootNote = midiNoteForNormalPitch;
    }
    Array<int> getGroups(){return groups;}
    typedef ReferenceCountedObjectPtr<SampleSound> Ptr;
    double getSampleRate(){return sampleRate;}
    int getRootNote(){return rootNote;}
    FxSelector* getFxSelector(){return fx_selector;}
    

private:
    Array<int> groups;
    double sampleRate;
    int rootNote;
    FxSelector* fx_selector;
};

                    
                    
                                    
    
    
    





#endif  // SAMPLER_H_INCLUDED
