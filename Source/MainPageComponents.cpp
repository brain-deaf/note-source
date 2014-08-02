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