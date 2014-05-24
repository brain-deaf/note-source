/*
  ==============================================================================

    zoneInfo.h
    Created: 18 May 2014 11:13:44am
    Author:  patrick

  ==============================================================================
*/

#ifndef ZONEINFO_H_INCLUDED
#define ZONEINFO_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "instrumentMappingEditor.h"

class zoneInfo : public Component, public ChangeListener
{
public:
    instrumentMappingEditor* mapping_editor;
    SelectedItemSet<instrumentMappingEditor::mappingEditorGraph::Zone*>* zone;
    
    Label* file_name;
    
    zoneInfo(instrumentMappingEditor* m) : Component(), mapping_editor(m), zone(m->graph->zone_info_set){
        zone->addChangeListener(this);
        file_name = new Label("lolol");
        file_name->setBounds(5, 10, 600, 20);
        addAndMakeVisible(file_name);
    };
    
    ~zoneInfo(){file_name = nullptr;};
    
    void changeListenerCallback(ChangeBroadcaster* source){
        file_name->setText((zone->getSelectedItem(0))->name, dontSendNotification);
        std::cout<<file_name->getText()<<std::endl;
    };
    
    void resize(){
        file_name->setBounds(5, 10, 600, 20);
        std::cout<<"file name resize"<<std::endl;
    };
    
    void paint(Graphics& g){
        g.fillAll (Colours::white);
    };
    
    void mouseMove(const MouseEvent& e){
    };
};



#endif  // ZONEINFO_H_INCLUDED
