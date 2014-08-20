/*
  ==============================================================================

    Adsr.cpp
    Created: 5 Jul 2014 2:23:07pm
    Author:  patrick

  ==============================================================================
*/

#include "Adsr.h"
#include <math.h>

Adsr::Adsr() : Component(), adsr_plot(this), attack_time(new MidiSlider()), attack_curve(new MidiSlider()),
    decay_time(new MidiSlider()), decay_curve(new MidiSlider()), sustain(new MidiSlider()), release_time(new MidiSlider()), release_curve(new MidiSlider()){
    addAndMakeVisible(&adsr_plot);
    
    attack_time->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    attack_time->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    attack_time->setRange(1.0, 600.0, 0.1);
    attack_time->setValue(180.0);
    attack_time->addListener(this);
    addAndMakeVisible(attack_time);
    
    attack_curve->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    attack_curve->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    attack_curve->setRange(-0.1, 0.1);
    attack_curve->setValue(-0.05);
    attack_curve->addListener(this);
    addAndMakeVisible(attack_curve);
    
    decay_time->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    decay_time->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    decay_time->setRange(1.0, 600.0, 0.1);
    decay_time->setValue(100.0);
    decay_time->addListener(this);
    addAndMakeVisible(decay_time);
    
    decay_curve->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    decay_curve->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    decay_curve->setRange(-0.1, 0.1);
    decay_curve->setValue(-0.05);
    decay_curve->addListener(this);
    addAndMakeVisible(decay_curve);
    
    sustain->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    sustain->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    sustain->setRange(0.0, 200.0, 0.1);
    sustain->setValue(150.0);
    sustain->addListener(this);
    addAndMakeVisible(sustain);
    
    release_time->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    release_time->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    release_time->setRange(1.0, 600.0, 0.1);
    release_time->setValue(100.0);
    release_time->addListener(this);
    addAndMakeVisible(release_time);
    
    release_curve->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    release_curve->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight,
                                true, 100, 20);
    release_curve->setRange(-0.1, 0.1);
    release_curve->setValue(-0.05);
    release_curve->addListener(this);
    addAndMakeVisible(release_curve);
}

void Adsr::resized(){
    adsr_plot.setBounds(0, 0, getWidth(), getHeight() - 120);
    attack_time->setBounds(0, getHeight() - 120, 140, 40);
    attack_curve->setBounds(0, getHeight() - 80, 140, 40);
    decay_time->setBounds(150, getHeight() - 120, 140, 40);
    decay_curve->setBounds(150, getHeight() - 80, 140, 40);
    sustain->setBounds(300, getHeight() - 120, 140, 40);
    sustain->setRange(0.0, getHeight(), 0.1);
    release_time->setBounds(450, getHeight() - 120, 140, 40);
    release_curve->setBounds(450, getHeight() - 80, 140, 40);
}

void Adsr::paint(Graphics& g){
    g.fillAll(Colours::yellow);
}

void Adsr::sliderValueChanged(Slider* slider){
    adsr_plot.repaint();
    if (slider == attack_curve && attack_curve->getValue() == 0){
        attack_curve->setValue(0.00001);
    }
    if (slider == decay_curve && decay_curve->getValue() == 0){
        decay_curve->setValue(0.00001);
    }
    if (slider == release_curve && release_curve->getValue() == 0){
        release_curve->setValue(0.00001);
    }
}


//https://www.desmos.com/calculator/g6a0fpawvq
//y = (y2-y1)/(e^wx2 - e^wx1) * (e^wx - e^wx1) + y1
//time = x2
//max_volume = y2


//y = max_volume / (e^curve_width*time - 1) * (e^curve_width*X - 1)
static double plotAdsr(int x1, int time, int y1, int max_volume, double curve_width, int x){
    return(max_volume - y1) / (pow(M_E, curve_width*time) - pow(M_E, curve_width*x1)) * (pow(M_E, curve_width*x) - pow(M_E, curve_width*x1)) + y1;
}

Adsr::Graph::Graph(Adsr* _parent) : Component(), parent(_parent){std::cout<<"adsr ctor"<<std::endl;}

void Adsr::Graph::paint(Graphics& g){
    g.fillAll(Colours::black);
    
    g.setColour(Colours::white);
    g.setOpacity(1.0f);
    Path myPath;
    myPath.startNewSubPath(0, getHeight());
    
    //attack
    int max_volume(getHeight());
    double atk_time(parent->getAttackTimeSlider()->getValue());
    double atk_curve_width(parent->getAttackCurveSlider()->getValue());
    
    for (int i=0; i <= atk_time; i++){
        myPath.lineTo (i, getHeight() - plotAdsr(0, atk_time, 0, max_volume, atk_curve_width, i));
    }
    
    //decay
    double dec_time = parent->getDecayTimeSlider()->getValue();
    double dec_curve_width = parent->getDecayCurveSlider()->getValue();
    double sustain_level = parent->getSustainSlider()->getValue();
    
    for (int i=0; i <= dec_time; i++){
        myPath.lineTo (i + atk_time, getHeight() - plotAdsr(0, dec_time, max_volume, sustain_level, dec_curve_width, i));
    }

    //sustain
    int sustain_time = 150;
    myPath.lineTo(atk_time + dec_time + sustain_time, getHeight() - sustain_level);
    
    //release
    double rel_time = parent->getReleaseTimeSlider()->getValue();
    double rel_curve_width = parent->getReleaseCurveSlider()->getValue();
    
    for (int i=0; i <= rel_time; i++){
        myPath.lineTo (i + atk_time + dec_time + sustain_time, getHeight() - plotAdsr(0, rel_time, sustain_level, 0, rel_curve_width, i));
    }
    g.strokePath (myPath, PathStrokeType (1.0f));
    
    Path myPath2;
    myPath2.startNewSubPath(atk_time, 0);
    myPath2.lineTo(atk_time, getHeight());
    myPath2.startNewSubPath(dec_time+atk_time, 0);
    myPath2.lineTo(dec_time+atk_time, getHeight());
    myPath2.startNewSubPath(dec_time+atk_time+sustain_time, 0);
    myPath2.lineTo(dec_time+atk_time+sustain_time, getHeight());
    
    //PathStrokeType(1.0f).createDashedStroke(myPath, myPath, 3.0f, 10);
    
    float* dashed_lengths;
    dashed_lengths[0] = 2.0f;
    dashed_lengths[1] = 4.0f;
    
    PathStrokeType(1.0f).createDashedStroke(myPath2, myPath2, dashed_lengths, 2);
    g.strokePath (myPath2, PathStrokeType(1.0f));
}



