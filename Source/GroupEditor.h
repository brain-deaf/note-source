/*
  ==============================================================================

    GroupEditor.h
    Created: 14 Jun 2014 3:12:46pm
    Author:  patrick

  ==============================================================================
*/

#ifndef GROUPEDITOR_H_INCLUDED
#define GROUPEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class FxBin;
class TransformBin;
class InstrumentMappingEditor;
class GroupEditor;
class GroupView;

class GroupBoxModel : public ListBoxModel
{
public:
    GroupBoxModel(){num_rows = 1;}
    int getNumRows(){return num_rows;}
    void incNumRows(){num_rows++;}
    void decNumRows(){num_rows--;}
    void clearNumRows(){num_rows=0;group_names.clear();}
    void paintListBoxItem(int rowNumber, Graphics& g, int w, int h, bool rowIsSelected);
    void selectedRowsChanged(int row);
    
    void addGroupName(String s){group_names.add(s);}
    StringArray& getGroupNames(){return group_names;}
    void setListBox(ListBox* l){_list_box = l;}
    ListBox* getListBox(){return _list_box;}
    void setParent(GroupEditor* g){parent = g;}
private:
    int num_rows;
    StringArray group_names;
    ListBox* _list_box;
    GroupEditor* parent;
};

class GroupEditor : public Component, public Label::Listener, public Button::Listener
{
public:
    GroupEditor(int w, int h, InstrumentMappingEditor* m);
    ~GroupEditor();
    void paint(Graphics& g);
    void resized();
    void labelTextChanged(Label* source);
    void buttonClicked(Button* button);
    void updateLabels(String group_name, int index);
    void addGroup();
    void removeGroups();
    
    int get_width(){return width;}
    int get_height(){return height;}
    int getRowHeight(){return row_height;}
    GroupBoxModel* getModel(){return model;}
    ListBox* getListBox(){return list_box;}
    TextButton* getCreateGroupButton(){return create_group_button;}
    TextButton* getDeleteGroupButton(){return delete_group_button;}
    ArrowButton* getGroupDownButton(){return groupDownButton.get();}
    ArrowButton* getGroupUpButton(){return groupUpButton.get();}
    InstrumentMappingEditor* getMappingEditor(){return mapping_editor;}
    GroupView* getGroupView(){return group_view;}
    void setFxBin(FxBin*);
    void setTransformBin(TransformBin*);
    void setGroupView(GroupView* g){group_view=g;}
    FxBin* getFxBin(){return fx_bin;}
    TransformBin* getTransformBin(){return transform_bin;}
    
private:
    int width;
    int height;
    int row_height;
    int header_height;
    int footer_height;
    Label* group_name_lbl;
    Label* group_number_lbl;
    Label* group_name;
    Label* group_number;
    GroupBoxModel* model;
    ListBox* list_box;
    TextButton* create_group_button;
    TextButton* delete_group_button;
    ScopedPointer<ArrowButton> groupUpButton;
    ScopedPointer<ArrowButton> groupDownButton;
    InstrumentMappingEditor* mapping_editor;
    String temp;
    FxBin* fx_bin;
    TransformBin* transform_bin;
    GroupView* group_view;
};



#endif  // GROUPEDITOR_H_INCLUDED
