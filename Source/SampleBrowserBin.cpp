/*
  ==============================================================================

    SamplerBrowserBin.cpp
    Created: 10 Aug 2014 9:53:59am
    Author:  patrick

  ==============================================================================
*/

#include "SampleBrowserBin.h"

SampleBrowserBin::SampleBrowserBin() : Component(),
                                       buttonHeight(20),
                                       dragButton(new DragButton()),
                                       browser(new SampleFileBrowser())
{
    addAndMakeVisible(browser);
    addAndMakeVisible(dragButton);
    
    browser->setDragButton(dragButton);
}
                                       
void SampleBrowserBin::resized(){
    browser->setBounds(0, 0, getWidth(), getHeight() - buttonHeight);
    dragButton->setBounds(0, getHeight() - buttonHeight, getWidth(), buttonHeight);
}
