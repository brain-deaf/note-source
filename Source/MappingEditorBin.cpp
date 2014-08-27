/*
  ==============================================================================

    mappingEditorBin.cpp
    Created: 18 May 2014 10:51:58am
    Author:  patrick

  ==============================================================================
*/
#include <memory>
#include "MappingEditorBin.h"
MappingEditorBin::MappingEditorBin(InstrumentComponent& i) : Component{},
    mapping_editor(std::make_shared<InstrumentMappingEditor>("",i)), 
    zone_info{mapping_editor}, mapping_editor_height{435},
    instrument(&i)
{
    addAndMakeVisible(*mapping_editor);
    mapping_editor->setBounds(0, getHeight() - mapping_editor_height, getWidth(), mapping_editor_height);
    addAndMakeVisible(zone_info);
    zone_info.resized();
}

void MappingEditorBin::resized() {
    mapping_editor->setBounds(0, getHeight() - mapping_editor_height, getWidth(), mapping_editor_height);
    zone_info.setBounds(0, 0, getWidth(), getHeight() - mapping_editor_height);
}
