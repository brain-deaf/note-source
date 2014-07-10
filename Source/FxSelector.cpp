/*
  ==============================================================================

    FxSelector.cpp
    Created: 6 Jul 2014 2:54:41pm
    Author:  patrick

  ==============================================================================
*/

#include "FxSelector.h"
#include "FxBin.h"

FxSelector::FxSelector(int rows, int columns) : Component(),
    num_rows(rows), num_columns(columns)
{
    for (int i=0; i<num_columns*rows; i++){
        fx_boxes.add(new FxBox(new FxButton(this), this));
        fx_boxes[i]->getButton()->setButtonText(String(i));
    }
    for (auto box : fx_boxes){
        addAndMakeVisible(box);
    }
    chooser = new FxChooser(2, 4, this);
}

FxSelector::~FxSelector(){
    for (auto box : fx_boxes){
        delete box;
    }
    delete chooser;
}

void FxSelector::resized(){
    int box_width = getWidth() / num_columns;
    int box_height = getHeight() / num_rows;
    
    for (int row=0; row<num_rows; row++){
        for (int i=0; i<num_columns; i++){
            fx_boxes[i+row*num_columns]->setBounds(i*box_width, box_height*row, box_width, box_height);
        }
    }
}

void FxSelector::paint(Graphics& g){
    
    if (isDragAndDropActive()){
    }else{
    }
}

void FxSelector::updateButtonText(String buttonText){
    for (auto box : fx_boxes){
        if (box->getButton() == fxButtonChoice){
            box->getButton()->setButtonText(buttonText);
        }
    }
}


FxButton::FxButton(FxSelector* _parent) : TextButton("fx"), parent(_parent), fx(FxChooser::FX::NONE){
    //setClickingTogglesState(true);
}

void FxBox::resized(){
    _button->setSize(getWidth(), getHeight());
}

void FxButton::mouseDrag(const MouseEvent& e){
    DragAndDropContainer::findParentDragContainerFor(this)->startDragging("fx", this);
    parent->setDragObject(this);
}

void FxButton::mouseDown(const MouseEvent& e){
    ModifierKeys keys = e.mods;
    parent->fxButtonChoice = this;
    if (keys.isRightButtonDown()){
        parent->addAndMakeVisible(parent->getChooser());
        int x_margin = 50;
        int y_margin = 10;
        parent->getChooser()->setBounds(x_margin, y_margin, parent->getWidth() - x_margin*2, parent->getHeight() - y_margin*2);
    }else{
        typedef FxChooser::FX FX;
        FxBox* box = (FxBox*)getParentComponent();
        FxSelector* selector = box->getParent();
        FxBin* bin = (FxBin*)selector->getParentComponent();
        switch (fx){
        case FX::ADSR:{
            bin->getFxComponent()->loadFx(FX::ADSR);
            break;}
        case FX::NONE:{
            bin->getFxComponent()->loadFx(FX::NONE);
            break;}
        }
    }
}

FxBox::FxBox(FxButton* button, FxSelector* _parent): _button(button), parent(_parent), item_entered(false){
    addAndMakeVisible(_button);
}

void FxBox::itemDropped(const SourceDetails& dragSourceDetails){
    Array<FxBox*> boxes = parent->getBoxes();
    
    FxBox* dragged_box;
    int dragged_index;
    for (auto box : boxes){
        if (box->getButton() == parent->getDragObject()){
            dragged_box = box;
            dragged_index = boxes.indexOf(box);
            boxes.removeFirstMatchingValue(dragged_box);
            break;
        }
    }
    
    int new_index = boxes.indexOf(this);
    if (new_index == -1){
        boxes.insert(dragged_index, dragged_box);
    }
    else if (new_index < dragged_index){
        boxes.insert(new_index, dragged_box);
    }
    else{
        boxes.insert(new_index+1, dragged_box);
    }

    for (auto box : boxes){
        box->setEntered(false);
        box->repaint();
    }
    parent->setBoxes(boxes);
    parent->resized();
}

void FxBox::itemDragEnter(const SourceDetails& dragSourceDetails){
    item_entered = true;
    repaint();
}

void FxBox::itemDragExit(const SourceDetails& dragSourceDetails){
    item_entered = false;
    repaint();
}

void FxBox::paint(Graphics& g){
    if (DragAndDropContainer::findParentDragContainerFor(this)->isDragAndDropActive()){
        if (item_entered){
            g.fillAll(Colours::blue);
        }
        else if (!item_entered){
            g.fillAll(Colours::grey);
        }
    }else{
        g.fillAll(Colours::grey);
    }
}

void FxChoiceButton::mouseDown(const MouseEvent& e){
    ((FxSelector*)((FxChooser*)getParentComponent())->getParentComponent())->updateButtonText(getButtonText());
    getParentComponent()->setVisible(false);
    ((FxChooser*)getParentComponent())->callButtonClick(this);
}

void FxChoiceButton::mouseEnter(const MouseEvent& e){
    ((FxChooser*)(getParentComponent()))->focusButton(this);
}

void FxChoiceButton::mouseExit(const MouseEvent& e){
    ((FxChooser*)(getParentComponent()))->unfocusButton(this);
}

FxChooser::FxChooser(int rows, int columns, FxSelector * f) : Component(), numRows(rows), numColumns(columns), parent(f)
{
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            buttons.add(new FxChoiceButton());
        }
    }
        
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            addAndMakeVisible(buttons[i*numColumns + j]);
        }
    }
    buttons[0]->setButtonText("ADSR");
}

FxChooser::~FxChooser(){
    for (auto i : buttons){
        delete i;
    }
}

void FxChooser::paint(Graphics& g){
    g.setColour(Colours::black);
    g.setOpacity(0.9f);
    g.fillRoundedRectangle(0.0, 0.0, getWidth(), getHeight(), 30.0);
}

void FxChooser::resized(){
    float button_width = getWidth() / numColumns;
    float button_height = getHeight() / numRows;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            buttons[i*numColumns+j]->setBounds(button_width*j + button_width/4, button_height*i + button_height/4, button_width/2, button_height/2);
        }
    }
}

void FxChooser::callButtonClick(FxChoiceButton* b){
    FxSelector* selector = (FxSelector*)getParentComponent();
    FxBin* bin = ((FxBin*)(selector->getParentComponent()));
    if (buttons.indexOf(b) == ADSR){
        selector->fxButtonChoice->setFx(ADSR);
        bin->getFxComponent()->loadFx(ADSR);
    }else{ 
        selector->fxButtonChoice->setFx(NONE);
        bin->getFxComponent()->loadFx(NONE);
    }
}

void FxChooser::focusButton(FxChoiceButton* button){
    float button_width = getWidth() / numColumns;
    float button_height = getHeight() / numRows;
    int x_offset = 20;
    int y_offset = 7;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            if (buttons[i*numColumns+j] == button){
                button->setBounds(button_width*j+x_offset, button_height*i+y_offset, button_width-x_offset*2, button_height-y_offset*2);
            }
        }
    }
}

void FxChooser::unfocusButton(FxChoiceButton* button){
    float button_width = getWidth() / numColumns;
    float button_height = getHeight() / numRows;
    for (int i=0; i<numRows; i++){
        for (int j=0; j<numColumns; j++){
            if (buttons[i*numColumns+j] == button){
                button->setBounds(button_width*j + button_width/4, button_height*i + button_height/4, button_width/2, button_height/2);
            }
        }
    }
}