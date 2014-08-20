/*
  ==============================================================================

    MidiSlider.h
    Created: 19 Aug 2014 8:13:31pm
    Author:  patrick

  ==============================================================================
*/

#ifndef MIDISLIDER_H_INCLUDED
#define MIDISLIDER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class MidiSlider;

class MidiSliderCallback : public MidiInputCallback
{
public:
    MidiSliderCallback(MidiSlider* slider) 
        : midi_input_id(-1), parent(slider){}        
    void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message);
    void setMidiChannel(int i){midi_input_id = i;}
private:
    MidiSlider* parent;
    int midi_input_id;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSliderCallback)
};


class MidiSlider : public Slider
{
public:
    MidiSlider();
    ~MidiSlider(){};
    void setMidiCC(int c){cc=c;}
    int getAssignedCC(){return cc;}
    void mouseDown(const MouseEvent&);
    bool setLearning(bool b){learning = b;}
    bool isLearning(){return learning;}
private:
    int cc;
    bool learning;
    ScopedPointer<MidiSliderCallback> callback;
};



#endif  // MIDISLIDER_H_INCLUDED
