/*
  ==============================================================================

    WaveBin.h
    Created: 26 Jul 2014 2:09:03pm
    Author:  patrick

  ==============================================================================
*/

#ifndef WAVEBIN_H_INCLUDED
#define WAVEBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "GroupEditorZ.h"
#include "MappingEditorBin.h"
#include "WaveformView.h"

class WaveBin : public Component {
public:
    WaveBin(MappingEditorBin*);
    ~WaveBin();
    void resized();
    WaveformView* getWaveformView(){return waveform;}
private:
    GroupView* group_view;
    MappingEditorBin* mapping_editor;
    int group_view_width;
    int vport_width;
    int waveform_height;
    Viewport* Vport;
    WaveformView* waveform;
};



#endif  // WAVEBIN_H_INCLUDED
