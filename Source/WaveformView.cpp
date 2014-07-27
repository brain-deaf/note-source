/*
  ==============================================================================

    WaveformView.cpp
    Created: 20 Jun 2014 9:47:10pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveformView.h"




WaveformView::WaveformView()
: Component(), formatManager(), cache(5), thumbnail(256, formatManager, cache)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

void WaveformView::updateWaveformForFilePlayer(Zone* z){
    thumbnail.setSource(new FileInputSource(File(z->getName())));
    repaint();
}

void WaveformView::paint(Graphics& g){
    g.fillAll(Colours::white);
    g.setColour(Colours::blue);
    thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 0.7f);
}

void WaveformView::changeListenerCallback(ChangeBroadcaster* source){
    if (source == &thumbnail){
        repaint();
    }
}