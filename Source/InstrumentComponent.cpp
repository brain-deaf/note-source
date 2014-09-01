/*
  ==============================================================================

    instrumentComponent.cpp
    Created: 9 May 2014 6:12:25pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentComponent.h"

InstrumentComponent::InstrumentComponent(InstrumentBin* p)
: Component(), deviceManager(), mixer(), button(new TextButton("asdfasdf")),
    sourcePlayer(), parent(p), 
    tabs(*this,TabbedButtonBar::TabsAtBottom),
    master(p, this){
    addAndMakeVisible(tabs);
    addAndMakeVisible(master);
    addChildComponent(button);
}

void InstrumentComponent::resized(){
    tabs.setBounds(0, 80, getWidth(), getHeight() - 80);
    master.setBounds(0, 0, getWidth(), 80);
}
void InstrumentComponent::addFilePlayer(FilePlayer* p) {
    sources.add(p);
    sampleNames.add(p->getSampleName());
}
