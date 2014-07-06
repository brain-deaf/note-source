/*
  ==============================================================================

    FxBin.h
    Created: 6 Jul 2014 2:53:45pm
    Author:  patrick

  ==============================================================================
*/

#ifndef FXBIN_H_INCLUDED
#define FXBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "FxComponent.h"
#include "FxSelector.h"

class FxBin : public Component {
public:
    FxBin();
    ~FxBin();
    void resized();
private:
    FxSelector* fx_selector;
    FxComponent* fx_component;
    int selector_height;
};



#endif  // FXBIN_H_INCLUDED
