/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent() : menu_bar(), instrument_bin(TabbedButtonBar::TabsAtTop, menu_bar.device_manager()) {
    addAndMakeVisible(instrument_bin);
    setSize (1000, 730);
    addAndMakeVisible(menu_bar);
    menu_bar.set_parent_instrument_bin(&instrument_bin);
}

MainContentComponent::~MainContentComponent() {
}

void MainContentComponent::resized()
{
    menu_bar.setBounds(0, 0, getWidth(), 20);
    instrument_bin.setBounds(0, 30, getWidth(), getHeight() - 30);
}
