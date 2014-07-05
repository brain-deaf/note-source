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
    void addSample(String path, int root_note, int note_low, int note_high);
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
    IIRFilter filter;
    //Array<InstrumentMappingEditor::MappingEditorGraph::Zone*> zones;
};

class SampleVoice : public SamplerVoice
{
public:
    SampleVoice();
    void renderNextBlock(AudioSampleBuffer&, int startSample, int numSamples) override;
private:
    IIRFilter filter;
};

class AudioPlayer : public AudioSourcePlayer
{
public:
    AudioPlayer();
    void audioDeviceIOCallback(const float** inputChannelData, int totalNumInputChannels, 
                          float** outputChannelData, int totalNumOutputChannels, int numSamples);
private:
    IIRFilter filter;
};
    
    
    





#endif  // SAMPLER_H_INCLUDED
