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
#include "MidiSlider.h"

class RingModulator : public Component{
public:
    RingModulator();
    void paint(Graphics&);
    void resized();
    Slider* getFrequencySlider(){return static_cast<Slider*>(frequency.get());}
    Slider* getAmplitudeSlider(){return static_cast<Slider*>(amplitude.get());}
private:
    ScopedPointer<MidiSlider> frequency;
    ScopedPointer<MidiSlider> amplitude;
};



#endif  // RINGMODULATOR_H_INCLUDED
