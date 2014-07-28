/*
  ==============================================================================

    WaveformView.cpp
    Created: 20 Jun 2014 9:47:10pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveformView.h"
#include "WaveBin.h"

WaveformView::WaveformView()
: Component(), formatManager(), parent(nullptr), cache(5), vScale(0.7), thumbnail(256, formatManager, cache)
{
    length_sec = 0.0;
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
    
    SharedResourcePointer<AudioDeviceManager> dm;
    AudioDeviceManager::AudioDeviceSetup a;
    dm->getAudioDeviceSetup(a);
    sample_rate = a.sampleRate;
}

WaveformView::WaveformView(WaveBin* w)
: Component(), formatManager(), parent(w), cache(5), vScale(0.7), thumbnail(256, formatManager, cache)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

void WaveformView::updateWaveformForFilePlayer(Zone* z){
    SharedResourcePointer<AudioDeviceManager> dm;
    AudioDeviceManager::AudioDeviceSetup a;
    dm->getAudioDeviceSetup(a);
    sample_rate = a.sampleRate;
    
    thumbnail.setSource(new FileInputSource(File(z->getName())));
    length_sec = thumbnail.getTotalLength();
    if (parent != nullptr){ 
        parent->setFileLength(sample_rate*length_sec);
    }
    
    repaint();
}

void WaveformView::paint(Graphics& g){
    g.fillAll(Colours::white);
    g.setColour(Colours::blue);
    thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), vScale);
    
    g.setColour(Colours::green);
    Path myPath;
    if (sample_rate > 1){
        double length_per_sample = getWidth() / (length_sec*sample_rate);
        myPath.startNewSubPath(length_per_sample*sample_start, 0);
        myPath.lineTo(length_per_sample*sample_start, getHeight());
    }
    
    g.strokePath (myPath, PathStrokeType (3.0f));
}

void WaveformView::changeListenerCallback(ChangeBroadcaster* source){
    if (source == &thumbnail){
        repaint();
    }
}