/*
  ==============================================================================

    MainPageComponents.cpp
    Created: 2 Aug 2014 12:48:17pm
    Author:  patrick

  ==============================================================================
*/

#include "MainPageComponents.h"

MainHorizontalSlider::MainHorizontalSlider(String s, double minimum, double maximum, double interval) : 
                                           Slider(), id(0), name(s)
{
    setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(name);
}

MainVerticalSlider::MainVerticalSlider(String s, double minimum, double maximum, double interval) : 
                                           Slider(), id(0), name(s)
{
    setSliderStyle(Slider::SliderStyle::LinearVertical);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setRange(minimum, maximum, interval);
    setName(name);
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