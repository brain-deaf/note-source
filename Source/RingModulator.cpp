/*
  ==============================================================================

    RingModulator.cpp
    Created: 17 Aug 2014 10:24:13pm
    Author:  patrick

  ==============================================================================
*/

#include "RingModulator.h"

RingModulator::RingModulator(){
    frequency = new MidiSlider();
    frequency->setSliderStyle(Slider::RotaryVerticalDrag);
    frequency->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    frequency->setRange(100.0, 1000.0);
    
    amplitude = new MidiSlider();
    amplitude->setSliderStyle(Slider::RotaryVerticalDrag);
    amplitude->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    amplitude->setRange(0.0, 1.0);
    amplitude->setValue(1.0);
    
    addAndMakeVisible(frequency);
    addAndMakeVisible(amplitude);
}

void RingModulator::resized(){
    frequency->setBounds(0, getHeight()/2, 50, 50);
    amplitude->setBounds(70, getHeight()/2, 50, 50);
}

void RingModulator::paint(Graphics& g){
    g.fillAll(Colours::purple);
}