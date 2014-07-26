/*
  ==============================================================================

    WaveBin.h
    Created: 26 Jul 2014 2:09:03pm
    Author:  patrick

  ==============================================================================
*/

#ifndef WAVEBIN_H_INCLUDED
#define WAVEBIN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "GroupEditorZ.h"
#include "MappingEditorBin.h"

class WaveBin : public Component {
public:
    WaveBin(MappingEditorBin*);
    ~WaveBin();
    void resized();
private:
    GroupView* group_view;
    Component* wave_editor;
    MappingEditorBin* mapping_editor;
    int group_view_width;
    int vport_width;
    Viewport* Vport;
};



#endif  // WAVEBIN_H_INCLUDED
