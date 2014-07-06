/*
  ==============================================================================

    Adsr.cpp
    Created: 5 Jul 2014 2:23:07pm
    Author:  patrick

  ==============================================================================
*/

#include "Adsr.h"
#include <math.h>

Adsr::Adsr() : Component(), adsr_plot(this), attack_time(), curve(){
    addAndMakeVisible(&adsr_plot);
    
    attack_time.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    attack_time.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    attack_time.setRange(0.0, 600.0, 0.1);
    attack_time.setValue(5.0);
    attack_time.addListener(this);
    addAndMakeVisible(&attack_time);
    
    curve.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    curve.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    curve.setRange(-0.1, 0.1);
    curve.setValue(0.0001);
    curve.addListener(this);
    addAndMakeVisible(&curve);
}

void Adsr::resized(){
    adsr_plot.setBounds(0, 0, getWidth(), getHeight() - 80);
    attack_time.setBounds(0, getHeight() - 90, 140, 80);
    curve.setBounds(150, getHeight() - 90, 140, 80);
}

void Adsr::paint(Graphics& g){
    g.fillAll(Colours::yellow);
}

void Adsr::sliderValueChanged(Slider* slider){
    adsr_plot.repaint();
    if (slider == &curve && curve.getValue() == 0){
        curve.setValue(0.00001);
    }
}


//https://www.desmos.com/calculator/g6a0fpawvq
//y = (y2-y1)/(e^wx2 - e^wx1) * (e^wx - e^wx1) + y1
//time = x2
//max_volume = y2


//y = max_volume / (e^curve_width*time - 1) * (e^curve_width*X - 1)
static double plotAdsr(int time, int max_volume, double curve_width, int x){
    return(max_volume / (pow(M_E, curve_width*time) - 1) * (pow(M_E, curve_width*x) - 1));
}

Adsr::Graph::Graph(Adsr* _parent) : Component(), parent(_parent){}

void Adsr::Graph::paint(Graphics& g){
    g.fillAll(Colours::black);
    
    g.setColour(Colours::white);
    g.setOpacity(1.0f);
    Path myPath;
    myPath.startNewSubPath(0, getHeight());
    
    int max_volume(getHeight());
    double time(parent->getAttackTimeSlider()->getValue());
    double curve_width(parent->getCurveSlider()->getValue());
    
    for (int i=0; i <= time; i++){
        myPath.lineTo (i, getHeight() - plotAdsr(time, max_volume, curve_width, i));
    }
    myPath.startNewSubPath(time, 0);
    myPath.lineTo(time, getHeight());

    g.strokePath (myPath, PathStrokeType (1.0f));
}



