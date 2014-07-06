/*
  ==============================================================================

    FxSelector.h
    Created: 6 Jul 2014 2:54:52pm
    Author:  patrick

  ==============================================================================
*/

#ifndef FXSELECTOR_H_INCLUDED
#define FXSELECTOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class FxButton;

class FxSelector : public Component{
public:
    FxSelector();
    ~FxSelector();
    void paint(Graphics& g);
private:
    Array<FxButton*> fx_buttons;
};

class FxButton : public TextButton{
public:
    FxButton();
};


#endif  // FXSELECTOR_H_INCLUDED
