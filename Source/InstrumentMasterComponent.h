/*
  ==============================================================================

    instrumentMasterComponent.h
    Created: 9 May 2014 6:24:36pm
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTMASTERCOMPONENT_H_INCLUDED
#define INSTRUMENTMASTERCOMPONENT_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"

class InstrumentMasterComponent : public Component, public Button::Listener {
public:

    class InstrumentLabel : public Label {
    public:
        InstrumentLabel(const String& componentName, 
                        const String& labelText, TabbedComponent* p)
        : Label(componentName, labelText),parent(p){}

        void textWasEdited();
    private:
        TabbedComponent* parent;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentLabel)
    };

    class InstrumentButton : public ShapeButton
    {
    public:
        InstrumentButton(const String& name, Colour normalColour, 
                Colour overColour, Colour downColour, TabbedComponent* p) 
        : ShapeButton{name, normalColour, overColour, downColour}, parent{p}{}

        void paintButton(Graphics& g, bool isMouseOver, bool isButtonDown);
    private:
        TabbedComponent* parent;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentButton)
    };

    InstrumentMasterComponent(TabbedComponent* p);
    void paint(Graphics& g);
    void resized();
    void buttonClicked(Button* button);
private:
    TabbedComponent* parent;
    InstrumentLabel instrumentLabel;
    InstrumentButton xButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentMasterComponent)
};




#endif  // INSTRUMENTMASTERCOMPONENT_H_INCLUDED
