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
class FxBox;

class FxSelector : public Component, public DragAndDropContainer{
public:
    FxSelector(int rows, int columns);
    ~FxSelector();
    void paint(Graphics& g);
    void resized();
private:
    Array<FxBox*> fx_boxes;
    int num_rows;
    int num_columns;
};

class FxButton : public TextButton{
public:
    FxButton();
    void mouseDrag(const MouseEvent& e);
};

class FxBox : public Component, public DragAndDropTarget{
public:
    FxBox(FxButton* button);
    ~FxBox(){delete _button;}
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override {return true;}
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails);
    void itemDragExit(const SourceDetails& dragSourceDetails);
    void paint(Graphics&);
    void resized();
private:
    FxButton* _button;
    bool item_entered;
};




#endif  // FXSELECTOR_H_INCLUDED
