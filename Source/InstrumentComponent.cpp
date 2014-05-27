/*
  ==============================================================================

    instrumentComponent.cpp
    Created: 9 May 2014 6:12:25pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentComponent.h"

InstrumentComponent::InstrumentComponent(TabbedComponent* p)
: Component{}, parent{p}, tabbed_component{new InstrumentTabWindow(TabbedButtonBar::TabsAtBottom)},
        instrument_master_component{new InstrumentMasterComponent(parent)}{
    
    instrument_master_component = new InstrumentMasterComponent(parent);
    
    addAndMakeVisible(tabbed_component);
    addAndMakeVisible(instrument_master_component);
}

void InstrumentComponent::resized(){
    tabbed_component->setBounds(0, 80, getWidth(), getHeight() - 80);
    instrument_master_component->setBounds(0, 0, getWidth(), 80);
}
