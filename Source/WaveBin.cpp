/*
  ==============================================================================

    WaveBin.cpp
    Created: 26 Jul 2014 2:08:53pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveBin.h"

WaveBin::WaveBin(MappingEditorBin* m): mapping_editor(m), waveform(new WaveformView()),
                 sample_start(new TextButton("Start")), dragging(false){
    mapping_editor->getMappingEditor()->group_editor;
    group_view = new GroupView(mapping_editor->getMappingEditor()->group_editor, this);
    
    Vport = new Viewport();
    Vport->setViewedComponent(group_view);
    Vport->removeMouseListener(this);
    addAndMakeVisible(Vport);
    
    WaveVport = new Viewport();
    WaveVport->setViewedComponent(waveform);
    WaveVport->removeMouseListener(this);
    addAndMakeVisible(WaveVport);
    
    addAndMakeVisible(sample_start);
    
    group_view_width = 800;
    vport_width = 300;
    lower_limit = 100;
    drag_threshold = 5;
    waveform_height = 200;
    waveform_width = 600;
    waveform_padding = 30;
    top_padding = 30;
    
    waveform->setSize(waveform_width, waveform_height);
}

WaveBin::~WaveBin(){
    delete group_view;
    group_view = nullptr;
    delete Vport;
    Vport = nullptr;
    delete waveform;
    waveform = nullptr;
    delete sample_start;
    sample_start = nullptr;
    delete WaveVport;
    WaveVport = nullptr;
}

void WaveBin::resized(){
    group_view->setBounds(0, 0, group_view_width, getHeight() - 20);
    Vport->setBounds(0, 0, vport_width, getHeight());
    WaveVport->setBounds(vport_width+waveform_padding, top_padding, getWidth()-vport_width-waveform_padding*2, waveform_height);
}

void WaveBin::paint(Graphics& g){
    g.fillAll(Colours::grey);
}

void WaveBin::mouseMove(const MouseEvent& e){
    if (!dragging){
        if (fabs(e.getPosition().getX() - vport_width) <= drag_threshold){
            setMouseCursor(MouseCursor::LeftRightResizeCursor);
        }else{
            if (getMouseCursor() != MouseCursor()){
                setMouseCursor(MouseCursor());
            }
        }
    }
}

void WaveBin::mouseDown(const MouseEvent& e){
    if (getMouseCursor() == MouseCursor::LeftRightResizeCursor){
        dragging = true;
    }
}

void WaveBin::mouseUp(const MouseEvent& e){
    dragging = false;
}

void WaveBin::mouseDrag(const MouseEvent& e){
    if (dragging){
        vport_width = e.getPosition().getX() <= group_view_width ? e.getPosition().getX() : group_view_width;
        vport_width = e.getPosition().getX() >= lower_limit ? vport_width : lower_limit;
        resized();
    }
}