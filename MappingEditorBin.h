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
#include "ZoneInfo.h"
#include "InstrumentMappingEditor.h"

class MappingEditorBin : public Component
{
    InstrumentMappingEditor* mapping_editor;
    ZoneInfo* zone_info;
    int mapping_editor_height;
public:
    MappingEditorBin(AudioDeviceManager* audio);
    ~MappingEditorBin();
    void resized();
};

#endif  // MAPPINGEDITORBIN_H_INCLUDED
