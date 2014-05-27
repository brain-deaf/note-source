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
#include "MappingEditorBin.h"

class InstrumentTabWindow : public TabbedComponent
{
public:
    InstrumentTabWindow(TabbedButtonBar::Orientation orientation);
    ~InstrumentTabWindow();
    
private:
    ScopedPointer<MappingEditorBin> mapping_editor;
    ScopedPointer<TextButton> zoom_button;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentTabWindow)
};



#endif  // INSTRUMENTTABWINDOW_H_INCLUDED
