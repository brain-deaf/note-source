/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent() : deviceManager{},
    instrumentBin{TabbedButtonBar::TabsAtTop},
    menuBar{&instrumentBin} {

    XmlDocument xml_file(File::getCurrentWorkingDirectory().getChildFile("audio_settings.xml"));
    deviceManager->initialise (0,2,xml_file.getDocumentElement(),true);
    
    addAndMakeVisible (instrumentBin);
    setSize (1000, 730);
    addAndMakeVisible (menuBar);
}

void MainContentComponent::resized()
{
    menuBar.setBounds(0, 0, getWidth(), 20);
    instrumentBin.setBounds(0, 30, getWidth(), getHeight() - 30);
}
