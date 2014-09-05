/*
  ==============================================================================

    LevelMeter.cpp
    Created: 3 Sep 2014 8:23:25pm
    Author:  patrick

  ==============================================================================
*/

#include "LevelMeter.h"
#include "Sampler.h"
#include <math.h>  

void LevelMeterHandle::paint(Graphics& g){
    g.fillAll(Colours::black);
}

void LevelMeterTimer::timerCallback(){
    levelMeter->drawAmplitude();
}

void LevelMeter::paint(Graphics& g){
    g.fillAll(Colours::white);
    
    float decibels;
    if (sampler == nullptr){
        decibels = decibelRangeLow;
    }else{
        decibels = 20*(log10(sampler->getPeak()));
    }
    //std::cout<<decibels<<std::endl;
    float px_per_decibel = (decibelRangeHigh - decibelRangeLow) / getHeight();
    
    float green_px = (greenRangeHigh - decibelRangeLow) * px_per_decibel;
    float yellow_px = (yellowRangeHigh - greenRangeHigh) * px_per_decibel;
    float orange_px = (orangeRangeHigh - yellowRangeHigh) * px_per_decibel;
    float red_px = (decibelRangeHigh - orangeRangeHigh) * px_per_decibel;
    float px = px_per_decibel*(decibels-decibelRangeLow);
    
    if (decibels > greenRangeHigh){
        g.setColour(Colours::green);
        const Rectangle<float> r(0, getHeight() - green_px, getWidth(), green_px); 
        g.fillRect(r);
    }else{
        g.setColour(Colours::green);
        const Rectangle<float> r(0, getHeight() - px, getWidth(), px); 
        g.fillRect(r);
        g.setColour(Colours::black);
        g.drawRect(0, 0, getWidth(), getHeight());
        return;
    }
    if (decibels > yellowRangeHigh){
        g.setColour(Colours::yellow);
        const Rectangle<float> r(0, getHeight() - (yellow_px+green_px), getWidth(), yellow_px); 
        g.fillRect(r);
    }else{
        g.setColour(Colours::yellow);
        float yellowpx = px - green_px;
        const Rectangle<float> r(0, getHeight() - px, getWidth(), yellowpx); 
        g.fillRect(r);
        g.setColour(Colours::black);
        g.drawRect(0, 0, getWidth(), getHeight());
        return;
    }
    if (decibels > orangeRangeHigh){
        g.setColour(Colours::orange);
        const Rectangle<float> r(0, getHeight() - (orange_px+yellow_px+green_px), getWidth(), orange_px); 
        g.fillRect(r);
    }else{
        g.setColour(Colours::orange);
        float orangepx = px - yellow_px - green_px;
        const Rectangle<float> r(0, getHeight() - px, getWidth(), orangepx); 
        g.fillRect(r);
        g.setColour(Colours::black);
        g.drawRect(0, 0, getWidth(), getHeight());
        return;
    }
    if (decibels >= decibelRangeHigh){
        g.setColour(Colours::red);
        const Rectangle<float> r(0, 0, getWidth(), getHeight() - (orange_px+yellow_px+green_px)); 
        g.fillRect(r);
    }else{
        g.setColour(Colours::red);
        float redpx = px - orange_px - yellow_px - green_px;
        const Rectangle<float> r(0, getHeight() - px, getWidth(), redpx); 
        g.fillRect(r);
        g.setColour(Colours::black);
        g.drawRect(0, 0, getWidth(), getHeight());
        return;
    }
}

void LevelMeter::mouseDrag(const MouseEvent& m){
    int y = m.getPosition().getY();
    handle->setBounds(0, y, getWidth(), 3);
    float handleRatio = handle->getPosition().getY() / getHeight();
    float px_per_decibel = (1.5 + 0.0) / getHeight();
    handleValue = px_per_decibel * (getHeight() - handle->getPosition().getY()) + 0.0;
    handleValue = handleValue <= 1.5 ? handleValue : 1.5;
    handleValue = handleValue > 0 ? handleValue : 0.0;
    sampler->setInstrumentVolume(handleValue);
}

void LevelMeter::mouseDown(const MouseEvent& m){
    int y = m.getPosition().getY();
    handle->setBounds(0, y, getWidth(), 3);
    float handleRatio = handle->getPosition().getY() / getHeight();
    float px_per_decibel = (1.5 + 0.0) / getHeight();
    handleValue = px_per_decibel * (getHeight() - handle->getPosition().getY()) + 0.0;
    handleValue = handleValue <= 1.5 ? handleValue : 1.5;
    handleValue = handleValue > 0 ? handleValue : 0.0;
    sampler->setInstrumentVolume(handleValue);
}

void LevelMeter::resized(){
    handle->setBounds(0, 20, getWidth(), 3);
    float handleRatio = handle->getPosition().getY() / getHeight();
    float px_per_decibel = (1.5 + 0.0) / getHeight();
    handleValue = px_per_decibel * (getHeight() - handle->getPosition().getY()) + 0.0;
}
    