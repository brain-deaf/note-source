/*
  ==============================================================================

    instrumentBin.cpp
    Created: 8 May 2014 6:58:20pm
    Author:  patrick

  ==============================================================================
*/

#include "instrumentBin.h"

instrumentBin::instrumentBin(TabbedButtonBar::Orientation orientation, AudioDeviceManager* audio) 
: TabbedComponent(orientation), instrument_component(new instrumentComponent(this, audio)){
    addTab("New Instrument", Colour(100, 100, 100), instrument_component, false);
    instruments.add(instrument_component);
}

instrumentBin::~instrumentBin(){
    for (int i=0; i<instruments.size(); i++){
        delete instruments[i];
    }
}

void instrumentBin::register_tab(instrumentComponent* c){
    instruments.add(c);
}