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
    void paint(Graphics&) override;
    void mouseMove(const MouseEvent&) override;
    void mouseDown(const MouseEvent&) override;
    void mouseUp(const MouseEvent&) override;
    void mouseDrag(const MouseEvent&) override;
    WaveformView* getWaveformView(){return waveform;}
private:
    GroupView* group_view;
    MappingEditorBin* mapping_editor;
    int group_view_width;
    int vport_width;
    int drag_threshold;
    int lower_limit;
    int waveform_height;
    int waveform_padding;
    bool dragging;
    Viewport* Vport;
    WaveformView* waveform;
    TextButton* sample_start;
};



#endif  // WAVEBIN_H_INCLUDED
