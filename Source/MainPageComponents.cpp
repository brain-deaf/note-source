/*
  ==============================================================================

    MainPageComponents.cpp
    Created: 2 Aug 2014 12:48:17pm
    Author:  patrick

  ==============================================================================
*/

#include "MainPageComponents.h"

MainHorizontalSlider::MainHorizontalSlider(String s, double minimum, double maximum, double interval) 
 : Slider(), id(0), name(s)
{
    setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(name);
}

MainVerticalSlider::MainVerticalSlider(String s, double minimum, double maximum, double interval) : 
                                       Slider(), id(0), name(s), image(nullptr)
{
    setSliderStyle(Slider::SliderStyle::LinearVertical);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(name);
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
    if (getImage()->isNull()){
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

MainKnob::MainKnob(String s, double minimum, double maximum, double interval) : 
                                           Slider(), id(0), name(s)
{
    setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(name);
}

MainButton::MainButton(String s) : TextButton(""), id(0), name(s)
{
    setName(name);
}

MainLabel::MainLabel(String s, String text) : Label(), id(0), name(s)
{
    setName(name);
    setText(text, dontSendNotification);
}

MainComboBox::MainComboBox(String s) : ComboBox(""), id(0), name(s)
{
    setName(name);
    setJustificationType(Justification::Flags::centred);
}