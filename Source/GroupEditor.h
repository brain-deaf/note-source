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
class GroupEditor;

class GroupBoxModel : public ListBoxModel
{
public:
    GroupBoxModel(){num_rows = 1;}
    int getNumRows(){return num_rows;}
    void incNumRows(){num_rows++;}
    void decNumRows(){num_rows--;}
    void paintListBoxItem(int rowNumber, Graphics& g, int w, int h, bool rowIsSelected);
    void selectedRowsChanged(int row);
    
    void addGroupName(String s){group_names.add(s);}
    StringArray& getGroupNames(){return group_names;}
    void setListBox(ListBox* l){_list_box = l;}
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
    GroupEditor(int w, int h);
    ~GroupEditor();
    void paint(Graphics& g);
    void resized();
    void labelTextChanged(Label* source);
    void buttonClicked(Button* button);
    void updateLabels(String group_name, int index);
    
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getRowHeight(){return row_height;}
    GroupBoxModel* getModel(){return model;}
    
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
    String temp;
};



#endif  // GROUPEDITOR_H_INCLUDED
