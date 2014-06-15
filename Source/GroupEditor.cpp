/*
  ==============================================================================

    GroupEditor.cpp
    Created: 14 Jun 2014 3:12:28pm
    Author:  patrick

  ==============================================================================
*/

#include "GroupEditor.h"

GroupEditor::GroupEditor(int w, int h) : Component(), width(w), height(h), row_height(30),
    model(new GroupBoxModel()), list_box(new ListBox("list box", nullptr)){
    setBounds(0, 0, w, h);
    
    addAndMakeVisible(list_box);
    list_box->setBounds(0, 0, w, h);
    list_box->setModel(model);

}

void GroupEditor::paint(Graphics& g){
    
    g.setColour(Colours::black);
    g.setOpacity(1.0f);
    Path myPath;
    myPath.startNewSubPath(0.0f, 0.0f);
    myPath.lineTo(width, 0);

    g.strokePath(myPath, PathStrokeType(1.0f));
}

void GroupBoxModel::paintListBoxItem(int rowNumber, Graphics& g, int w, int h, bool rowIsSelected){
    if (rowIsSelected){ 
        g.fillAll(Colours::lightblue);
    }else{
        g.fillAll(Colours::white);
    }
    g.drawText("cool row", 5, 0, w, h, Justification::centredLeft, true);
}

void GroupEditor::resized(){
    setBounds(0, 0, width, height);
}