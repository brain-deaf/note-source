/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "MenuBar.h"

//==============================================================================
MainContentComponent::MainContentComponent() : deviceManager{},
    transport(), metronome(new MetronomeComponent()),
    toggleBrowser(new BrowserButton())
{
    browser = new SampleFileBrowser();
    instrumentBin = new InstrumentBin(TabbedButtonBar::TabsAtTop, this);
    menuBar = new MenuBar(instrumentBin);
    XmlDocument xml_file(File::getCurrentWorkingDirectory().getChildFile("audio_settings.xml"));
    deviceManager->initialise (0,2,xml_file.getDocumentElement(),true);
    
    setSize (1000, 829);
    addAndMakeVisible (instrumentBin);
    addAndMakeVisible (menuBar);
    addAndMakeVisible(metronome);
    addAndMakeVisible(&transport);
    //addAndMakeVisible(browser);
    addAndMakeVisible(toggleBrowser);
    toggleBrowser->addListener(this);
}

void MainContentComponent::resized()
{
    
    if (!toggleBrowser->getToggleState()){
        menuBar->setBounds(0, 0, getWidth(), 20);
        instrumentBin->setBounds(0, 80, getWidth(), getHeight() - 80);
        transport.setBounds(getWidth()/3*2, 20, getWidth()/3, 80);
        metronome->setBounds(getWidth()/5*3, 20, getWidth()/3*2 - (getWidth()/5*3) - 10, 80);
        toggleBrowser->setBounds(5, 25, 50, 20);
    }else{
        setSize(1000+browser->getWidth(), getHeight());
        menuBar->setBounds(0+browser->getWidth(), 0, getWidth(), 20);
        instrumentBin->setBounds(0+browser->getWidth(), 80, getWidth()-browser->getWidth(), getHeight() - 80);
        transport.setBounds((getWidth() - browser->getWidth())/3*2+browser->getWidth(), 20, (getWidth()-browser->getWidth())/3, 80);
        metronome->setBounds((getWidth() - browser->getWidth())/5*3+browser->getWidth(), 20, (getWidth()-browser->getWidth())/3*2 - ((getWidth()-browser->getWidth())/5*3) - 10, 80);
        toggleBrowser->setBounds(5+browser->getWidth(), 25, 50, 20);
    }
    
}

void MainContentComponent::buttonClicked(Button* source){
    if (source == toggleBrowser){
        if (toggleBrowser->getToggleState()){
            setSize(1000+browser->getWidth(), getHeight());
            addAndMakeVisible(browser);
            browser->setBounds(0, 0, 300, getHeight());
            resized();
        }else{
            setSize(1000, getHeight());
            removeChildComponent(browser);
            resized();
        }
    }
}
