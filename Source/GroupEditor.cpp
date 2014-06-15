/*
  ==============================================================================

    GroupEditor.cpp
    Created: 14 Jun 2014 3:12:28pm
    Author:  patrick

  ==============================================================================
*/

#include "GroupEditor.h"

GroupEditor::GroupEditor(int w, int h) : Component(), width(w), height(h), row_height(30),
    header_height(50), model(new GroupBoxModel()), list_box(new ListBox("list box", nullptr)),
    group_name_lbl(new Label("")), group_number_lbl(new Label("")),
    group_name(new Label("")), group_number(new Label("")){
    setBounds(0, 0, w, h);
    
    addAndMakeVisible(list_box);
    list_box->setBounds(0, header_height, w, h-header_height);
    list_box->setModel(model);
    list_box->setMultipleSelectionEnabled(true);
    
    group_name_lbl->setBounds(2, 3, 80, 20);
    group_name_lbl->setText("Group Name ", dontSendNotification);
    group_name_lbl->setFont(Font(12.0f));
    addAndMakeVisible(group_name_lbl);
    
    group_number_lbl->setBounds(2, 26, 80, 20);
    group_number_lbl->setText("Group Index ", dontSendNotification);
    group_number_lbl->setFont(Font(12.0f));
    addAndMakeVisible(group_number_lbl);
    
    group_name->setBounds(72, 3, 125, 20);
    group_name->setText("Master Group", dontSendNotification);
    group_name->setFont(Font(12.0f));
    group_name->setEditable(false, true, true);
    group_name->addListener(this);
    addAndMakeVisible(group_name);
    
    group_number->setBounds(72, 26, 50, 20);
    group_number->setText("0", dontSendNotification);
    group_number->setFont(Font(12.0f));
    addAndMakeVisible(group_number);
    
    group_names.add(group_name->getText());
    
    model->_group_names = group_names.begin();
}

GroupEditor::~GroupEditor(){
    delete model; 
    model = nullptr; 
    delete list_box; 
    list_box = nullptr;
    delete group_name_lbl;
    group_name_lbl = nullptr;
    delete group_number_lbl;
    group_number_lbl = nullptr;
}

void GroupEditor::paint(Graphics& g){
    g.fillAll(Colours::white);
    g.setColour(Colours::lightgrey);
    g.fillRect(0, 1, width, header_height);
    g.setColour(Colours::black);
    g.setOpacity(1.0f);
    Path myPath;
    myPath.startNewSubPath(0.0f, 0.0f);
    myPath.lineTo(width, 0);
    myPath.startNewSubPath(0.0f, header_height);
    myPath.lineTo(width, header_height);

    g.strokePath(myPath, PathStrokeType(1.0f));
}

void GroupEditor::labelTextChanged(Label* source){
    if (source == group_name){
        group_names.set(group_number->getText().getIntValue(), source->getText());
        list_box->repaintRow(group_number->getText().getIntValue());
    }
}

void GroupBoxModel::paintListBoxItem(int rowNumber, Graphics& g, int w, int h, bool rowIsSelected){
    if (rowIsSelected){ 
        g.fillAll(Colours::lightblue);
    }else{
        g.fillAll(Colours::white);
    }
    g.drawText(*(_group_names + rowNumber), 5, 0, w, h, Justification::centredLeft, true);
}

void GroupEditor::resized(){
    setBounds(0, 0, width, height);
}