/*
  ==============================================================================

    instrumentMasterComponent.cpp
    Created: 9 May 2014 6:25:05pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentMasterComponent.h"


InstrumentMasterComponent::InstrumentMasterComponent(TabbedComponent* Parent)
: Component{}, parent{Parent}{
    instrument_label = new InstrumentLabel("component name", "New Instrument", parent);
    instrument_label->setEditable(false, true, true);
    instrument_label->setJustificationType(Justification::centred);
    addAndMakeVisible(instrument_label);

    x_button = new InstrumentButton("close", Colour(50, 50, 50), Colour(60, 60, 60), Colour(0, 20, 90), parent);
    x_button->addListener(this);
    addAndMakeVisible(x_button);
}

InstrumentMasterComponent::~InstrumentMasterComponent(){
    instrument_label = nullptr;
    x_button = nullptr;
}

void InstrumentMasterComponent::paint (Graphics& g){
    g.fillAll (Colours::grey);
}

void InstrumentMasterComponent::resized(){
    instrument_label->setBounds(0, 0, getWidth(), getHeight());
    x_button->setBounds(getWidth() - 16, 4, 12, 12);
}

void InstrumentMasterComponent::buttonClicked(Button* button){
    if (button == x_button){
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
