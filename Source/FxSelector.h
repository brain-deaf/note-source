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
#include "GroupEditor.h"
#include "FilterComponent.h"
#include "RingModulator.h"

class FxButton;
class FxBox;
class FxChooser;
class FxBin;
class FxGroup;

class FxSelector : public Component, public DragAndDropContainer, public ButtonListener{
public:
    FxSelector(int rows, int columns);
    ~FxSelector();
    void paint(Graphics& g);
    void resized();
    void setDragObject(FxButton* b){drag_object=b;}
    Array<FxBox*>& getBoxes(){return fx_boxes;}
    Array<FxGroup*>& getGroups(){return groups;}
    FxButton* getDragObject(){return drag_object;}
    void setBoxes(Array<FxBox*> b){fx_boxes = b;}
    FxChooser* getChooser(){return chooser;}
    void updateButtonText(String);
    void updateFx();
    void buttonClicked(Button*);
    void registerGroupEditor();
    void loadPatch(XmlElement*);
    GroupEditor* getGroupEditor(){return group_editor;}
    
    FxButton* fxButtonChoice;
private:
    Array<FxBox*> fx_boxes;
    int num_rows;
    int num_columns;
    FxButton* drag_object;
    FxChooser* chooser;
    Array<FxGroup*> groups;
    GroupEditor* group_editor;
};

class Fx {
public:
    Fx() : fx_type(-1), content(nullptr){}
    Fx(int i, Component* c) : fx_type(i), content(c){}
    int& getFxType(){return fx_type;}
    Component* getContent(){return content;}
    void setContent(Component* c){content=c;}
private:
    Component* content;
    int fx_type;
};

class FxGroup {
public:
    FxGroup() : group_fx(){}
    ~FxGroup(){for (auto group : group_fx){delete group;}}
    Array<Fx*> group_fx;
};

class FxButton : public TextButton{
public:
    FxButton(FxSelector*);
    void mouseDrag(const MouseEvent& e);
    void mouseDown(const MouseEvent& e);
    void setFx(int i){fx=i;}
    void set_component(Component* c){if (content != nullptr) {delete content;} content=c;}
    Component* get_component(){return content;}
private:
    FxSelector* parent;
    Component* content;
    int fx;
};

class FxBox : public Component, public DragAndDropTarget{
public:
    FxBox(FxButton* button, FxSelector* parent);
    ~FxBox(){delete _button;}
    bool isInterestedInDragSource(const SourceDetails& ) override {return true;}
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
        ADSR = 0,
        FILTER = 1,
        RINGMOD = 2
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
