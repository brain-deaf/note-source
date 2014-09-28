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


class LevelMeterHandle : public Component
{
public:
    LevelMeterHandle(LevelMeter* l) : levelMeter(l), Component(){}
    ~LevelMeterHandle(){}
    void paint(Graphics&);
private:
    LevelMeter* levelMeter;
    float decibelValue;
};

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
                                handleYPosition(0),
                                handleValue(1.0)
    {
        levelMeterTimer = new LevelMeterTimer(this);
        handle = new LevelMeterHandle(this);
        addAndMakeVisible(handle);
    }
    ~LevelMeter(){};
    void setDecibelRange(float low, float high){decibelRangeLow=low, decibelRangeHigh=high;}
    void paint(Graphics&);
    void drawAmplitude(){repaint();}
    void mouseDrag(const MouseEvent&);
    void mouseDown(const MouseEvent&);
    void resized();
private:
    float decibelRangeLow;
    float decibelRangeHigh;
    float greenRangeHigh;
    float yellowRangeHigh;
    float orangeRangeHigh;
    ScopedPointer<LevelMeterHandle> handle;
    ScopedPointer<LevelMeterTimer> levelMeterTimer;
    int handleYPosition;
    float handleValue;
};





#endif  // LEVELMETER_H_INCLUDED
