/*
  ==============================================================================

    ZoneInfo.cpp
    Created: 24 May 2014 8:08:14pm
    Author:  allie

  ==============================================================================
*/

#include "ZoneInfo.h"
#include "WaveformView.h"



ZoneInfo::ZoneInfo(std::shared_ptr<InstrumentMappingEditor> m) : Component{},
    mappingEditor{m}, zone{&m->graph->getZoneInfoSet()}, /*adsr(),*/
    audio_thumbnail(new WaveformView()){
    zone->addChangeListener(this);
    
    fileName = new Label("");
    fileName->setBounds(80, 10, 600, 20);
    addAndMakeVisible(fileName);
    
    fileNameLabel = new Label("Sample: ");
    fileNameLabel->setBounds(5, 10, 70, 20);
    addAndMakeVisible(fileNameLabel);
    
    noteNumber = new Label("");
    noteNumber->setBounds(80, 30, 30, 20);
    noteNumber->setEditable(false, true, true);
    noteNumber->addListener(this);
    addAndMakeVisible(noteNumber);
    
    noteNumberLabel = new Label("Note: ");
    noteNumberLabel->setBounds(5, 30, 70, 20);
    addAndMakeVisible(noteNumberLabel);
    
    noteName = new Label("");
    noteName->setBounds(110, 30, 40, 20);
    addAndMakeVisible(noteName);
    
    velocityLabel = new Label("Velocity: ");
    velocityLabel->setBounds(5, 50, 70, 20);
    addAndMakeVisible(velocityLabel);
    
    velocityMin = new Label("");
    velocityMin->setBounds(80, 50, 30, 20);
    velocityMin->setEditable(false, true, true);
    velocityMin->addListener(this);
    addAndMakeVisible(velocityMin);
    
    velocityMax= new Label("");
    velocityMax->setBounds(110, 50, 30, 20);
    velocityMax->setEditable(false, true, true);
    velocityMax->addListener(this);
    addAndMakeVisible(velocityMax);
    
    //audio_thumbnail->setBounds(500, 25, 400, 100);
    addAndMakeVisible(audio_thumbnail);
    
    vLayout = new TextButton("Make Column");
    vLayout->addListener(this);
    addAndMakeVisible(vLayout);
    hLayout = new TextButton("Make Block");
    hLayout->addListener(this);
    addAndMakeVisible(hLayout);
    vLayout->setBounds(5, 168, 60, 30);
    hLayout->setBounds(70, 168, 60, 30);
    
    Array<String> noteLetters;
    noteLetters.add("C");
    noteLetters.add("Db");
    noteLetters.add("D");
    noteLetters.add("Eb");
    noteLetters.add("E");
    noteLetters.add("F");
    noteLetters.add("Gb");
    noteLetters.add("G");
    noteLetters.add("Ab");
    noteLetters.add("A");
    noteLetters.add("Bb");
    noteLetters.add("B");
    
    for (int i=0; i<128; i++){
        noteNames.add(noteLetters[i % 12] + String((int)(i / 12) - 2));
    }
    
    /*addAndMakeVisible(&adsr);
    adsr.setBounds(100, 5, 400, 150);*/
}

void ZoneInfo::changeListenerCallback(ChangeBroadcaster* /*source*/){
    if (zone->getSelectedItem(0) != nullptr){
        audio_thumbnail->setBounds(getWidth() - 410, 50, 400, 100);
        audio_thumbnail->updateWaveformForFilePlayer(zone->getSelectedItem(0));
        fileName->setText((zone->getSelectedItem(0))->getName(), dontSendNotification);
        fileNameLabel->setText("Sample: ", dontSendNotification);
        noteNumber->setText(String((zone->getSelectedItem(0))->getNote()), dontSendNotification);
        noteNumberLabel->setText("Note: ", dontSendNotification);
        noteName->setText(noteNames[(zone->getSelectedItem(0))->getNote()], dontSendNotification);
        velocityLabel->setText("Velocity: ", dontSendNotification);
        velocityMin->setText(String(zone->getSelectedItem(0)->getVelocity().first), dontSendNotification);
        velocityMax->setText(String(zone->getSelectedItem(0)->getVelocity().second), dontSendNotification);
        
    }
}

void ZoneInfo::resize(){
    fileName->setBounds(80, 10, 600, 20);
    fileNameLabel->setBounds(5, 10, 70, 20);
    noteNumber->setBounds(80, 30, 30, 20);
    noteNumberLabel->setBounds(5, 30, 70, 20);
    noteName->setBounds(110, 30, 40, 20);
    audio_thumbnail->setBounds(getWidth() - 410, 50, 400, 100);
    velocityLabel->setBounds(5, 50, 70, 20);
    vLayout->setBounds(5, 168, 60, 30);
    hLayout->setBounds(70, 168, 60, 30);
}

void ZoneInfo::paint(Graphics& g){
    g.fillAll (Colours::white);
}

