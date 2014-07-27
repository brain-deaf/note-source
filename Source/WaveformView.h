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
#include "InstrumentMappingEditor.h"

typedef InstrumentMappingEditor::MappingEditorGraph::Zone Zone;

class WaveformView : public Component, public ChangeListener
{
public:
    WaveformView();
    ~WaveformView(){}
    void updateWaveformForFilePlayer(Zone*z);
    void paint(Graphics& g);
    void changeListenerCallback(ChangeBroadcaster* source);
private:
    AudioThumbnailCache cache;
    AudioThumbnail thumbnail;
    AudioFormatManager formatManager;
};



#endif  // WAVEFORMVIEW_H_INCLUDED
