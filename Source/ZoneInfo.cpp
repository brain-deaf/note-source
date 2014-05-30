/*
  ==============================================================================

    ZoneInfo.cpp
    Created: 24 May 2014 8:08:14pm
    Author:  allie

  ==============================================================================
*/

#include "ZoneInfo.h"

ZoneInfo::ZoneInfo(std::shared_ptr<InstrumentMappingEditor> m) : Component{},
    mappingEditor{m}, zone{&m->graph->getZoneInfoSet()} {
    zone->addChangeListener(this);
    fileName = new Label("lolol");
    fileName->setBounds(5, 10, 600, 20);
    addAndMakeVisible(fileName);
}

void ZoneInfo::changeListenerCallback(ChangeBroadcaster* source){
    fileName->setText((zone->getSelectedItem(0))->getName(), dontSendNotification);
}

void ZoneInfo::resize(){
    fileName->setBounds(5, 10, 600, 20);
}
void ZoneInfo::paint(Graphics& g){
    g.fillAll (Colours::white);
}
