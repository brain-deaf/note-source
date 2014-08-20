/*
  ==============================================================================

    MainPageComponents.cpp
    Created: 2 Aug 2014 12:48:17pm
    Author:  patrick

  ==============================================================================
*/

#include "MainPageComponents.h"

MainHorizontalSlider::MainHorizontalSlider(String n, int i, double minimum, double maximum, double interval) 
 : MidiSlider(), id(i), image(nullptr)
{
    setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(n);
}

void MainHorizontalSlider::setImage(String imagePath, int numFrames){
    if (image != nullptr){ 
        delete image;
        image = nullptr;
    }
    
    File imageFile(File(File::getCurrentWorkingDirectory().getFullPathName() + "/" + imagePath));
    PNGImageFormat p;
    image = new Image(PNGImageFormat::loadFrom(*imageFile.createInputStream()));
    setNumFrames(numFrames);
}

void MainHorizontalSlider::paint(Graphics& g){
    if (getImage() == nullptr){
        Slider::paint(g);
    }else{
        double slider_range = getMaximum() - getMinimum();
        double values_per_frame = slider_range / getNumFrames();
        double pixels_per_frame = getImage()->getHeight() / getNumFrames();
        int frame = getValue() / values_per_frame;
        if (frame >= getNumFrames()) frame=getNumFrames()-1;
        
        g.drawImage(*getImage(), 0, 0, getWidth(), getHeight(), 
                    0, frame*pixels_per_frame, getImage()->getWidth(), pixels_per_frame);
    }       
}

MainVerticalSlider::MainVerticalSlider(String n, int i, double minimum, double maximum, double interval) : 
                                       MidiSlider(), id(i), image(nullptr)
{
    setSliderStyle(Slider::SliderStyle::LinearVertical);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(n);
}

void MainVerticalSlider::setImage(String imagePath, int numFrames){
    if (image != nullptr){ 
        delete image;
        image = nullptr;
    }
    
    File imageFile(File(File::getCurrentWorkingDirectory().getFullPathName() + "/" + imagePath));
    PNGImageFormat p;
    image = new Image(PNGImageFormat::loadFrom(*imageFile.createInputStream()));
    setNumFrames(numFrames);
}

void MainVerticalSlider::paint(Graphics& g){
    if (getImage() == nullptr){
        Slider::paint(g);
    }else{
        double slider_range = getMaximum() - getMinimum();
        double values_per_frame = slider_range / getNumFrames();
        double pixels_per_frame = getImage()->getHeight() / getNumFrames();
        int frame = getValue() / values_per_frame;
        if (frame >= getNumFrames()) frame=getNumFrames()-1;
        
        g.drawImage(*getImage(), 0, 0, getWidth(), getHeight(), 
                    0, frame*pixels_per_frame, getImage()->getWidth(), pixels_per_frame);
    }       
}

MainKnob::MainKnob(String n, int i, double minimum, double maximum, double interval) : 
                                           MidiSlider(), id(i)
{
    setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(n);
}

MainButton::MainButton(String n, int i) : TextButton(""), id(i)
{
    setName(n);
}

MainLabel::MainLabel(int i, String text) : Label(), id(i)
{
    setText(text, dontSendNotification);
}

MainComboBox::MainComboBox(String n, int i) : ComboBox(""), id(i)
{
    setJustificationType(Justification::Flags::centred);
    setName(n);
}