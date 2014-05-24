/*
  ==============================================================================

    mappingEditorBin.h
    Created: 18 May 2014 10:51:36am
    Author:  patrick

  ==============================================================================
*/
#ifndef MAPPINGEDITORBIN_H_INCLUDED
#define MAPPINGEDITORBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "zoneInfo.h"
#include "instrumentMappingEditor.h"

class mappingEditorBin : public Component
{
public:
    instrumentMappingEditor* mapping_editor;
    zoneInfo* zone_info;
    
    int mapping_editor_height;
    
    mappingEditorBin(AudioDeviceManager* audio) : mapping_editor_height(455), Component(){
        mapping_editor = new instrumentMappingEditor("", this, audio);
        zone_info = new zoneInfo(mapping_editor);
        addAndMakeVisible(mapping_editor);
        addAndMakeVisible(zone_info);
    };
    ~mappingEditorBin(){mapping_editor = nullptr; zone_info = nullptr;};
    
    void resized(){
        mapping_editor->setBounds(0, getHeight() - mapping_editor_height, getWidth(), mapping_editor_height);
        zone_info->setBounds(0, 0, getWidth(), getHeight() - mapping_editor_height);
    };
};

#endif  // MAPPINGEDITORBIN_H_INCLUDED
