/*
  ==============================================================================

    Transforms.h
    Created: 11 Aug 2014 1:54:36pm
    Author:  patrick

  ==============================================================================
*/

#ifndef TRANSFORMS_H_INCLUDED
#define TRANSFORMS_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"

class MidiTransformCallback;
class LinearGraph;

class LinearTransform : public Component
{
public:
    LinearTransform();
    ~LinearTransform(){}
    void paint(Graphics&);
    void resized();
private:
    ScopedPointer<Slider> startSlider;
    ScopedPointer<Slider> endSlider;
    ScopedPointer<LinearGraph> graph;
    ScopedPointer<Label> sourceLabel;
    ScopedPointer<Label> targetLabel;
    ScopedPointer<ComboBox> sourceBox;
    ScopedPointer<ComboBox> targetBox;
    ScopedPointer<MidiTransformCallback> midiCallback;
    StringArray combo_items;
    int tValue;
};

class LinearGraph : public Component, public Slider::Listener
{
public:
    LinearGraph(Slider* s, Slider* e) : Component(), 
                                        startSlider(s),
                                        endSlider(e){}
    ~LinearGraph(){}
    void paint(Graphics&);
    void sliderValueChanged(Slider*);
private:
    Slider* startSlider;
    Slider* endSlider;
};

class MidiTransformCallback : public MidiInputCallback
{
public:
    MidiTransformCallback(){}        
    void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiTransformCallback)
};



#endif  // TRANSFORMS_H_INCLUDED
