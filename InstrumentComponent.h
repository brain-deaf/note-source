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

class InstrumentComponent : public Component
{
public:
    InstrumentComponent(TabbedComponent* Parent, AudioDeviceManager* audio);
    ~InstrumentComponent();
    void resized();
private:
    AudioDeviceManager* content_component;
    ScopedPointer<InstrumentTabWindow> tabbed_component;
    ScopedPointer<InstrumentMasterComponent> instrument_master_component;
    TabbedComponent* parent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentComponent)
};



#endif  // INSTRUMENTCOMPONENT_H_INCLUDED
