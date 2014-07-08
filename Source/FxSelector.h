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
    void setDragObject(FxButton* b){drag_object=b;}
    Array<FxBox*>& getBoxes(){return fx_boxes;}
    FxButton* getDragObject(){return drag_object;}
    void setBoxes(Array<FxBox*> b){fx_boxes = b;}
private:
    Array<FxBox*> fx_boxes;
    int num_rows;
    int num_columns;
    FxButton* drag_object;
};

class FxButton : public TextButton{
public:
    FxButton(FxSelector*);
    void mouseDrag(const MouseEvent& e);
    void mouseDown(const MouseEvent& e);
private:
    FxSelector* parent;
};

class FxBox : public Component, public DragAndDropTarget{
public:
    FxBox(FxButton* button, FxSelector* parent);
    ~FxBox(){delete _button;}
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override {return true;}
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails);
    void itemDragExit(const SourceDetails& dragSourceDetails);
    void paint(Graphics&);
    void resized();
    void setEntered(bool b){item_entered = b;}
    FxButton* getButton(){return _button;}
private:
    FxButton* _button;
    bool item_entered;
    FxSelector* parent;
};

class FxChoiceButton : public TextButton{
public:
    FxChoiceButton() : TextButton(){}
    void mouseDown(const MouseEvent& e) override;
    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    //void paint(Graphics& g) override;
};

class FxChooser : public Component{
public:
    FxChooser(int rows, int columns);
    ~FxChooser();
    void resized();
    void paint(Graphics& g);
    void focusButton(FxChoiceButton*);
    void unfocusButton(FxChoiceButton*);
private:
    Array<FxChoiceButton*> buttons;
    int numRows;
    int numColumns;
};




#endif  // FXSELECTOR_H_INCLUDED
