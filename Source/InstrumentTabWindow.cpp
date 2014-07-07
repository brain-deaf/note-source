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
    : TabbedComponent{orientation},mainPage{new MainPage(i)},
    mappingEditor{new MappingEditorBin(i)}, fxbin{new FxBin(mappingEditor)} {
    addTab("Main", Colour(100, 100, 100), mainPage, false);
    addTab("FX", Colour(100, 110, 100), fxbin, false);
    addTab("Mapping", Colour(120, 100, 100), mappingEditor , false);
    addTab("Wave", Colour(100, 130, 100), nullptr, false);
    addTab("Mod", Colour(100, 140, 100), nullptr, false);
}

void InstrumentTabWindow::currentTabChanged(int new_index, const String& tab_name){
    if (new_index == 2){
        mappingEditor->getMappingEditor()->refreshGroupEditor();
    }
}

