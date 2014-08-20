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
#include "MidiSlider.h"

class MainHorizontalSlider : public MidiSlider
{
public:
    MainHorizontalSlider(String, int, double, double, double);
    ~MainHorizontalSlider(){if (image != nullptr) delete image;}
    int getId(){return id;}
    void paint(Graphics&);
    void setImage(String, int);
    Image* getImage(){return image;}
    void setNumFrames(int i){frames = i;}
    int getNumFrames(){return frames;}
private:
    int id;
    Image* image;
    int frames;
};

class MainVerticalSlider : public MidiSlider
{
public:
    MainVerticalSlider(String, int, double, double, double);
    ~MainVerticalSlider(){if (image != nullptr) delete image;}
    int getId(){return id;}
    void paint(Graphics&);
    void setImage(String, int);
    Image* getImage(){return image;}
    void setNumFrames(int i){frames = i;}
    int getNumFrames(){return frames;}
private:
    int id;
    Image* image;
    int frames;
};

class MainKnob : public MidiSlider
{
public:
    MainKnob(String, int, double, double, double);
    int getId(){return id;}
private:
    int id;
};

class MainButton : public TextButton
{
public:
    MainButton(String, int);
    int getId(){return id;}
private:
    int id;
};

class MainLabel: public Label
{
public:
    MainLabel(int, String);
    int getId(){return id;}
private:
    int id;
};

class MainComboBox : public ComboBox
{
public:
    MainComboBox(String, int);
    int getId(){return id;}
private:
    int id;
};




#endif  // MAINPAGECOMPONENTS_H_INCLUDED
