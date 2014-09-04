/*
  ==============================================================================

    LevelMeter.h
    Created: 3 Sep 2014 8:23:36pm
    Author:  patrick

  ==============================================================================
*/

#ifndef LEVELMETER_H_INCLUDED
#define LEVELMETER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
class Sampler;
class LevelMeter;

class LevelMeterTimer : public Timer
{
public:
    LevelMeterTimer(LevelMeter* l) : Timer(), levelMeter(l)
    {
        startTimer(25);
    }
    ~LevelMeterTimer(){stopTimer();}
    void timerCallback();
private:
    LevelMeter* levelMeter;
};

class LevelMeter : public Component
{
public:
    LevelMeter() : Component(), decibelRangeLow(-50.0), 
                                decibelRangeHigh(6.0), 
                                greenRangeHigh(-16.0),
                                yellowRangeHigh(-5.0),
                                orangeRangeHigh(0.0),
                                sampler(nullptr)
    {
        levelMeterTimer = new LevelMeterTimer(this);
    }
    ~LevelMeter(){};
    void setDecibelRange(float low, float high){decibelRangeLow=low, decibelRangeHigh=high;}
    void paint(Graphics&);
    void drawAmplitude(){repaint();}
    void setSampler(Sampler* s){sampler=s;}
    Sampler* getSampler(){return sampler;}
private:
    float decibelRangeLow;
    float decibelRangeHigh;
    float greenRangeHigh;
    float yellowRangeHigh;
    float orangeRangeHigh;
    Sampler* sampler;
    ScopedPointer<LevelMeterTimer> levelMeterTimer;
};





#endif  // LEVELMETER_H_INCLUDED
