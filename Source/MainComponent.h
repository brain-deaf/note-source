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
#include "MasterComponents.h"
#include "SampleFileBrowser.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public Component, public Button::Listener
{
public:
    MainContentComponent();
    ~MainContentComponent(){delete instrumentBin; delete menuBar; delete metronome; delete browser;}
    MetronomeComponent* getMetronome(){return metronome;}
    TransportComponent* getTransport(){return &transport;}
    void resized();
    void buttonClicked(Button*);
private:
    SharedResourcePointer<AudioDeviceManager> deviceManager;
    MenuBar* menuBar;
    MetronomeComponent* metronome;
    TransportComponent transport;
    InstrumentBin* instrumentBin;
    SampleFileBrowser* browser;
    BrowserButton* toggleBrowser;
    int browserWidth;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
