/*
  ==============================================================================

    InstrumentBin.cpp
    Created: 8 May 2014 6:58:20pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentBin.h"

InstrumentBin::InstrumentBin(TabbedButtonBar::Orientation orientation, std::shared_ptr<AudioDeviceManager> audio)
: TabbedComponent{orientation}, instrument_component{new InstrumentComponent(this, audio)}{
    addTab("New Instrument", Colour(100, 100, 100), instrument_component, false);
    instruments.add(instrument_component);
}

InstrumentBin::~InstrumentBin(){
    for (auto i : instruments){
        delete i;
    }
}

void InstrumentBin::register_tab(InstrumentComponent* c){
    instruments.add(c);
}
