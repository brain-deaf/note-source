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
: Component(), formatManager(), parent(nullptr), cache(5), 
  vScale(0.7), sample_rate(44100.0), playPosition(0.0), playing(false),
  length_sec(0.1), thumbnail(256, formatManager, cache), loopStart(0.0),
  loopEnd(0.0), zone(nullptr)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

WaveformView::WaveformView(WaveBin* w)
: Component(), formatManager(), parent(w), cache(5), vScale(0.7), thumbnail(256, formatManager, cache)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

void WaveformView::updateWaveformForFilePlayer(Zone* z){
    zone = z;
    
    AudioFormatReader* afr = formatManager.createReaderFor(File(z->getName()));
    sample_rate = afr->sampleRate;
    
    thumbnail.setSource(new FileInputSource(File(z->getName())));
    sample_start = z->getPlaySettings()->getSampleStart();
    if (parent != nullptr){ 
        parent->setFileLength(sample_rate*thumbnail.getTotalLength());
    }
    
    repaint();
}

void WaveformView::paint(Graphics& g){
    g.fillAll(Colours::white);
    g.setColour(Colours::blue);
    g.setOpacity(1.0);
    if (zone != nullptr){
        thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), vScale);
    
        g.setColour(Colours::green);
        Path myPath;
        if (sample_rate > 1 && parent != nullptr){
            double length_per_sample = getWidth() / (thumbnail.getTotalLength()*sample_rate);
            myPath.startNewSubPath(length_per_sample*sample_start, 0);
            myPath.lineTo(length_per_sample*sample_start, getHeight());
            g.strokePath (myPath, PathStrokeType (3.0f));
            
            if (loopEnd > loopStart){
                g.setColour(Colours::red);
                Path xfade_path;
                xfade_path.startNewSubPath(length_per_sample*loopStart, getHeight());
                xfade_path.lineTo(length_per_sample*(loopStart+xfadeLength), 0);
                xfade_path.startNewSubPath(length_per_sample*loopEnd, getHeight());
                xfade_path.lineTo(length_per_sample*(loopEnd-xfadeLength), 0);
                g.strokePath(xfade_path, PathStrokeType(1.0f));
                
                g.setColour(Colours::yellow);
                g.setOpacity(0.5);
                const Rectangle<float> loop_rectangle(length_per_sample*loopStart, 0, length_per_sample*loopEnd-length_per_sample*loopStart, getHeight());
                g.fillRect(loop_rectangle);
            }
        }
        if (playing){
            g.setOpacity(1.0);
            Path myPath;
            g.setColour(Colours::red);
            double length_per_sample = getWidth() / (length_sec*sample_rate);
            double position = playPosition*sample_rate;
            myPath.startNewSubPath(length_per_sample*position, 0);
            myPath.lineTo(length_per_sample*position, getHeight());
            g.strokePath(myPath, PathStrokeType(2.0f));
        }
    }
}

void WaveformView::changeListenerCallback(ChangeBroadcaster* source){
    if (source == &thumbnail){
        repaint();
    }
}