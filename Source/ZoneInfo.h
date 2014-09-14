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
class Zone;

class ZoneInfo : public Component, public ChangeListener, public Label::Listener, public Button::Listener {
    std::shared_ptr<InstrumentMappingEditor> mappingEditor;
	SelectedItemSet<ReferenceCountedObjectPtr<Zone> >* zone;
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
    ScopedPointer<WaveformView> audio_thumbnail;

public:
    ZoneInfo(std::shared_ptr<InstrumentMappingEditor> m);
	~ZoneInfo();
	
    void labelTextChanged(Label* source);
    WaveformView* getAudioThumbnai(){return audio_thumbnail;}
    void changeListenerCallback(ChangeBroadcaster* source);
    void resize();
    void paint(Graphics& g);
    void mouseMove(const MouseEvent& ){}
    void buttonClicked(Button*);
    //Adsr adsr;
};


class ZoneSorterByNote
{
public:
	static int compareElements(Zone* z1, Zone* z2);
};

class ZoneSorterByVelocity
{
public:
	static int compareElements(Zone* z1, Zone* z2);
};

#endif  // ZONEINFO_H_INCLUDED
