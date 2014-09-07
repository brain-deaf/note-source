/*
  ==============================================================================

    MidiSlider.cpp
    Created: 19 Aug 2014 8:13:23pm
    Author:  patrick

  ==============================================================================
*/

#include "MidiSlider.h"

MidiSlider::MidiSlider() : Slider(), cc(-1), learning(false),
                           callback(new MidiSliderCallback(this))
{
    SharedResourcePointer<AudioDeviceManager> dm;
    dm->addMidiInputCallback("", callback.get());
}

void MidiSlider::mouseDown(const MouseEvent& m){
    Slider::mouseDown(m);
    if (m.mods.isRightButtonDown()){
        PopupMenu m;
        m.addItem(1, "Midi Learn");
        m.addItem(2, "Remove learned controller");
        
        const int result = m.show();
        
        if (result == 0){
            //no choice
        }
        else if (result == 1){
            learning = true;
        }
        else if (result == 2){
            cc = -1;
        }   
    }
}

void MidiSliderCallback::handleIncomingMidiMessage(MidiInput* /*source*/, const MidiMessage& message)
{
    if (message.getChannel() == midi_input_id || midi_input_id== -1){ 
        if (message.isController() && parent->isLearning())
        {
            parent->setMidiCC(message.getControllerNumber());
            parent->setLearning(false);
        }
        
        if (message.isController() && !parent->isLearning() && 
            message.getControllerNumber() == parent->getAssignedCC())
        {
            double max = parent->getMaximum();
            double min = parent->getMinimum();
            const MessageManagerLock lock;
            parent->setValue(message.getControllerValue()*(max-min)/128+min);
            //std::cout<<"setting value: "<<message.getControllerValue()*(max-min)/128+min<<std::endl;
        }
    }
}
    