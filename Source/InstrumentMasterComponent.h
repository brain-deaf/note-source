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
class InstrumentComponent;
class InstrumentBin;

class InstrumentMasterComponent : public Component, public Button::Listener,
      public ComboBox::Listener{
public:
    enum MidiInputId {
        omni=-1,
        channel_1=1,
        channel_2=2,
        channel_3=3,
        channel_4=4,
        channel_5=5,
        channel_6=6,
        channel_7=7,
        channel_8=8,
        channel_9=9,
        channel_10=10,
        channel_11=11,
        channel_12=12,
        channel_13=13,
        channel_14=14,
        channel_15=15,
        channel_16=16
    };
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

    InstrumentMasterComponent(InstrumentBin* p, InstrumentComponent*);
    ~InstrumentMasterComponent(){delete midi_input_menu;}
    void paint(Graphics& g);
    void resized();
    void buttonClicked(Button* button);
    void comboBoxChanged(ComboBox*);
private:
    InstrumentBin* parent;
    InstrumentComponent* instrument_parent;
    InstrumentLabel instrumentLabel;
    InstrumentButton xButton;
    ComboBox* midi_input_menu;
    Label* midi_input_lbl;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentMasterComponent)
};




#endif  // INSTRUMENTMASTERCOMPONENT_H_INCLUDED
