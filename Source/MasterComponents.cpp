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
    playButton->setClickingTogglesState(true);
    stopButton = new TextButton("stop");
    pauseButton = new TextButton("pause");
    pauseButton->setClickingTogglesState(true);
    tempoSlider = new Slider();
    tempoSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    tempoSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, false, 150, 30);
    tempoSlider->setTextBoxIsEditable(true);
    tempoSlider->setRange(0.1, 300.0);
    tempoSlider->setValue(120.0);
    
    playButton->addListener(this);
    pauseButton->addListener(this);
    stopButton->addListener(this);
    tempoSlider->addListener(this);
    
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(tempoSlider);
}

TransportComponent::~TransportComponent(){
	playButton = nullptr;
	stopButton = nullptr;
	pauseButton = nullptr;
	tempoSlider = nullptr;
}

void TransportComponent::paint(Graphics& ){
    //g.fillAll(Colours::red);
}

void TransportComponent::resized(){
    double button_width = getWidth() / 3;
    double button_height = 20;
    
    playButton->setBounds(0, getHeight() - button_height, button_width, button_height);
    pauseButton->setBounds(button_width*1, getHeight() - button_height, button_width, button_height);
    stopButton->setBounds(button_width*2, getHeight() - button_height, button_width, button_height);
    tempoSlider->setBounds(65, 0, 200, getHeight() - button_height);
}

void TransportComponent::buttonClicked(Button* source){
    if (source == pauseButton){
        if (pauseButton->getToggleState()){
            if (playButton->getToggleState()){
                playButton->setToggleState(false, dontSendNotification);
                MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
                if (metronome != nullptr) m->setTransport(false);
            }else{
                pauseButton->setToggleState(false, dontSendNotification);
            }
        }else{
            playButton->setToggleState(true, dontSendNotification);
            MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
            if (metronome != nullptr) m->setTransport(true);
        }
    }
    else if(source == playButton){
        if (playButton->getToggleState()){
            MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
            if (metronome != nullptr) m->setTransport(true);
            pauseButton->setToggleState(false, dontSendNotification);
        }else{
            MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
            if (metronome != nullptr) m->setTransport(false);
        }
    }
    else if (source == stopButton){
        playButton->setToggleState(false, dontSendNotification);
        pauseButton->setToggleState(false, dontSendNotification);
        MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
        if (metronome != nullptr){ 
            m->setTransport(false);
            m->reset();
        }
    }
}

void TransportComponent::sliderValueChanged(Slider* s){
    if (s == tempoSlider){
        MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
        if (metronome != nullptr) m->setTempo(s->getValue());
    }
}
        

MetronomeComponent::MetronomeComponent() : Component()
{
    clickButton = new TextButton("click");
    clickButton->setClickingTogglesState(true);
    volumeSlider = new Slider();
    volumeSlider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    volumeSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    volumeSlider->setRange(0.0, 1.0);
    volumeSlider->setValue(0.5);
    
    clickButton->addListener(this);
    volumeSlider->addListener(this);
    
    addAndMakeVisible(clickButton);
    addAndMakeVisible(volumeSlider);
}

MetronomeComponent::~MetronomeComponent(){
    delete clickButton;
    delete volumeSlider;
}

void MetronomeComponent::resized(){
    volumeSlider->setBounds(0, 5, getWidth(), getHeight() - 28);
    clickButton->setBounds(0, getHeight()-20, getWidth(), 20);
}

void MetronomeComponent::paint(Graphics& ){
}

void MetronomeComponent::buttonClicked(Button* source){
    if (source == clickButton){
        if (metronome != nullptr){
            MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
            m->setClick(clickButton->getToggleState());
        }
    }
}

void MetronomeComponent::sliderValueChanged(Slider* s){
    if (s == volumeSlider){
        if (metronome != nullptr){
            MetronomeVoice* m = static_cast<MetronomeVoice*>(metronome->getSynth()->getVoice(0));
            m->setVolume(s->getValue());
        }
    }
}

BrowserButton::BrowserButton() : TextButton("Browser"){
    setClickingTogglesState(true);
}
        


    