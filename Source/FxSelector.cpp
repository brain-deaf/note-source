/*
  ==============================================================================

    FxSelector.cpp
    Created: 6 Jul 2014 2:54:41pm
    Author:  patrick

  ==============================================================================
*/

#include "FxSelector.h"

FxSelector::FxSelector(){
    for (int i=0; i<6; i++){
        fx_buttons.add(new FxButton());
    }
    for (auto button : fx_buttons){
        addAndMakeVisible(button);
    }
}

FxSelector::~FxSelector(){
    for (auto button : fx_buttons){
        delete button;
    }
}

void FxSelector::paint(Graphics& g){
    g.fillAll(Colours::red);
}

FxButton::FxButton() : TextButton("fx"){
    setClickingTogglesState(true);
}