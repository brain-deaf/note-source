/*
  ==============================================================================

    InstrumentComponent.h
    Created: 9 May 2014 6:12:43pm
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTCOMPONENT_H_INCLUDED
#define INSTRUMENTCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "InstrumentTabWindow.h"
#include "InstrumentMasterComponent.h"
#include "FilePlayer.h"

class InstrumentComponent : public Component
{
public:
    InstrumentComponent(TabbedComponent* Parent);
    void resized();
    void addFilePlayer(std::shared_ptr<FilePlayer>);
private:
    ScopedPointer<AudioSource> mixer;
    Array<std::shared_ptr<FilePlayer>> sources;
    ScopedPointer<InstrumentTabWindow> tabbed_component;
    ScopedPointer<InstrumentMasterComponent> instrument_master_component;
    TabbedComponent* parent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentComponent)
};



#endif  // INSTRUMENTCOMPONENT_H_INCLUDED
