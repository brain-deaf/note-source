/*
  ==============================================================================

    RingModulator.h
    Created: 17 Aug 2014 10:24:27pm
    Author:  patrick

  ==============================================================================
*/

#ifndef RINGMODULATOR_H_INCLUDED
#define RINGMODULATOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class RingModulator : public Component{
public:
    RingModulator();
    void paint(Graphics&);
    void resized();
    Slider* getFrequencySlider(){return frequency.get();}
    Slider* getAmplitudeSlider(){return amplitude.get();}
private:
    ScopedPointer<Slider> frequency;
    ScopedPointer<Slider> amplitude;
};



#endif  // RINGMODULATOR_H_INCLUDED
