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
#include "FilePlayer.h"

class WaveBin : public Component, public Slider::Listener, public Button::Listener, public ChangeListener, public Timer {
public:
    WaveBin(MappingEditorBin*);
    ~WaveBin();
    void resized();
    void paint(Graphics&) override;
    void mouseMove(const MouseEvent&) override;
    void mouseDown(const MouseEvent&) override;
    void mouseUp(const MouseEvent&) override;
    void mouseDrag(const MouseEvent&) override;
    void sliderValueChanged(Slider*) override;
    void setFileLength(double d){
        fileLength=d;
        sample_start->setRange(0, d, 1);
        loopEnd->setRange(0, d, 1);
        loopStart->setRange(0, d, 1);
    }
    void updateZone(Zone*_zone);
    void buttonClicked(Button*);
    void changeListenerCallback(ChangeBroadcaster*);
    void timerCallback();
    WaveformView* getWaveformView(){return waveform;}
private:
    ScopedPointer<GroupView> group_view;
    MappingEditorBin* mapping_editor;
    int group_view_width;
    int vport_width;
    int drag_threshold;
    int lower_limit;
    int waveform_width;
    int waveform_height;
    int top_padding;
    int waveform_padding;
    int scaling_slider_height;
    double fileLength;
    bool dragging;
    ScopedPointer<Viewport> Vport;
    ScopedPointer<Viewport> WaveVport;
    ScopedPointer<WaveformView> waveform;
    ScopedPointer<Slider> sample_start;
    ScopedPointer<Slider> vScaling;
    ScopedPointer<Slider> hScaling;
    ScopedPointer<TextButton> playButton;
    ScopedPointer<FilePlayer> filePlayer;
    ScopedPointer<Slider> loopStart;
    ScopedPointer<Slider> loopEnd;
    ScopedPointer<Slider> xfadeLength;
    ScopedPointer<Slider> tuneSlider;
    ScopedPointer<TextButton> toggleLoop;
    Zone* z;
};



#endif  // WAVEBIN_H_INCLUDED
