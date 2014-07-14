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

class Sampler : public AudioSource
{
public:
    Sampler();
    void addSample(String path, int root_note, int note_low, int note_high, Array<int>&);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void setMidiChannel(int i){midi_input_id = i;}
    
    Synthesiser* getSynth(){return &synth;}
    MidiMessageCollector& getMidiCollector(){return midiCollector;}
private:
    MidiMessageCollector midiCollector;
    Synthesiser synth;
    AudioFormatManager formatManager;
    int midi_input_id;
    IIRFilter filter1;
    IIRFilter filter2;
    //Array<InstrumentMappingEditor::MappingEditorGraph::Zone*> zones;
};

class SampleVoice : public SamplerVoice
{
public:
    SampleVoice();
    void renderNextBlock(AudioSampleBuffer&, int startSample, int numSamples) override;
private:
    IIRFilter filter1;
    IIRFilter filter2;
    float samplePosition;
    float attackTime;
    float autoReleaseTime;
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
                Array<int> group) : 
                    SamplerSound(name, source, midiNotes, midiNoteForNormalPitch, 
                                 attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds),
                    groups(group){
        sampleRate = source.sampleRate;
    }
    Array<int> getGroups(){return groups;}
    typedef ReferenceCountedObjectPtr<SampleSound> Ptr;
    double getSampleRate(){return sampleRate;}
private:
    Array<int> groups;
    double sampleRate;
};

                    
                    
                                    
    
    
    





#endif  // SAMPLER_H_INCLUDED
