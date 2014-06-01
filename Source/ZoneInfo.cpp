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
    
    fileName = new Label("");
    fileName->setBounds(80, 10, 600, 20);
    addAndMakeVisible(fileName);
    
    fileNameLabel = new Label("Sample: ");
    fileNameLabel->setBounds(5, 10, 70, 20);
    addAndMakeVisible(fileNameLabel);
    
    noteNumber = new Label("");
    noteNumber->setBounds(80, 30, 40, 20);
    noteNumber->setEditable(false, true, true);
    noteNumber->addListener(this);
    addAndMakeVisible(noteNumber);
    
    noteNumberLabel = new Label("Note: ");
    noteNumberLabel->setBounds(5, 30, 70, 20);
    addAndMakeVisible(noteNumberLabel);
}

void ZoneInfo::changeListenerCallback(ChangeBroadcaster* source){
    fileName->setText((zone->getSelectedItem(0))->getName(), dontSendNotification);
    fileNameLabel->setText("Sample: ", dontSendNotification);
    noteNumber->setText(String((zone->getSelectedItem(0))->getNote()), dontSendNotification);
    noteNumberLabel->setText("Note: ", dontSendNotification);
}

void ZoneInfo::resize(){
    fileName->setBounds(80, 10, 600, 20);
    fileNameLabel->setBounds(5, 10, 70, 20);
    noteNumber->setBounds(80, 30, 40, 20);
    noteNumberLabel->setBounds(5, 30, 70, 20);
}

void ZoneInfo::paint(Graphics& g){
    g.fillAll (Colours::white);
}

void ZoneInfo::labelTextChanged(Label* source){
    std::cout<<"label changed"<<std::endl;
}
    
