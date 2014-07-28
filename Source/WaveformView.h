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
class WaveBin;
typedef InstrumentMappingEditor::MappingEditorGraph::Zone Zone;

class WaveformView : public Component, public ChangeListener
{
public:
    WaveformView();
    WaveformView(WaveBin*);
    ~WaveformView(){}
    void updateWaveformForFilePlayer(Zone*z);
    void paint(Graphics& g);
    void changeListenerCallback(ChangeBroadcaster* source);
    void setVScale(double d){vScale=d;}
    void setSampleStart(double d){sample_start=d;repaint();}
private:
    WaveBin* parent;
    double vScale;
    double length_sec;
    double sample_rate;
    double sample_start;
    AudioThumbnailCache cache;
    AudioThumbnail thumbnail;
    AudioFormatManager formatManager;
};



#endif  // WAVEFORMVIEW_H_INCLUDED
