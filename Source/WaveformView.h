/*
  ==============================================================================

    WaveformView.h
    Created: 20 Jun 2014 9:47:34pm
    Author:  patrick

  ==============================================================================
*/

#ifndef WAVEFORMVIEW_H_INCLUDED
#define WAVEFORMVIEW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class FilePlayer;

class WaveformView : public Component, public ChangeListener
{
public:
    WaveformView(int w, int h);
    ~WaveformView(){
        delete thumbnail; 
        thumbnail = nullptr;
        delete cache;
        cache = nullptr;
    }
    void updateWaveformForFilePlayer(FilePlayer*);
    void paint(Graphics& g);
    void changeListenerCallback(ChangeBroadcaster* source);
    int get_width(){return width;}
    int get_height(){return height;}
private:
    AudioThumbnail* thumbnail;
    AudioThumbnailCache* cache;
    int width;
    int height;
};



#endif  // WAVEFORMVIEW_H_INCLUDED
