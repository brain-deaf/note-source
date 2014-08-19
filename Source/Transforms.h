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

enum TransformType{
    LINEAR=0,
    EXPONENTIAL=1
};

enum TransformID{
    VOLUME=33,
    NOTE=34,
    VELOCITY=35,
    PITCHWHEEL=36,
    TUNING=37
};
    
    

class Transformation{
public:
    Transformation() : tValue(-1){}
    void setTValue(int t){tValue = t;}
    int getTValue(){return tValue;}
private:
    int tValue;

};
    

class LinearGraph : public Component, public Slider::Listener
{
public:
    LinearGraph(Slider* s, Slider* e) : Component(), 
                                        startSlider(s),
                                        endSlider(e),
                                        init(true),
                                        points(nullptr),
                                        pxThreshold(10),
                                        selectedPointIndex(-1),
                                        gValue(-1){}
    ~LinearGraph(){}
    void paint(Graphics&);
    void sliderValueChanged(Slider*);
    void setGValue(int g){gValue=g;}
    int getGValue(){return gValue;}
    double getTValue(){return tValue;}
    void calculateTValue();
    void mouseDown(const MouseEvent&);
    void mouseUp(const MouseEvent&);
    void mouseDrag(const MouseEvent&);
    Array<Point<int> > getPoints(){return *points;}
private:
    Slider* startSlider;
    Slider* endSlider;
    int gValue;
    double tValue;
    bool init;
    Array<Point<int> >* points;
    int pxThreshold;
    int selectedPointIndex;
};

class LinearTransform : public Transformation, public Component
{
public:
    LinearTransform();
    ~LinearTransform(){
        SharedResourcePointer<AudioDeviceManager> dm;
        dm->removeMidiInputCallback("", (MidiInputCallback*)midiCallback.get());
        midiCallback=nullptr;
    }
    ComboBox* getSourceBox(){return sourceBox.get();}
    ComboBox* getTargetBox(){return targetBox.get();}
    LinearGraph* getGraph(){return graph.get();}
    Slider* getStartSlider(){return startSlider.get();}
    Slider* getEndSlider(){return endSlider.get();}
    void addPoint(Point<int> p){points.add(p);}
    void paint(Graphics&);
    void resized();
    void setGValue(int g){graph->setGValue(g);}
    Array<Point<int> >* getPoints(){return &points;}
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
    Array<Point<int> > points;
};

class ExponentialGraph : public Component, public Slider::Listener
{
public:
    ExponentialGraph(Slider* s, Slider* e) : Component(), 
                                        startSlider(s),
                                        endSlider(e),
                                        init(true),
                                        points(nullptr),
                                        pxThreshold(10),
                                        selectedPointIndex(-1),
                                        gValue(-1){}
    ~ExponentialGraph(){}
    void paint(Graphics&);
    void sliderValueChanged(Slider*);
    void setGValue(int g){gValue=g;}
    int getGValue(){return gValue;}
    double getTValue(){return tValue;}
    void calculateTValue();
    void mouseDown(const MouseEvent&);
    void mouseUp(const MouseEvent&);
    void mouseDrag(const MouseEvent&);
    Array<Point<int> > getPoints(){return *points;}
    Array<double> getCurves(){return curves;}
    void addCurve(double c){curves.add(c);}
private:
    Slider* startSlider;
    Slider* endSlider;
    int gValue;
    double tValue;
    bool init;
    Array<Point<int> >* points;
    Array<double> curves;
    int pxThreshold;
    int selectedPointIndex;
};

class ExponentialTransform : public Transformation, public Component
{
public:
    ExponentialTransform();
    ~ExponentialTransform(){
        SharedResourcePointer<AudioDeviceManager> dm;
        dm->removeMidiInputCallback("", (MidiInputCallback*)midiCallback.get());
        midiCallback=nullptr;
    }
    ComboBox* getSourceBox(){return sourceBox.get();}
    ComboBox* getTargetBox(){return targetBox.get();}
    ExponentialGraph* getGraph(){return graph.get();}
    Slider* getStartSlider(){return startSlider.get();}
    Slider* getEndSlider(){return endSlider.get();}
    void addPoint(Point<int> p){points.add(p);}
    void paint(Graphics&);
    void resized();
    void setGValue(int g){graph->setGValue(g);}
    Array<Point<int> >* getPoints(){return &points;}
private:
    ScopedPointer<Slider> startSlider;
    ScopedPointer<Slider> endSlider;
    ScopedPointer<ExponentialGraph> graph;
    ScopedPointer<Label> sourceLabel;
    ScopedPointer<Label> targetLabel;
    ScopedPointer<ComboBox> sourceBox;
    ScopedPointer<ComboBox> targetBox;
    ScopedPointer<MidiTransformCallback> midiCallback;
    
    StringArray combo_items;
    Array<Point<int> > points;
};

class MidiTransformCallback : public MidiInputCallback
{
public:
    MidiTransformCallback(int type, Transformation* t) 
        : midi_input_id(-1), transformType(type), parent(t){}        
    void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message);
    void setMidiChannel(int i){midi_input_id = i;}
private:
    Transformation* parent;
    int midi_input_id;
    int transformType;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiTransformCallback)
};



#endif  // TRANSFORMS_H_INCLUDED
