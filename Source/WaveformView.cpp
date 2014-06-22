/*
  ==============================================================================

    WaveformView.cpp
    Created: 20 Jun 2014 9:47:10pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveformView.h"
#include "FilePlayer.h"

WaveformView::WaveformView(int w, int h)
: Component(), width(w), height(h), 
  cache(new AudioThumbnailCache(5)),
  thumbnail(nullptr){}

void WaveformView::updateWaveformForFilePlayer(FilePlayer* f){
    if (thumbnail == nullptr){
        thumbnail = new AudioThumbnail(256, f->getAudioFormatManager(), *cache);
    }
    thumbnail->setSource(new FileInputSource(File(f->getSampleName())));
    thumbnail->addChangeListener(this);
    repaint();
}

void WaveformView::paint(Graphics& g){
    g.fillAll(Colours::yellow);
    g.setColour(Colours::blue);
    if (thumbnail != nullptr){
        thumbnail->drawChannels(g, getLocalBounds(), 0.0, thumbnail->getTotalLength(), 0.7f);
    }
}

void WaveformView::changeListenerCallback(ChangeBroadcaster* source){
    if (source == thumbnail){
        repaint();
    }
}