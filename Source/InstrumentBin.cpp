/*
  ==============================================================================

    InstrumentBin.cpp
    Created: 8 May 2014 6:58:20pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentBin.h"
#include "MainComponent.h"

InstrumentBin::InstrumentBin(TabbedButtonBar::Orientation orientation, MainContentComponent* m)
: TabbedComponent{orientation}, parent(m), instrumentComponent(new InstrumentComponent(this)){
    addTab("New Instrument", Colour(100, 100, 100), instrumentComponent, false);
    instruments.add(instrumentComponent);
}

InstrumentBin::~InstrumentBin(){
    for (auto i : instruments){
        delete i;
    }
}

void InstrumentBin::registerTab(InstrumentComponent* c){
    instruments.add(c);
}
