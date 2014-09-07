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
#include "Metronome.h"

class TransformBin;

class MidiTransformCallback;

enum TransformType{
    LINEAR=0,
    EXPONENTIAL=1,
    SINE=2
};

enum TransformID{
    VOLUME=33,
    NOTE=34,
    VELOCITY=35,
    PITCHWHEEL=36,
    TUNING=37,
    TIMER=38
};


class Transformation{
public:
    Transformation() : tValue(-1){}
    void setTValue(double t){tValue = t;}
    double getTValue(){return tValue;}
private:
    double tValue;

};


class LFO;
class LFOTimer : public Timer
{
public:
    LFOTimer(LFO* parent) : lfo(parent){}
    ~LFOTimer(){stopTimer();}
    void timerCallback();
private:
    LFO* lfo;
};


class LFO
{
public:
    LFO(TransformType t, Transformation* tf) : 
        elapsedSamples(0),quitting(false),
        sampleCycleLength(44100),timer(this),
        syncToTempo(true), sampleCount(0.0)
    {
        transformType=t; parent=tf;
        timer.startTimer(10);
    }
    ~LFO(){}
    void elapseTime();
    void quit(){quitting=true;}
    void setMetronome(Metronome* m){metronome=m;}
private:
    int elapsedSamples;
    int sampleCycleLength;
    int transformType;
    Transformation* parent;
    bool quitting;
    bool syncToTempo;
    double sampleCount;
    LFOTimer timer;
    Metronome* metronome;
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
                                        gValue(-1)
    {
        
    }
    ~LinearGraph(){}
    void paint(Graphics&);
    void sliderValueChanged(Slider*);
    void setGValue(double g){gValue=g;}
    double getGValue(){return gValue;}
    double getTValue(){return tValue;}
    void calculateTValue();
    void mouseDown(const MouseEvent&);
    void mouseUp(const MouseEvent&);
    void mouseDrag(const MouseEvent&);
    void setPoints(Array<Point<int> >* a){points = a;}
    Array<Point<int> > getPoints(){return *points;}
private:
    Slider* startSlider;
    Slider* endSlider;
    double gValue;
    double tValue;
    bool init;
    Array<Point<int> >* points;
    int pxThreshold;
    int selectedPointIndex;
};

class LinearTransform : public Transformation, public Component, public ComboBox::Listener
{
public:
    LinearTransform(TransformBin* t);
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
    void comboBoxChanged(ComboBox*) override;
    LFO* getLFO(){return lfo.get();}
    Array<Point<int> >* getPoints(){return &points;}
    void quit(){lfo->quit();}
private:
    ScopedPointer<Slider> startSlider;
    ScopedPointer<Slider> endSlider;
    ScopedPointer<LinearGraph> graph;
    ScopedPointer<Label> sourceLabel;
    ScopedPointer<Label> targetLabel;
    ScopedPointer<ComboBox> sourceBox;
    ScopedPointer<ComboBox> targetBox;
    ScopedPointer<LFO> lfo;
    ScopedPointer<MidiTransformCallback> midiCallback;
    TransformBin* tf_bin;
    Metronome* metronome;
    
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
    void setPoints(Array<Point<int> >* a){points = a;}
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

class ExponentialTransform : public Transformation, public Component, public ComboBox::Listener
{
public:
    ExponentialTransform(TransformBin*);
    ~ExponentialTransform(){
        SharedResourcePointer<AudioDeviceManager> dm;
        dm->removeMidiInputCallback("", (MidiInputCallback*)midiCallback.get());
        midiCallback=nullptr;
    }
    ComboBox* getSourceBox(){return sourceBox.get();}
    ComboBox* getTargetBox(){return targetBox.get();}
    void comboBoxChanged(ComboBox*) override;
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
    ScopedPointer<LFO> lfo;
    ScopedPointer<MidiTransformCallback> midiCallback;
    Metronome* metronome;
    TransformBin* tf_bin;
    
    StringArray combo_items;
    Array<Point<int> > points;
};

class SineGraph : public Component, public Slider::Listener
{
public:
    SineGraph(Slider* s, Slider* e) : Component(), 
                                        frequencySlider(s),
                                        amplitudeSlider(e),
                                        init(true),
                                        points(nullptr),
                                        pxThreshold(10),
                                        selectedPointIndex(-1),
                                        gValue(-1)
    {
        
    }
    ~SineGraph(){}
    void paint(Graphics&);
    void sliderValueChanged(Slider*);
    void setGValue(double g){gValue=g;}
    double getGValue(){return gValue;}
    double getTValue(){return tValue;}
    void calculateTValue();
    void mouseDown(const MouseEvent&);
    void mouseUp(const MouseEvent&);
    void mouseDrag(const MouseEvent&);
    void setPoints(Array<Point<int> >* a){points = a;}
    Array<Point<int> > getPoints(){return *points;}
private:
    Slider* frequencySlider;
    Slider* amplitudeSlider;
    double gValue;
    double tValue;
    bool init;
    Array<Point<int> >* points;
    int pxThreshold;
    int selectedPointIndex;
};

class SineTransform : public Transformation, public Component, public ComboBox::Listener
{
public:
    SineTransform(TransformBin* t);
    ~SineTransform(){
        SharedResourcePointer<AudioDeviceManager> dm;
        dm->removeMidiInputCallback("", (MidiInputCallback*)midiCallback.get());
        midiCallback=nullptr;
        
    }
    ComboBox* getSourceBox(){return sourceBox.get();}
    ComboBox* getTargetBox(){return targetBox.get();}
    SineGraph* getGraph(){return graph.get();}
    Slider* getFrequencySlider(){return frequencySlider.get();}
    Slider* getAmplitudeSlider(){return amplitudeSlider.get();}
    void addPoint(Point<int> p){points.add(p);}
    void paint(Graphics&);
    void resized();
    void setGValue(int g){graph->setGValue(g);}
    void comboBoxChanged(ComboBox*) override;
    LFO* getLFO(){return lfo.get();}
    Array<Point<int> >* getPoints(){return &points;}
    void quit(){lfo->quit();}
private:
    ScopedPointer<Slider> frequencySlider;
    ScopedPointer<Slider> amplitudeSlider;
    ScopedPointer<SineGraph> graph;
    ScopedPointer<Label> sourceLabel;
    ScopedPointer<Label> targetLabel;
    ScopedPointer<ComboBox> sourceBox;
    ScopedPointer<ComboBox> targetBox;
    ScopedPointer<LFO> lfo;
    ScopedPointer<MidiTransformCallback> midiCallback;
    TransformBin* tf_bin;
    Metronome* metronome;
    
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
