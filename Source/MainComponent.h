/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "MenuBar.h"
#include "InstrumentBin.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public Component
{
public:
    MainContentComponent();
    void resized();
private:
    SharedResourcePointer<AudioDeviceManager> device_manager;
    MenuBar menu_bar;
    InstrumentBin instrument_bin;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
