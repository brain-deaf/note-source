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

class Zone;
class WaveBin;
class GroupView;
class Group;
class GroupViewItem : public TreeViewItem
{
public:
    GroupViewItem(String, bool, Zone*, GroupView*);
    void itemOpennessChanged(bool);
    void paintItem(Graphics&, int, int) override;
    String getUniqueName(){return name;}
    bool mightContainSubItems(){return canBeOpened;}
    void itemClicked(const MouseEvent&) override;
private:
    String name;
    bool canBeOpened;
    Zone* z;
    GroupView* parent;
};

class GroupView : public TreeView
{
public:
    GroupView(GroupEditor*, WaveBin*);
	~GroupView(){
		deleteRootItem();
		setRootItem(nullptr);
	}
    void refreshRows();
    void paint(Graphics&);
    WaveBin* getParent(){return parent;}
private:
    GroupViewItem* root_item;
    GroupEditor* group_editor;
    //TreeView tree_view;
    Array<Group*> groups;
    WaveBin* parent;
};




#endif  // GROUPEDITORZ_H_INCLUDED
