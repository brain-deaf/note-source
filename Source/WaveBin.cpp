/*
  ==============================================================================

    WaveBin.cpp
    Created: 26 Jul 2014 2:08:53pm
    Author:  patrick

  ==============================================================================
*/

#include "WaveBin.h"

WaveBin::WaveBin(MappingEditorBin* m): mapping_editor(m), wave_editor(new Component()){
    mapping_editor->getMappingEditor()->group_editor;
    group_view = new GroupView(mapping_editor->getMappingEditor()->group_editor);
    Vport = new Viewport();
    Vport->setViewedComponent(group_view);
    addAndMakeVisible(wave_editor);
    addAndMakeVisible(Vport);
    
    group_view_width = 800;
    vport_width = 300;
}

WaveBin::~WaveBin(){
    delete wave_editor;
    wave_editor = nullptr;
    delete group_view;
    group_view = nullptr;
    delete Vport;
    Vport = nullptr;
}

void WaveBin::resized(){
    group_view->setBounds(0, 0, group_view_width, getHeight() - 20);
    Vport->setBounds(0, 0, vport_width, getHeight());
    wave_editor->setBounds(group_view_width, 0, getWidth() - group_view_width, getHeight());
}