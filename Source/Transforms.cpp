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
    targetBox->addItemList(combo_items, 1);
    
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

void LinearGraph::paint(Graphics& g){
    g.fillAll(Colours::grey);
    
    g.setColour(Colours::black);
    if (startSlider != nullptr && endSlider != nullptr){
        Path myPath;
        myPath.startNewSubPath(0, getHeight()-startSlider->getValue()*getHeight());
        myPath.lineTo(getWidth(), getHeight()-endSlider->getValue()*getHeight());    
        g.strokePath (myPath, PathStrokeType (1.0f));
        
        if (gValue != -1){
            Point<float> intersection = myPath.getPointAlongPath(myPath.getLength()/128*gValue);
            float ellipse_width = 10.0;
            g.setColour(Colours::red);
            g.drawEllipse(intersection.getX()-ellipse_width/2, intersection.getY()-ellipse_width/2, ellipse_width, ellipse_width, 2.0);
        }
    }
}

void LinearGraph::sliderValueChanged(Slider* source){
    if (source == startSlider || source == endSlider)
        repaint();
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
            }
        }
    }
}


































