/*
  ==============================================================================

    instrumentMasterComponent.cpp
    Created: 9 May 2014 6:25:05pm
    Author:  patrick

  ==============================================================================
*/

#include "InstrumentMasterComponent.h"
#include "InstrumentComponent.h"

InstrumentMasterComponent::InstrumentMasterComponent(TabbedComponent* p, InstrumentComponent* i)
: Component{}, parent{p}, midi_input_menu(new ComboBox()),
    instrument_parent(i),
    instrumentLabel{"component name", "New Instrument", p},
    midi_input_lbl(new Label("MIDI Input Channel")),
    xButton{"close", Colour(50, 50, 50), Colour(60, 60, 60), 
        Colour(0, 20, 90), parent} {
    instrumentLabel.setEditable(false, true, true);
    instrumentLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(instrumentLabel);
    xButton.addListener(this);
    addAndMakeVisible(xButton);
    xButton.repaint();
    addAndMakeVisible(midi_input_menu);
    midi_input_menu->addItem("Omni", -1);
    midi_input_menu->addItem("1", 1);
    midi_input_menu->addItem("2", 2);
    midi_input_menu->addItem("3", 3);
    midi_input_menu->addItem("4", 4);
    midi_input_menu->addItem("5", 5);
    midi_input_menu->addItem("6", 6);
    midi_input_menu->addItem("7", 7);
    midi_input_menu->addItem("8", 8);
    midi_input_menu->addItem("9", 9);
    midi_input_menu->addItem("10", 10);
    midi_input_menu->addItem("11", 11);
    midi_input_menu->addItem("12", 12);
    midi_input_menu->addItem("13", 13);
    midi_input_menu->addItem("14", 14);
    midi_input_menu->addItem("15", 15);
    midi_input_menu->addItem("16", 16);
    addAndMakeVisible(midi_input_lbl);
    midi_input_lbl->setText("MIDI Input Channel", dontSendNotification);
    midi_input_menu->setSelectedId(omni);
    
    instrument_parent->getTabWindow().getMappingEditorBin()->
    getMappingEditor()->graph->getMidiCallback().setMidiChannel(-1);
    
    midi_input_menu->addListener(this);
}

void InstrumentMasterComponent::paint (Graphics& g){
    g.fillAll (Colours::grey);
}

void InstrumentMasterComponent::resized(){
    instrumentLabel.setBounds(0, 0, getWidth(), getHeight());
    xButton.setBounds(getWidth() - 16, 4, 12, 12);
    midi_input_menu->setBounds(28, 52, 70, 20);
    midi_input_lbl->setBounds(5, 31, 120, 20);
}

void InstrumentMasterComponent::buttonClicked(Button* button){
    if (button == &xButton){
        parent->removeTab(parent->getCurrentTabIndex());
    }
}

void InstrumentMasterComponent::comboBoxChanged(ComboBox* source){
    if (source == midi_input_menu){
        instrument_parent->getTabWindow().getMappingEditorBin()->
            getMappingEditor()->graph->getMidiCallback().setMidiChannel(midi_input_menu->getSelectedId());
        
        instrument_parent->getTabWindow().getMappingEditorBin()->
            getMappingEditor()->graph->getSampler().setMidiChannel(midi_input_menu->getSelectedId());
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


