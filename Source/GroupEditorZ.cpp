/*
  ==============================================================================

    GroupEditorZ.cpp
    Created: 26 Jul 2014 1:47:26pm
    Author:  patrick

  ==============================================================================
*/

#include "GroupEditorZ.h"
#include "WaveformView.h"
#include "WaveBin.h"

GroupViewItem::GroupViewItem(String s, bool b, InstrumentMappingEditor::MappingEditorGraph::Zone* zone, GroupView* p) : 
    TreeViewItem(), name(s), canBeOpened(b), z(zone), parent(p){}

void GroupViewItem::itemOpennessChanged(bool ){}

void GroupViewItem::itemClicked(const MouseEvent& ){
    if (z != nullptr){
        parent->getParent()->getWaveformView()->updateWaveformForFilePlayer(z);
        parent->getParent()->updateZone(z);
    }
}

void GroupViewItem::paintItem(Graphics& g, int width, int height) {
    
    if (isSelected()) g.fillAll(Colours::lightblue);
    
    g.setColour (Colours::black);
    g.setFont (15.0f);

    g.drawText (name,
                4, 0, width - 4, height,
                Justification::centredLeft, true);
}

GroupView::GroupView(GroupEditor* g, WaveBin* p) : TreeView(), group_editor(g), parent(p) 
{
    root_item = new GroupViewItem("Groups", true, nullptr, this);
    setRootItem(root_item);
    setRootItemVisible(true);
    
    groups = group_editor->getMappingEditor()->graph->getGroups();
    
    for (auto name : group_editor->getModel()->getGroupNames()){
        root_item->addSubItem(new GroupViewItem(name, true, nullptr, this));
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
        GroupViewItem* group = new GroupViewItem(name, true, nullptr, this);
        root_item->addSubItem(group);
        
        if (groups[group_count] != nullptr){
            for (int i=0; i<groups[group_count]->getZones()->size(); i++){ 
                InstrumentMappingEditor::MappingEditorGraph::Zone* z = (*(groups[group_count]->getZones()))[i];
                GroupViewItem* zone = new GroupViewItem(z->getName(), false, z, this);
                group->addSubItem(zone);
            }
        }
        group_count++;
    }
}
    


