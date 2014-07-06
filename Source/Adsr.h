/*
  ==============================================================================

    Adsr.h
    Created: 5 Jul 2014 2:23:19pm
    Author:  patrick

  ==============================================================================
*/

#ifndef ADSR_H_INCLUDED
#define ADSR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//use rational function for attack function of adsr:
//ex: y = (2x + 5) / (x - 1)
//vertical asymptote = 1 [since 1-1=0]
//horizontal asymptote = 2 [since 2/1 (leading coefficents of num and denom) = 2]

//-e^-x for attack
//e^-x for release

class Adsr : public Component, public Slider::Listener
{
public:
    class Graph : public Component{
    public:
        Graph(Adsr*);
        void paint(Graphics&);
    private:
        Adsr* parent;
    };

    Adsr();
    void paint(Graphics&);
    void resized();
    void sliderValueChanged(Slider*);
    Slider* getAttackTimeSlider(){return &attack_time;}
    Slider* getCurveSlider(){return &curve;}
private:
    Graph adsr_plot;
    Slider attack_time;
    Slider curve;
};



#endif  // ADSR_H_INCLUDED
