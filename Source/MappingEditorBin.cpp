/*
  ==============================================================================

    mappingEditorBin.cpp
    Created: 18 May 2014 10:51:58am
    Author:  patrick

  ==============================================================================
*/

#include "MappingEditorBin.h"
MappingEditorBin::MappingEditorBin(AudioDeviceManager* audio) : Component(),mapping_editor_height(435){
        mapping_editor = new InstrumentMappingEditor("", this, audio);
        zone_info = new ZoneInfo(mapping_editor);
        addAndMakeVisible(mapping_editor);
        addAndMakeVisible(zone_info);
}

MappingEditorBin::~MappingEditorBin() {
    delete mapping_editor;
    mapping_editor = nullptr; 
    delete zone_info;
    zone_info = nullptr;
}
void MappingEditorBin::resized() {
    mapping_editor->setBounds(0, getHeight() - mapping_editor_height, getWidth(), mapping_editor_height);
    zone_info->setBounds(0, 0, getWidth(), getHeight() - mapping_editor_height);
}
