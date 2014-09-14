/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED
class Sampler;


#include "../JuceLibraryCode/JuceHeader.h"
#include "MenuBar.h"
#include "InstrumentBin.h"
#include "MasterComponents.h"
#include "SampleBrowserBin.h"
#include "InstrumentMappingEditor.h"



//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MenuBar;
class InstrumentBin;
class Sampler;
class MainContentComponent : public Component, public Button::Listener, public DragAndDropContainer
{
public:
    MainContentComponent();
    ~MainContentComponent()
	{
		instrumentBin = nullptr; 
		menuBar = nullptr; 
		metronome = nullptr;
		toggleBrowser = nullptr;
		delete _static_sampler;
	}
    MetronomeComponent* getMetronome(){return metronome.get();}
    TransportComponent* getTransport(){return &transport;}
    void resized();
    void buttonClicked(Button*);
	static Sampler* _static_sampler;
private:
    SharedResourcePointer<AudioDeviceManager> deviceManager;
    ScopedPointer<MenuBar> menuBar;
    ScopedPointer<MetronomeComponent> metronome;
    TransportComponent transport;
    ScopedPointer<InstrumentBin> instrumentBin;
    ScopedPointer<SampleBrowserBin> browser;
    ScopedPointer<BrowserButton> toggleBrowser;
    int browserWidth;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
