/*
  ==============================================================================

    WaveBin.cpp
    Created: 26 Jul 2014 2:08:53pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveBin.h"

WaveBin::WaveBin(MappingEditorBin* m): mapping_editor(m), waveform(new WaveformView()){
    mapping_editor->getMappingEditor()->group_editor;
    group_view = new GroupView(mapping_editor->getMappingEditor()->group_editor, this);
    Vport = new Viewport();
    Vport->setViewedComponent(group_view);
    addAndMakeVisible(Vport);
    addAndMakeVisible(waveform);
    
    group_view_width = 800;
    vport_width = 300;
    waveform_height = 200;
}

WaveBin::~WaveBin(){
    delete group_view;
    group_view = nullptr;
    delete Vport;
    Vport = nullptr;
    delete waveform;
    waveform = nullptr;
    
}

void WaveBin::resized(){
    group_view->setBounds(0, 0, group_view_width, getHeight() - 20);
    Vport->setBounds(0, 0, vport_width, getHeight());
    waveform->setBounds(vport_width, 0, getWidth()-vport_width, waveform_height);
}