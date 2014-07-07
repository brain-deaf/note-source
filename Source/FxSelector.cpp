/*
  ==============================================================================

    FxSelector.cpp
    Created: 6 Jul 2014 2:54:41pm
    Author:  patrick

  ==============================================================================
*/

#include "FxSelector.h"

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
}

FxSelector::~FxSelector(){
    for (auto box : fx_boxes){
        delete box;
    }
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

FxButton::FxButton(FxSelector* _parent) : TextButton("fx"), parent(_parent){
    setClickingTogglesState(true);
}

FxBox::FxBox(FxButton* button, FxSelector* _parent): _button(button), parent(_parent), item_entered(false){
    addAndMakeVisible(_button);
}

void FxBox::resized(){
    _button->setSize(getWidth(), getHeight());
}

void FxButton::mouseDrag(const MouseEvent& e){
    DragAndDropContainer::findParentDragContainerFor(this)->startDragging("fx", this);
    parent->setDragObject(this);
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