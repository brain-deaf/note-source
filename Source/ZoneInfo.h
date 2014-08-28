/*
  ==============================================================================

    zoneInfo.h
    Created: 18 May 2014 11:13:44am
    Author:  patrick

  ==============================================================================
*/

#ifndef ZONEINFO_H_INCLUDED
#define ZONEINFO_H_INCLUDED
#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "InstrumentMappingEditor.h"
#include "Adsr.h"

class WaveformView;

class ZoneInfo : public Component, public ChangeListener, public Label::Listener, public Button::Listener {
    std::shared_ptr<InstrumentMappingEditor> mappingEditor;
    SelectedItemSet<InstrumentMappingEditor::MappingEditorGraph::Zone::Ptr>* zone;
    ScopedPointer<Label> fileName;
    ScopedPointer<Label> fileNameLabel;
    ScopedPointer<Label> noteNumber;
    ScopedPointer<Label> noteNumberLabel;
    ScopedPointer<Label> noteName;
    ScopedPointer<Label> velocityLabel;
    ScopedPointer<Label> velocityMin;
    ScopedPointer<Label> velocityMax;
    ScopedPointer<TextButton> vLayout;
    ScopedPointer<TextButton> hLayout;
    Array<String> noteNames;
    WaveformView* audio_thumbnail;

public:
    ZoneInfo(std::shared_ptr<InstrumentMappingEditor> m);
    void labelTextChanged(Label* source);
    WaveformView* getAudioThumbnai(){return audio_thumbnail;}
    void changeListenerCallback(ChangeBroadcaster* source);
    void resize();
    void paint(Graphics& g);
    void mouseMove(const MouseEvent& e){}
    void buttonClicked(Button*);
    //Adsr adsr;
};

#endif  // ZONEINFO_H_INCLUDED
