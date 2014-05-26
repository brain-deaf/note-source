/*
  ==============================================================================

    instrumentComponent.cpp
    Created: 9 May 2014 6:12:25pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentComponent.h"

InstrumentComponent::InstrumentComponent(TabbedComponent* p, std::shared_ptr<AudioDeviceManager>& audio)
: parent(p), tabbed_component(new InstrumentTabWindow(TabbedButtonBar::TabsAtBottom, audio),
        instrument_master_component(new InstrumentMasterComponent(parent){
    
    instrument_master_component = new InstrumentMasterComponent(parent);
    
    addAndMakeVisible(tabbed_component);
    addAndMakeVisible(instrument_master_component);
}

InstrumentComponent::~InstrumentComponent(){
    tabbed_component = nullptr;
    instrument_master_component = nullptr;
}

void InstrumentComponent::resized(){
    tabbed_component->setBounds(0, 80, getWidth(), getHeight() - 80);
    instrument_master_component->setBounds(0, 0, getWidth(), 80);
}
