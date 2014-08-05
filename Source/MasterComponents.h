/*
  ==============================================================================

    MasterComponents.h
    Created: 5 Aug 2014 3:58:51pm
    Author:  patrick

  ==============================================================================
*/

#ifndef MASTERCOMPONENTS_H_INCLUDED
#define MASTERCOMPONENTS_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

class TransportComponent : public Component, public Button::Listener
{
public:
    TransportComponent();
    ~TransportComponent();
    void paint(Graphics&);
    void resized();
    void buttonClicked(Button*);
private:
    TextButton* playButton;
    TextButton* pauseButton;
    TextButton* stopButton;
    Slider* tempoSlider;
};

class MetronomeComponent : public Component
{
public:
    MetronomeComponent();
    ~MetronomeComponent();
    void paint(Graphics&);
    void resized();
    
    TextButton* getClickButton(){return clickButton;}
    Slider* getVolumeSlider(){return volumeSlider;}
private:
    TextButton* clickButton;
    Slider* volumeSlider;
};



#endif  // MASTERCOMPONENTS_H_INCLUDED
