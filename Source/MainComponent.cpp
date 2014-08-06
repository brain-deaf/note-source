/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent() : deviceManager{},
    instrumentBin{TabbedButtonBar::TabsAtTop}, transport{}, metronome{},
    menuBar{&instrumentBin} 
{
    XmlDocument xml_file(File::getCurrentWorkingDirectory().getChildFile("audio_settings.xml"));
    deviceManager->initialise (0,2,xml_file.getDocumentElement(),true);
    
    setSize (1000, 830);
    addAndMakeVisible (instrumentBin);
    addAndMakeVisible (menuBar);
    addAndMakeVisible(&metronome);
    addAndMakeVisible(&transport);
}

void MainContentComponent::resized()
{
    menuBar.setBounds(0, 0, getWidth(), 20);
    instrumentBin.setBounds(0, 80, getWidth(), getHeight() - 80);
    transport.setBounds(getWidth()/3*2, 20, getWidth()/3, 80);
    metronome.setBounds(getWidth()/5*3, 20, getWidth()/3*2 - (getWidth()/5*3) - 10, 80);
}
