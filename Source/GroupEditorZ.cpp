/*
  ==============================================================================

    GroupEditorZ.cpp
    Created: 26 Jul 2014 1:47:26pm
    Author:  patrick

  ==============================================================================
*/

#include "GroupEditorZ.h"

GroupViewItem::GroupViewItem(String s, bool b) : TreeViewItem(), name(s), canBeOpened(b){}

void GroupViewItem::itemOpennessChanged(bool isOpen){}

void GroupViewItem::paintItem(Graphics& g, int width, int height) {
    
    if (isSelected()) g.fillAll(Colours::lightblue);
    
    g.setColour (Colours::black);
    g.setFont (15.0f);

    g.drawText (name,
                4, 0, width - 4, height,
                Justification::centredLeft, true);
}

GroupView::GroupView(GroupEditor* g) : TreeView(), group_editor(g) 
{
    root_item = new GroupViewItem("Groups", true);
    setRootItem(root_item);
    setRootItemVisible(true);
    
    groups = group_editor->getMappingEditor()->graph->getGroups();
    
    for (auto name : group_editor->getModel()->getGroupNames()){
        root_item->addSubItem(new GroupViewItem(name, true));
    }
    g->setGroupView(this);
}

void GroupView::paint(Graphics& g){
    g.fillAll(Colours::white);
}

void GroupView::refreshRows(){
    root_item->clearSubItems();
    groups = group_editor->getMappingEditor()->graph->getGroups();
    int group_count = 0;
    for (auto name : group_editor->getModel()->getGroupNames()){
        GroupViewItem* group = new GroupViewItem(name, true);
        root_item->addSubItem(group);
        
        if (groups[group_count] != nullptr){
            for (int i=0; i<groups[group_count]->getZones()->size(); i++){ 
                InstrumentMappingEditor::MappingEditorGraph::Zone* z = (*(groups[group_count]->getZones()))[i];
                GroupViewItem* zone = new GroupViewItem(z->getName(), false);
                group->addSubItem(zone);
            }
        }
        
        group_count++;
    }
}
    


