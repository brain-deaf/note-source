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
    InstrumentTabWindow(TabbedButtonBar::Orientation orientation, std::shared_ptr<AudioDeviceManager>& audio);
    ~InstrumentTabWindow();
    
private:
    std::shared_ptr<AudioDeviceManager> content_component;
    ScopedPointer<MappingEditorBin> mapping_editor;
    ScopedPointer<TextButton> zoom_button;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentTabWindow)
};



#endif  // INSTRUMENTTABWINDOW_H_INCLUDED
