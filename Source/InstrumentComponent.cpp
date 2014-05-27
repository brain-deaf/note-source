/*
  ==============================================================================

    instrumentComponent.cpp
    Created: 9 May 2014 6:12:25pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentComponent.h"

InstrumentComponent::InstrumentComponent(TabbedComponent* p)
: Component{}, device_manager{}, mixer{}, 
    source_player{}, tabs{new InstrumentTabWindow(*this,TabbedButtonBar::TabsAtBottom)},
        master{new InstrumentMasterComponent(parent)},parent{p}{
   
    addAndMakeVisible(tabs);
    addAndMakeVisible(master);
}

void InstrumentComponent::resized(){
    tabs->setBounds(0, 80, getWidth(), getHeight() - 80);
    master->setBounds(0, 0, getWidth(), 80);
}
void InstrumentComponent::addFilePlayer(std::shared_ptr<FilePlayer> p) {
    sources.add(p);
    sample_names.add(p->sampleName());
}
