/*
  ==============================================================================

    MainPageComponents.h
    Created: 2 Aug 2014 12:48:47pm
    Author:  patrick

  ==============================================================================
*/

#ifndef MAINPAGECOMPONENTS_H_INCLUDED
#define MAINPAGECOMPONENTS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class MainHorizontalSlider : public Slider
{
public:
    MainHorizontalSlider(String, double, double, double);
    int getId(){return id;}
private:
    int id;
    String name;
};

class MainVerticalSlider : public Slider
{
public:
    MainVerticalSlider(String, double, double, double);
    int getId(){return id;}
private:
    int id;
    String name;
};

class MainKnob : public Slider
{
public:
    MainKnob(String, double, double, double);
    int getId(){return id;}
private:
    int id;
    String name;
};

class MainButton : public TextButton
{
public:
    MainButton(String);
    int getId(){return id;}
private:
    int id;
    String name;
};

class MainLabel: public Label
{
public:
    MainLabel(String, String);
    int getId(){return id;}
private:
    int id;
    String name;
};

class MainComboBox : public ComboBox
{
public:
    MainComboBox(String);
    int getId(){return id;}
private:
    int id;
    String name;
};




#endif  // MAINPAGECOMPONENTS_H_INCLUDED
