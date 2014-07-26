/*
  ==============================================================================

    GroupEditorZ.h
    Created: 26 Jul 2014 1:47:39pm
    Author:  patrick

  ==============================================================================
*/

#ifndef GROUPEDITORZ_H_INCLUDED
#define GROUPEDITORZ_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "GroupEditor.h"
#include "InstrumentMappingEditor.h"

class GroupViewItem : public TreeViewItem
{
public:
    GroupViewItem(String, bool);
    void itemOpennessChanged(bool);
    void paintItem(Graphics&, int, int) override;
    String getUniqueName(){return name;}
    bool mightContainSubItems(){return canBeOpened;}
private:
    String name;
    bool canBeOpened;
};

class GroupView : public TreeView
{
public:
    GroupView(GroupEditor* g);
    void refreshRows();
    void paint(Graphics&);
private:
    GroupViewItem* root_item;
    GroupEditor* group_editor;
    TreeView tree_view;
    Array<InstrumentMappingEditor::MappingEditorGraph::Group*> groups;
};




#endif  // GROUPEDITORZ_H_INCLUDED