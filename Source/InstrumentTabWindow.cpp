/*
  ==============================================================================

    instrumentTabWindow.cpp
    Created: 9 May 2014 6:26:59pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentTabWindow.h"

InstrumentTabWindow::InstrumentTabWindow(InstrumentComponent& i,
        TabbedButtonBar::Orientation orientation) 
    : TabbedComponent{orientation},main_page{new MainPage(i)},
    mapping_editor{new MappingEditorBin(i)}{
    addTab("Main", Colour(100, 120, 100), main_page, false);
    addTab("FX", Colour(100, 100, 100), nullptr, false);
    addTab("Mapping", Colour(100, 100, 100), mapping_editor , false);
    addTab("Wave", Colour(100, 100, 100), nullptr, false);
    addTab("Mod", Colour(100, 100, 100), nullptr, false);
}

