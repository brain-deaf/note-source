/*
  ==============================================================================

    WaveformView.cpp
    Created: 20 Jun 2014 9:47:10pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveformView.h"
#include "WaveBin.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "RoundRobin.h"

static double plotAdsr(int x1, int time, int y1, int max_volume, double curve_width, int x){
	return(max_volume - y1) / (pow(M_E, curve_width*time) - pow(M_E, curve_width*x1)) * (pow(M_E, curve_width*x) - pow(M_E, curve_width*x1)) + y1;
}

WaveformView::WaveformView()
: Component(), formatManager(), parent(nullptr), cache(5), 
  vScale(0.7), sample_rate(44100.0), playPosition(0.0), playing(false),
  length_sec(0.1), thumbnail(256, getFormatManager(), getCache()), loopStart(0.0),
  loopEnd(0.0), zone(nullptr), xfadeCurve(0.0)
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

void WaveformView::updateWaveformForFilePlayer(Zone* z, int rr=-1){

    zone = z;
	if (zone == nullptr)
		return;

	ScopedPointer<AudioFormatReader> afr;
	if (rr == -1){
		afr = formatManager.createReaderFor(File(z->getName()));
	}else{
		afr = formatManager.createReaderFor(File(z->getRRGroup()->getRoundRobin(rr)->getFileName()));
	}

	if (afr == nullptr){
		thumbnail.setSource(0);
		repaint();
		return;
	}else{
		sample_rate = afr->sampleRate;
		if (rr == -1){
			thumbnail.setSource(new FileInputSource(File(z->getName())));
		}else{
			thumbnail.setSource(new FileInputSource(File(z->getRRGroup()->getRoundRobin(rr)->getFileName())));
		}
		sample_start = z->getPlaySettings()->getSampleStart();
		releaseStart = z->getPlaySettings()->getReleaseStart();
		if (parent != nullptr){
			parent->setFileLength(sample_rate*thumbnail.getTotalLength());
			parent->updateZone(z);
		}
		afr = nullptr;
		repaint();
	}
}

void WaveformView::paint(Graphics& g){
    g.fillAll(Colours::white);
    g.setColour(Colours::blue);
    g.setOpacity(1.0);
    if (zone != nullptr){
        thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), vScale);
    
        if (sample_rate > 1 && parent != nullptr){
			g.setColour(Colours::green);
			Path myPath;
            double length_per_sample = getWidth() / (thumbnail.getTotalLength()*sample_rate);
            myPath.startNewSubPath(length_per_sample*sample_start, 0);
            myPath.lineTo(length_per_sample*sample_start, getHeight());
            g.strokePath (myPath, PathStrokeType (3.0f));

			Path releasePath;
			g.setColour(Colours::red);
			releasePath.startNewSubPath(length_per_sample*releaseStart, 0);
			releasePath.lineTo(length_per_sample*releaseStart, getHeight());
			
			for (int i = length_per_sample*releaseStart; i < length_per_sample*(releaseStart + releaseTime); i++){
				double y = plotAdsr(length_per_sample*releaseStart, length_per_sample*(releaseStart + releaseTime), 0, getHeight(), releaseCurve, i);
				if (!std::isnan(y))
					releasePath.lineTo(i, getHeight() - y);
			}
			g.strokePath(releasePath, PathStrokeType(1.0f));
            
            if (loopEnd > loopStart){
                g.setColour(Colours::red);
                Path xfade_path;
                xfade_path.startNewSubPath(length_per_sample*loopStart, getHeight());
				for (int i = length_per_sample*loopStart; i < length_per_sample*(loopStart + xfadeLength); i++){
					double y = plotAdsr(length_per_sample*loopStart, length_per_sample*(loopStart + xfadeLength), 0, getHeight(), xfadeCurve, i);
					if (!std::isnan(y))
						xfade_path.lineTo(i, getHeight() - y);
				}
                xfade_path.startNewSubPath(length_per_sample*(loopEnd-xfadeLength), 0);
				for (int i = length_per_sample*(loopEnd-xfadeLength); i < length_per_sample*(loopEnd); i++){
					double y = plotAdsr(length_per_sample*(loopEnd-xfadeLength), length_per_sample*(loopEnd), getHeight(), 0, xfadeCurve*-1, i);
					if (!std::isnan(y))
						xfade_path.lineTo(i, getHeight() - y);
				}
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