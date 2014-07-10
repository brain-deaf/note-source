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
class FxChooser;
class FxBin;

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
    FxChooser* getChooser(){return chooser;}
    void updateButtonText(String);
    
    FxButton* fxButtonChoice;
private:
    Array<FxBox*> fx_boxes;
    int num_rows;
    int num_columns;
    FxButton* drag_object;
    FxChooser* chooser;
};

class FxButton : public TextButton{
public:
    FxButton(FxSelector*);
    void mouseDrag(const MouseEvent& e);
    void mouseDown(const MouseEvent& e);
    void setFx(int i){fx=i;}
private:
    FxSelector* parent;
    int fx;
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
    FxSelector* getParent(){return parent;}
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
    enum FX
    {
        NONE = -1,
        ADSR = 0
    };

    FxChooser(int rows, int columns, FxSelector* parent);
    ~FxChooser();
    void resized();
    void paint(Graphics& g);
    void focusButton(FxChoiceButton*);
    void unfocusButton(FxChoiceButton*);
    void callButtonClick(FxChoiceButton*);
private:
    Array<FxChoiceButton*> buttons;
    int numRows;
    int numColumns;
    FxSelector* parent;
};




#endif  // FXSELECTOR_H_INCLUDED
