/*
  ==============================================================================

    instrumentTabWindow.cpp
    Created: 9 May 2014 6:26:59pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentTabWindow.h"

InstrumentTabWindow::InstrumentTabWindow(InstrumentComponent& instrument,
        TabbedButtonBar::Orientation orientation) 
    : TabbedComponent{orientation},mapping_editor{new MappingEditorBin(instrument)}{
    addTab("Main", Colour(100, 100, 100), nullptr, false);
    addTab("FX", Colour(100, 100, 100), nullptr, false);
    addTab("Mapping", Colour(100, 100, 100), mapping_editor , false);
    addTab("Wave", Colour(100, 100, 100), nullptr, false);
    addTab("Mod", Colour(100, 100, 100), nullptr, false);
}

InstrumentTabWindow::~InstrumentTabWindow(){
    mapping_editor = nullptr;
    zoom_button = nullptr;
}
