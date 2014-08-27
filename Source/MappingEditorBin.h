/*
  ==============================================================================

    mappingEditorBin.h
    Created: 18 May 2014 10:51:36am
    Author:  patrick

  ==============================================================================
*/
#ifndef MAPPINGEDITORBIN_H_INCLUDED
#define MAPPINGEDITORBIN_H_INCLUDED
#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "ZoneInfo.h"
#include "InstrumentMappingEditor.h"

class MappingEditorBin : public Component
{
public:
    MappingEditorBin(InstrumentComponent&);
    std::shared_ptr<InstrumentMappingEditor> getMappingEditor(){return mapping_editor;}
    void resized();
    float groupEditorHeight;
    float fileBrowserHeight;
    InstrumentComponent* getInstrument(){return instrument;}
private:
    std::shared_ptr<InstrumentMappingEditor> mapping_editor;
    ZoneInfo zone_info;
    int mapping_editor_height;
    InstrumentComponent* instrument;
};

#endif  // MAPPINGEDITORBIN_H_INCLUDED
