/*
  ==============================================================================

    MasterComponents.cpp
    Created: 5 Aug 2014 3:58:34pm
    Author:  patrick

  ==============================================================================
*/

#include "MasterComponents.h"

TransportComponent::TransportComponent() : Component()
{
    playButton = new TextButton("play");
    stopButton = new TextButton("stop");
    pauseButton = new TextButton("pause");
    tempoSlider = new Slider();
    tempoSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    tempoSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, false, 150, 30);
    tempoSlider->setTextBoxIsEditable(true);
    tempoSlider->setRange(0.1, 300.0);
    
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(tempoSlider);
}

TransportComponent::~TransportComponent(){
    delete playButton;
    delete stopButton;
    delete pauseButton;
    delete tempoSlider;
}

void TransportComponent::paint(Graphics& g){
    //g.fillAll(Colours::red);
}

void TransportComponent::resized(){
    double button_width = getWidth() / 3;
    double button_height = 20;
    
    playButton->setBounds(0, getHeight() - button_height, button_width, button_height);
    pauseButton->setBounds(button_width*1, getHeight() - button_height, button_width, button_height);
    stopButton->setBounds(button_width*2, getHeight() - button_height, button_width, button_height);
    tempoSlider->setBounds(0, 0, 200, getHeight() - button_height);
}