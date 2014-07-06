/*
  ==============================================================================

    FxBin.cpp
    Created: 6 Jul 2014 2:53:34pm
    Author:  patrick

  ==============================================================================
*/

#include "FxBin.h"

FxBin::FxBin() : Component(), selector_height(150), fx_selector(new FxSelector()), fx_component(new FxComponent()){
    addAndMakeVisible(fx_selector);
    addAndMakeVisible(fx_component);
}

FxBin::~FxBin(){
    delete fx_selector;
    delete fx_component;
}

void FxBin::resized(){
    fx_selector->setBounds(100, 0, getWidth() - 100, selector_height);
    fx_component->setBounds(0, selector_height + 30, getWidth(), getHeight() - (selector_height + 30)); 
}