void ZoneInfo::buttonClicked(Button* source){
    if (source == vLayout){
        int total_selected = zone->getNumSelected();
        int velocity_per_zone = 128 / total_selected;
        int note = zone->getSelectedItem(0)->getNote();
        float y_per_velocity = mappingEditor->graph->getHeight() / 128;
        
        Array<InstrumentMappingEditor::MappingEditorGraph::Zone*> zones;
        for (int i=0; i<total_selected; i++){
            zones.add(zone->getSelectedItem(i));
        }
        ZoneSorterByNote z_note_sorter;
        zones.sort(z_note_sorter);
        
        for (int i=0; i<zones.size(); i++){
            auto z = zones[i];
            int grid_outline = 1;
            int start_velocity = velocity_per_zone*i;
            int end_velocity = velocity_per_zone*(i+1) - 1;
            
            z->setNote(note);
            z->getVelocity().first = start_velocity;
            z->getVelocity().second = end_velocity;
            z->setX(note * mappingEditor->graph->getWidth() / mappingEditor->graph->getNumColumns() + grid_outline);
            z->setY((128 - end_velocity)*y_per_velocity);
            z->setHeight((end_velocity - start_velocity) * y_per_velocity);
            z->setBounds(z->getX(), z->getY(), 
                         mappingEditor->graph->getWidth() / 128 - grid_outline, 
                         z->getHeight());
            mappingEditor->graph->updateZone(z);
        }
    }
    if (source == hLayout){
        int total_selected = zone->getNumSelected();
        
        Array<InstrumentMappingEditor::MappingEditorGraph::Zone*> zones;
        Array<InstrumentMappingEditor::MappingEditorGraph::Zone*> zones2;
        for (int i=0; i<total_selected; i++){
            zones.add(zone->getSelectedItem(i));
            zones2.add(zone->getSelectedItem(i));
        }
        
        ZoneSorterByVelocity z_velocity_sorter;
        zones.sort(z_velocity_sorter);
        
        ZoneSorterByNote z_note_sorter;
        zones2.sort(z_note_sorter);
        
        int start_note = zones2[0]->getNote();
        
        for (int i=0; i<zones.size(); i++){
            auto z = zones[i];
            int grid_outline = 1;
            
            z->setNote(start_note+i);
            z->getVelocity().first = 0;
            z->getVelocity().second = 127;
            z->setX((start_note+i) * mappingEditor->graph->getWidth() / mappingEditor->graph->getNumColumns() + grid_outline);
            z->setY(0);
            z->setHeight(mappingEditor->graph->getHeight());
            z->setBounds(z->getX(), z->getY(), 
                         mappingEditor->graph->getWidth() / 128 - grid_outline, 
                         z->getHeight());
            mappingEditor->graph->updateZone(z);
        }
    }
}

void ZoneInfo::labelTextChanged(Label* source){
    //auto z = zone->getSelectedItem(0);
    
    for (int i=0; i<zone->getNumSelected(); i++){
        auto z = zone->getSelectedItem(i);
    
        if (source == noteNumber){
            if (noteNumber->getText().containsOnly("1234567890")){
                int new_note = noteNumber->getText().getIntValue();
                int grid_outline = 1;
            
                if (new_note > 127){new_note = 127;}
                if (new_note < 0){new_note = 0;}
            
                z->setNote(new_note);
                z->setX(new_note * mappingEditor->graph->getWidth() / mappingEditor->graph->getNumColumns() + grid_outline);
                z->setTopLeftPosition(z->getX(), z->getY());
                mappingEditor->graph->getZoneInfoSet().changed();
                mappingEditor->graph->updateZone(z);
            }else{
                noteNumber->setText(String(z->getNote()), dontSendNotification);
            }
        }
    
        if (source == velocityMin){
            if (velocityMax->getText().containsOnly("1234567890")){
                int new_velocity_min = velocityMin->getText().getIntValue();
                int velocity_max = velocityMax->getText().getIntValue();
            
                if (new_velocity_min < 0){
                    new_velocity_min = 0;
                    velocityMin->setText("0", dontSendNotification);
                }
                else if (new_velocity_min >= velocity_max){
                    new_velocity_min = velocity_max - 1;
                    velocityMin->setText(String(new_velocity_min), dontSendNotification);
                }
                z->getVelocity().first = new_velocity_min;
            
                int y = mappingEditor->graph->getHeight() / 128 * (128 - new_velocity_min);
                int new_height = y - z->getY();
                z->setHeight(new_height);
                z->setSize(mappingEditor->graph->getWidth() / 128 - 1, z->getHeight());
                mappingEditor->graph->updateZone(z);
            }else{
                velocityMin->setText(String(z->getVelocity().first), dontSendNotification);
            }
        }
    
        if (source == velocityMax){
            if (velocityMax->getText().containsOnly("1234567890")){
                int velocity_min = velocityMin->getText().getIntValue();
                int new_velocity_max = velocityMax->getText().getIntValue();
        
                if (new_velocity_max > 127){
                    new_velocity_max = 127;
                    velocityMax->setText("127", dontSendNotification);
                }
        
                else if (new_velocity_max <= velocity_min){
                    new_velocity_max = z->getVelocity().first + 1;
                    velocityMax->setText(String(new_velocity_max), dontSendNotification);
                }
                z->getVelocity().second = new_velocity_max;
        
                float y_per_velocity = mappingEditor->graph->getHeight() / 128;
                int new_y = (128 - new_velocity_max) * y_per_velocity;
        
                z->setHeight((new_velocity_max - z->getVelocity().first) * y_per_velocity);
                z->setY(new_y);
                z->setBounds(z->getX(), new_y, mappingEditor->graph->getWidth() / 128 - 1, z->getHeight());
                mappingEditor->graph->updateZone(z);
            }else{
                velocityMax->setText(String(z->getVelocity().second), dontSendNotification);
            }
        }
    }
}
