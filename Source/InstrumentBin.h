/*
  ==============================================================================

    instrumentBin.h
    Created: 8 May 2014 6:58:34pm
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTBIN_H_INCLUDED
#define INSTRUMENTBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "InstrumentComponent.h"

class InstrumentBin : public TabbedComponent {
public:
    InstrumentBin(TabbedButtonBar::Orientation orientation);
    ~InstrumentBin();
    void registerTab(InstrumentComponent* c);

private:
    InstrumentComponent* instrumentComponent;
    Array<InstrumentComponent*> instruments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentBin)
};

#endif  // INSTRUMENTBIN_H_INCLUDED
