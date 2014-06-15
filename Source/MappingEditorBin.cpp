/*
  ==============================================================================

    mappingEditorBin.cpp
    Created: 18 May 2014 10:51:58am
    Author:  patrick

  ==============================================================================
*/
#include <memory>
#include "MappingEditorBin.h"
MappingEditorBin::MappingEditorBin(InstrumentComponent& instrument) : Component{},
    mapping_editor{std::make_shared<InstrumentMappingEditor>("",instrument)}, 
    zone_info{mapping_editor}, mapping_editor_height{435} {
    addAndMakeVisible(*mapping_editor);
    mapping_editor->setBounds(0, getHeight() - mapping_editor_height, getWidth(), mapping_editor_height);
    addAndMakeVisible(zone_info);
}

void MappingEditorBin::resized() {
    mapping_editor->setBounds(0, getHeight() - mapping_editor_height, getWidth(), mapping_editor_height);
    zone_info.setBounds(0, 0, getWidth(), getHeight() - mapping_editor_height);
}
