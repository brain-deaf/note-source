/*
  ==============================================================================

    Transforms.cpp
    Created: 11 Aug 2014 1:54:28pm
    Author:  patrick

  ==============================================================================
*/

#include "Transforms.h"

LinearTransform::LinearTransform() : Component(), 
                                     startSlider(new Slider()), 
                                     endSlider(new Slider()),
                                     graph(new LinearGraph(startSlider.get(), endSlider.get())),
                                     sourceLabel(new Label("", "Source")),
                                     targetLabel(new Label("", "Target")),
                                     sourceBox(new ComboBox()),
                                     targetBox(new ComboBox()),
                                     combo_items(),
                                     points(),
                                     midiCallback(new MidiTransformCallback(TransformType::LINEAR, static_cast<Transformation*>(this)))
{
    startSlider->setSliderStyle(Slider::RotaryVerticalDrag);
    startSlider->setTextBoxStyle(Slider::NoTextBox, false, 50, 20);
    startSlider->setRange(0, 1);
    endSlider->setSliderStyle(Slider::RotaryVerticalDrag);
    endSlider->setTextBoxStyle(Slider::NoTextBox, false, 50, 20);
    endSlider->setRange(0, 1);
    endSlider->setValue(1.0, dontSendNotification);
    
    startSlider->addListener(graph);
    endSlider->addListener(graph);
    
    for (int i=0; i<32; i++){
        combo_items.add(String("CC# ") + String(i+1));
    }
    combo_items.add("Volume");
    combo_items.add("Note");
    combo_items.add("Velocity");
    combo_items.add("Pitch Wheel");
    combo_items.add("Tuning");
    
    sourceBox->addItemList(combo_items, 1);

    targetBox->addItem("Volume", TransformID::VOLUME);
    targetBox->addItem("Velocity", TransformID::VELOCITY);
    targetBox->addItem("Pitch Wheel", TransformID::PITCHWHEEL);
    
    addAndMakeVisible(startSlider);
    addAndMakeVisible(endSlider);
    addAndMakeVisible(graph);
    addAndMakeVisible(sourceLabel);
    addAndMakeVisible(targetLabel);
    addAndMakeVisible(sourceBox);
    addAndMakeVisible(targetBox);
    
    SharedResourcePointer<AudioDeviceManager> dm;
    dm->addMidiInputCallback("", midiCallback.get());
}

void LinearTransform::resized(){
    
    
    
    sourceLabel->setBounds(80, 0, 50, 20);
    targetLabel->setBounds(getWidth()-130, 0, 50, 20);
    sourceBox->setBounds(40, 30, 130, 30);
    targetBox->setBounds(getWidth() - 170, 30, 130, 30);
    graph->setBounds(0, 80, getWidth(), getHeight()/5*4 - 100);
    startSlider->setBounds(0, getHeight()/5*4, 100, 100);
    endSlider->setBounds(getWidth() - 100, getHeight()/5*4, 100, 100);
}

void LinearTransform::paint(Graphics& g){
    g.fillAll(Colours::green);
}

void LinearGraph::calculateTValue(){
    Path myPath;
    Array<Point<int> > tPoints = *points;
    myPath.startNewSubPath(tPoints[0].getX(), tPoints[0].getY());
    for (int i=1; i<tPoints.size(); i++){
        myPath.lineTo(tPoints[i].getX(), tPoints[i].getY());
    }
        
    if (gValue != -1){
        Point<float> intersection = myPath.getPointAlongPath(myPath.getLength()/128*gValue);
        tValue = (intersection.getY()) / getHeight();
        if (tValue < 0)
            tValue = 0.0;
        tValue = 1.0 - tValue;
        std::cout<<tValue<<std::endl;
    }
}
    

