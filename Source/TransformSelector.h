/*
  ==============================================================================

    TransformSelector.h
    Created: 11 Aug 2014 1:16:41pm
    Author:  patrick

  ==============================================================================
*/

#ifndef TRANSFORMSELECTOR_H_INCLUDED
#define TRANSFORMSELECTOR_H_INCLUDED
#include "GroupEditor.h"
#include "FilterComponent.h"
#include "Transforms.h"

class TransformButton;
class TransformBox;
class TransformChooser;
class TransformBin;
class TransformGroup;

class TransformSelector : public Component, public DragAndDropContainer, public ButtonListener{
public:
    TransformSelector(int rows, int columns);
    ~TransformSelector();
    void paint(Graphics& g);
    void resized();
    void setDragObject(TransformButton* b){drag_object=b;}
    Array<TransformBox*>& getBoxes(){return fx_boxes;}
    Array<TransformGroup*>& getGroups(){return groups;}
    TransformButton* getDragObject(){return drag_object;}
    void setBoxes(Array<TransformBox*> b){fx_boxes = b;}
    TransformChooser* getChooser(){return chooser;}
    void updateButtonText(String);
    void updateFx();
    void buttonClicked(Button*);
    void registerGroupEditor();
    void loadPatch(XmlElement*);
    GroupEditor* getGroupEditor(){return group_editor;}
    
    TransformButton* fxButtonChoice;
private:
    Array<TransformBox*> fx_boxes;
    int num_rows;
    int num_columns;
    TransformButton* drag_object;
    TransformChooser* chooser;
    Array<TransformGroup*> groups;
    GroupEditor* group_editor;
};

class Transform {
public:
    Transform() : fx_type(-1), content(nullptr){}
    Transform(int i, Component* c) : fx_type(i), content(c){}
    int& getFxType(){return fx_type;}
    Component* getContent(){return content;}
    void setContent(Component* c){content=c;}
private:
    Component* content;
    int fx_type;
};

class TransformGroup {
public:
    TransformGroup() : group_fx(){}
    ~TransformGroup(){for (auto group : group_fx){delete group;}}
    Array<Transform*> group_fx;
};

class TransformButton : public TextButton{
public:
    TransformButton(TransformSelector*);
    void mouseDrag(const MouseEvent& e);
    void mouseDown(const MouseEvent& e);
    void setFx(int i){fx=i;}
    void set_component(Component* c){if (content != nullptr) {delete content;} content=c;}
    Component* get_component(){return content;}
private:
    TransformSelector* parent;
    Component* content;
    int fx;
};

class TransformBox : public Component, public DragAndDropTarget{
public:
    TransformBox(TransformButton* button, TransformSelector* parent);
    ~TransformBox(){delete _button;}
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override {return true;}
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails);
    void itemDragExit(const SourceDetails& dragSourceDetails);
    void paint(Graphics&);
    void resized();
    void setEntered(bool b){item_entered = b;}
    TransformButton* getButton(){return _button;}
    TransformSelector* getParent(){return parent;}
private:
    TransformButton* _button;
    bool item_entered;
    TransformSelector* parent;
};

class TransformChoiceButton : public TextButton{
public:
    TransformChoiceButton() : TextButton(){}
    void mouseDown(const MouseEvent& e) override;
    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    //void paint(Graphics& g) override;
};

class TransformChooser : public Component{
public:
    enum FX
    {
        NONE = -1,
        LINEAR = 0,
        EXPONENTIAL = 1,
        SINE = 2
    };

    TransformChooser(int rows, int columns, TransformSelector* parent);
    ~TransformChooser();
    void resized();
    void paint(Graphics& g);
    void focusButton(TransformChoiceButton*);
    void unfocusButton(TransformChoiceButton*);
    void callButtonClick(TransformChoiceButton*);
private:
    Array<TransformChoiceButton*> buttons;
    int numRows;
    int numColumns;
    TransformSelector* parent;
};




#endif  // TRANSFORMSELECTOR_H_INCLUDED
