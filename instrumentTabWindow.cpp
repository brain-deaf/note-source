/*
  ==============================================================================

    instrumentTabWindow.cpp
    Created: 9 May 2014 6:26:59pm
    Author:  patrick

  ==============================================================================
*/

#include "instrumentTabWindow.h"

instrumentTabWindow::instrumentTabWindow(TabbedButtonBar::Orientation orientation, AudioDeviceManager* audio) : TabbedComponent(orientation){
    addTab("Main", Colour(100, 100, 100), nullptr, false);
    addTab("FX", Colour(100, 100, 100), nullptr, false);
    //addTab("Mapping", Colour(100, 100, 100), mapping_editor = new instrumentMappingEditor("", this, audio), false);
    addTab("Mapping", Colour(100, 100, 100), mapping_editor = new mappingEditorBin(audio), false);
    addTab("Wave", Colour(100, 100, 100), nullptr, false);
    addTab("Mod", Colour(100, 100, 100), nullptr, false);
}

instrumentTabWindow::~instrumentTabWindow(){
    mapping_editor = nullptr;
    zoom_button = nullptr;
}