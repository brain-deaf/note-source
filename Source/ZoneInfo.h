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
#include "InstrumentMappingEditor.h"

class ZoneInfo : public Component, public ChangeListener {
    std::shared_ptr<InstrumentMappingEditor> mappingEditor;
    SelectedItemSet<InstrumentMappingEditor::MappingEditorGraph::Zone::Ptr>* zone;
    ScopedPointer<Label> fileName;

public:
    ZoneInfo(std::shared_ptr<InstrumentMappingEditor> m);

    void changeListenerCallback(ChangeBroadcaster* source);
    void resize();
    void paint(Graphics& g);
    void mouseMove(const MouseEvent& e){}
};

#endif  // ZONEINFO_H_INCLUDED
