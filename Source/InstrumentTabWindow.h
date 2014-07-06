/*
  ==============================================================================

    InstrumentTabWindow.h
    Created: 9 May 2014 6:26:40pm
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTTABWINDOW_H_INCLUDED
#define INSTRUMENTTABWINDOW_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainPage.h"
#include "MappingEditorBin.h"
#include "FxBin.h"

class InstrumentTabWindow : public TabbedComponent
{
public:
    InstrumentTabWindow(InstrumentComponent& instrument, TabbedButtonBar::Orientation orientation);
    MappingEditorBin* getMappingEditorBin(){return mappingEditor;}
private:
    ScopedPointer<MainPage> mainPage;
    ScopedPointer<MappingEditorBin> mappingEditor;
    ScopedPointer<FxBin> fxbin;
    ScopedPointer<TextButton> zoomButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentTabWindow)
};

#endif  // INSTRUMENTTABWINDOW_H_INCLUDED

