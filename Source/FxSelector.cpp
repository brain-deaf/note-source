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
        fx_boxes.add(new FxBox(new FxButton()));
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
    //g.fillAll(Colours::red);
    
    if (isDragAndDropActive()){
        //g.fillAll(Colours::red);
        Point<int> mouse_position(getMouseXYRelative());
        int focused_row = (int)(mouse_position.getY() / (getHeight() / num_rows));
        int focused_column = (int)(mouse_position.getX() / (getWidth() / num_columns));
        int column_width = getWidth()/num_columns;
        int row_height = getHeight()/num_rows;
        //std::cout<<column_width<<" "<<row_height<<" "<<focused_row<<" "<<focused_column<<std::endl;
        
        //g.setColour(Colours::purple);
        //g.setOpacity(0.5f);
        //g.fillAll(Colours::purple);
        //g.fillRect(Rectangle<int>(focused_column*column_width, focused_row*row_height, column_width, row_height));
    }else{
        //std::cout<<"not dragging"<<std::endl;
    }
}

FxButton::FxButton() : TextButton("fx"){
    setClickingTogglesState(true);
}

FxBox::FxBox(FxButton* button): _button(button), item_entered(false){
    addAndMakeVisible(_button);
}

void FxBox::resized(){
    _button->setSize(getWidth(), getHeight());
}

void FxButton::mouseDrag(const MouseEvent& e){
    DragAndDropContainer::findParentDragContainerFor(this)->startDragging("fx", this);
}

void FxBox::itemDropped(const SourceDetails& dragSourceDetails){}

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