void LinearGraph::paint(Graphics& g){
    if (init){
        init = false;
        points = (static_cast<LinearTransform*>(getParentComponent()))->getPoints();
        points->add(Point<int>(0, getHeight()));
        points->add(Point<int>(getWidth(), 0));
    }
    
    g.fillAll(Colours::grey);
    g.setColour(Colours::black);
    
    for (int i=0; i<points->size(); i++){
        Array<Point<int> >tPoints = *points;
        float ellipse_width = 10.0;
        if (i == selectedPointIndex)
            g.setColour(Colours::yellow);
        g.drawEllipse(tPoints[i].getX()-ellipse_width/2, tPoints[i].getY()-ellipse_width/2, ellipse_width, ellipse_width, 2.0);
        g.setColour(Colours::black);
    }
   
    Path myPath;
    Array<Point<int> >tPoints = *points;
    myPath.startNewSubPath(tPoints[0].getX(), tPoints[0].getY());
    for (int i=1; i<points->size(); i++){
        myPath.lineTo(tPoints[i].getX(), tPoints[i].getY());
    }
    g.strokePath (myPath, PathStrokeType (1.0f));
    
    if (gValue != -1){
        Point<float> intersection = myPath.getPointAlongPath(myPath.getLength()/128*gValue);
        float ellipse_width = 10.0;
        g.setColour(Colours::red);
        g.drawEllipse(intersection.getX()-ellipse_width/2, intersection.getY()-ellipse_width/2, ellipse_width, ellipse_width, 2.0);
    }
    
}

void LinearGraph::mouseDown(const MouseEvent& m){
    Array<Point<int> > tPoints = *points;
    if (m.mods.isRightButtonDown()){
        int x = m.getMouseDownX();
        for (int i=1; i<points->size(); i++){
            if (x < tPoints[i].getX() && x > tPoints[i-1].getX()){
                points->insert(i, m.getMouseDownPosition());
                break;
            }
        }
        repaint();
        return;
    }
    
    for (int i=0; i<tPoints.size(); i++){
        if (tPoints[i].getDistanceFrom(m.getMouseDownPosition()) <= pxThreshold){
            selectedPointIndex = i;
            if (selectedPointIndex == 0 || selectedPointIndex == tPoints.size() - 1)
                selectedPointIndex = -1;
            repaint();
        }
    }   
}

void LinearGraph::mouseUp(const MouseEvent& m){
    selectedPointIndex = -1;
    repaint();
}

void LinearGraph::mouseDrag(const MouseEvent& m){
    if (selectedPointIndex != -1){
        if (m.getPosition().getX() > 0 && m.getPosition().getY() > 0 
            && m.getPosition().getX() < getWidth() && m.getPosition().getY() < getHeight()){
            points->set(selectedPointIndex, m.getPosition());
            repaint();
        }
    }
}
        
void LinearGraph::sliderValueChanged(Slider* source){
    if (source == startSlider){
        points->set(0, Point<int>(0, getHeight() - startSlider->getValue()*getHeight()));
        repaint();
    }
    if (source == endSlider){
        points->set(points->size()-1, Point<int>(getWidth(), getHeight() - endSlider->getValue()*getHeight()));
        repaint();
    }
}

void MidiTransformCallback::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message)
{
    if (message.getChannel() == midi_input_id || midi_input_id== -1){ 
        switch (transformType){
        case TransformType::LINEAR :
            LinearTransform* t = static_cast<LinearTransform*>(parent);
            if (message.isController() && message.getControllerNumber() 
                == t->getSourceBox()->getSelectedId())
            {
                parent->setTValue(message.getControllerValue());
                t->setGValue(message.getControllerValue());
                const MessageManagerLock lock; //make component calls thread safe
                t->getGraph()->repaint();
                t->getGraph()->calculateTValue();
            }
            if (message.isNoteOn() && t->getSourceBox()->getSelectedId() == TransformID::VELOCITY)
            {
                parent->setTValue(message.getVelocity());
                t->setGValue(message.getVelocity());
                const MessageManagerLock lock; //make component calls thread safe
                t->getGraph()->repaint();
                t->getGraph()->calculateTValue();
            }
        }
    }
}


































