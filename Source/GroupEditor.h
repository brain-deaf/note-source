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
    int getNumRows(){std::cout<<"get num rows"<<std::endl;return 20;}
    void paintListBoxItem(int rowNumber, Graphics& g, int w, int h, bool rowIsSelected);
};

class GroupEditor : public Component
{
public:
    GroupEditor(int w, int h);
    ~GroupEditor(){delete model; model = nullptr; delete list_box; list_box = nullptr;}
    void paint(Graphics& g);
    void resized();
    
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getRowHeight(){return row_height;}
    GroupBoxModel* getModel(){return model;}
    
private:
    int width;
    int height;
    int row_height;
    GroupBoxModel* model;
    ListBox* list_box;
    
};



#endif  // GROUPEDITOR_H_INCLUDED
