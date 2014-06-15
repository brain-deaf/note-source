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

class GroupBoxModel : public ListBoxModel
{
public:
    int getNumRows(){return 1;}
    void paintListBoxItem(int rowNumber, Graphics& g, int w, int h, bool rowIsSelected);
    String* _group_names;
private:
    
};

class GroupEditor : public Component, public Label::Listener
{
public:
    GroupEditor(int w, int h);
    ~GroupEditor();
    void paint(Graphics& g);
    void resized();
    void labelTextChanged(Label* source);
    
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getRowHeight(){return row_height;}
    GroupBoxModel* getModel(){return model;}
    
private:
    int width;
    int height;
    int row_height;
    int header_height;
    Label* group_name_lbl;
    Label* group_number_lbl;
    Label* group_name;
    Label* group_number;
    GroupBoxModel* model;
    ListBox* list_box;
    StringArray group_names;
};



#endif  // GROUPEDITOR_H_INCLUDED
