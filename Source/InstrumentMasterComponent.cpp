/*
  ==============================================================================

    instrumentMasterComponent.cpp
    Created: 9 May 2014 6:25:05pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentMasterComponent.h"


InstrumentMasterComponent::InstrumentMasterComponent(TabbedComponent* p)
: Component{}, parent{p}, 
    instrumentLabel{"component name", "New Instrument"},
    xButton{"close", Colour(50, 50, 50), Colour(60, 60, 60), 
        Colour(0, 20, 90)} {
    instrumentLabel.setEditable(false, true, true);
    instrumentLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(instrumentLabel);
    xButton.addListener(this);
    addAndMakeVisible(xButton);
}

void InstrumentMasterComponent::paint (Graphics& g){
    g.fillAll (Colours::grey);
}

void InstrumentMasterComponent::resized(){
    instrumentLabel.setBounds(0, 0, getWidth(), getHeight());
    xButton.setBounds(getWidth() - 16, 4, 12, 12);
}

void InstrumentMasterComponent::buttonClicked(Button* button){
    if (button == &xButton){
        parent->removeTab(parent->getCurrentTabIndex());
    }
}

void InstrumentMasterComponent::InstrumentLabel::textWasEdited(){
    parent->setTabName(parent->getCurrentTabIndex(), getText());
}

void InstrumentMasterComponent::InstrumentButton::paintButton(Graphics& g, bool isMouseOver, bool isButtonDown){
    Path myPath;
    myPath.startNewSubPath (0.0f, 0.0f);    // move the current position to (10, 10)
    myPath.lineTo (12.0f, 12.0f);           // draw a line from here to (100, 200)
    myPath.startNewSubPath (12.0f, 0.0f);
    myPath.lineTo (0.0f, 12.0f);
    g.strokePath (myPath, PathStrokeType (2.0f));
}
