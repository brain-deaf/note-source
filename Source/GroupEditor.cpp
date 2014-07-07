/*
  ==============================================================================

    GroupEditor.cpp
    Created: 14 Jun 2014 3:12:28pm
    Author:  patrick

  ==============================================================================
*/

#include "GroupEditor.h"
#include "InstrumentMappingEditor.h"

GroupEditor::GroupEditor(int w, int h, InstrumentMappingEditor* m) : Component(), width(w), height(h), row_height(30),
    mapping_editor(m),
    header_height(50), footer_height(25), model(new GroupBoxModel()), list_box(new ListBox("list box", nullptr)),
    group_name_lbl(new Label("")), group_number_lbl(new Label("")),
    group_name(new Label("")), group_number(new Label("")), temp("123lkj"),
    create_group_button(new TextButton("Create Group")), delete_group_button(new TextButton("Delete Group")){
    setBounds(0, 0, w, h);
    
    addAndMakeVisible(list_box);
    list_box->setBounds(0, header_height, w, h-header_height - footer_height);
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
    group_name->setText("New Group", dontSendNotification);
    group_name->setFont(Font(12.0f));
    group_name->setEditable(false, true, true);
    group_name->addListener(this);
    addAndMakeVisible(group_name);
    
    group_number->setBounds(72, 26, 50, 20);
    group_number->setText("0", dontSendNotification);
    group_number->setFont(Font(12.0f));
    addAndMakeVisible(group_number);
    
    model->addGroupName(group_name->getText());
    model->setListBox(list_box);
    model->setParent(this);
    
    addAndMakeVisible(create_group_button);
    create_group_button->addListener(this);
    create_group_button->setBounds(3, height - footer_height + 3, 80, 20);
    addAndMakeVisible(delete_group_button);
    delete_group_button->addListener(this);
    delete_group_button->setBounds(86, height - footer_height + 3, 80, 20);
    
    list_box->selectRow(0);
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
    delete create_group_button;
    create_group_button = nullptr;
    delete delete_group_button;
    delete_group_button = nullptr;
}

void GroupEditor::paint(Graphics& g){
    g.fillAll(Colours::white);
    g.setColour(Colours::lightgrey);
    g.fillRect(0, 1, getWidth(), header_height);
    g.fillRect(0, getHeight() - footer_height, getWidth(), getHeight() - footer_height);
    g.setColour(Colours::black);
    g.setOpacity(1.0f);
    Path myPath;
    myPath.startNewSubPath(0.0f, 0.0f);
    myPath.lineTo(getWidth(), 0);
    myPath.startNewSubPath(0.0f, header_height);
    myPath.lineTo(getWidth(), header_height);
    //myPath.startNewSubPath(getWidth(), header_height);
    //myPath.lineTo(getWidth(), getHeight());
    myPath.startNewSubPath(0.0f, getHeight() - footer_height);
    myPath.lineTo(getWidth(), getHeight() - footer_height);

    g.strokePath(myPath, PathStrokeType(1.0f));
}

void GroupEditor::labelTextChanged(Label* source){
    if (source == group_name){
        model->getGroupNames().set(group_number->getText().getIntValue(), source->getText());
        list_box->repaintRow(group_number->getText().getIntValue());
    }
}

void GroupEditor::updateLabels(String name, int index){
    group_name->setText(name, dontSendNotification);
    group_number->setText(String(index), dontSendNotification);
}

void GroupEditor::buttonClicked(Button* source){
    if (source == create_group_button){
        model->incNumRows();
        list_box->updateContent();
        list_box->repaintRow(model->getNumRows());
        model->addGroupName("New Group");
    }
    else if (source == delete_group_button){
        SparseSet<int> s = list_box->getSelectedRows();
        for (int i=0; i<s.size(); i++){
            model->getGroupNames().set(s[i], temp);
        }
        for (int i=s.size()-1; i>=0; i--){
            model->getGroupNames().remove(s[i]);
            model->decNumRows();
        }
        list_box->updateContent();
        for (int i=0; i<model->getNumRows(); i++){
            list_box->repaintRow(i);
        }
    }
}

void GroupEditor::addGroup(){
    buttonClicked(create_group_button);
}

void GroupEditor::removeGroups(){
    model->clearNumRows();
    model->getGroupNames().clear();
    list_box->updateContent();
}

void GroupBoxModel::paintListBoxItem(int rowNumber, Graphics& g, int w, int h, bool rowIsSelected){
    if (rowIsSelected){ 
        g.fillAll(Colours::lightblue);
    }else{
        g.fillAll(Colours::white);
    }
    g.drawText(group_names[rowNumber], 5, 0, w, h, Justification::centredLeft, true);
}

void GroupBoxModel::selectedRowsChanged(int row){
    parent->updateLabels(group_names[row], row);
    (parent->getMappingEditor())->graph->updateZones();
}

void GroupEditor::resized(){
    create_group_button->setBounds(3, getHeight() - footer_height + 3, 80, 20);
    delete_group_button->setBounds(86, getHeight() - footer_height + 3, 80, 20);
}