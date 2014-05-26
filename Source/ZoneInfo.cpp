/*
  ==============================================================================

    ZoneInfo.cpp
    Created: 24 May 2014 8:08:14pm
    Author:  allie

  ==============================================================================
*/

#include "ZoneInfo.h"

ZoneInfo::ZoneInfo(std::shared_ptr<InstrumentMappingEditor> m) : Component(), mapping_editor(m), zone(m->graph->zone_info_set()){
    zone->addChangeListener(this);
    file_name = new Label("lolol");
    file_name->setBounds(5, 10, 600, 20);
    addAndMakeVisible(file_name);
}

ZoneInfo::~ZoneInfo(){
    delete file_name;
    file_name = nullptr;
}

void ZoneInfo::changeListenerCallback(ChangeBroadcaster* source){
    file_name->setText((zone->getSelectedItem(0))->name(), dontSendNotification);
    std::cout<<file_name->getText()<<std::endl;
}

void ZoneInfo::resize(){
    file_name->setBounds(5, 10, 600, 20);
    std::cout<<"file name resize"<<std::endl;
}
void ZoneInfo::paint(Graphics& g){
    g.fillAll (Colours::white);
}
