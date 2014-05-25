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
    InstrumentMappingEditor* mapping_editor;
    SelectedItemSet<InstrumentMappingEditor::MappingEditorGraph::Zone*>* zone;
    Label* file_name;

public:
    ZoneInfo(InstrumentMappingEditor* m);
    ~ZoneInfo();

    void changeListenerCallback(ChangeBroadcaster* source);
    void resize();
    void paint(Graphics& g);
    void mouseMove(const MouseEvent& e){}
};

#endif  // ZONEINFO_H_INCLUDED
