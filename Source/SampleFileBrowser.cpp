/*
  ==============================================================================

    SampleFileBrowser.cpp
    Created: 7 Aug 2014 12:13:52pm
    Author:  patrick

  ==============================================================================
*/

#include "SampleFileBrowser.h"
#include "MappingEditorBin.h"
#include "InstrumentMappingEditor.h"

SampleFileBrowser::SampleFileBrowser() : FileBrowserComponent(FileBrowserComponent::openMode|
                                         FileBrowserComponent::canSelectFiles|
                                         FileBrowserComponent::useTreeView,
                                         File::getCurrentWorkingDirectory(),
                                         nullptr,
                                         nullptr)
{      
    setSize(300, getHeight());
}

void SampleFileBrowser::paint(Graphics& g){
    g.fillAll(Colours::white);
}

void SampleFileBrowser::mouseMove(const MouseEvent& m){
    int x = m.getPosition().getX();
    MouseCursor mc = getMouseCursor();
    if (fabs(getWidth() - x) <= 5){
        if (mc != MouseCursor::LeftRightResizeCursor)
            setMouseCursor(MouseCursor::LeftRightResizeCursor);
    }else{
        if (mc != MouseCursor::NormalCursor)
            setMouseCursor(MouseCursor::NormalCursor);
    }
}

void SampleFileBrowser::mouseDrag(const MouseEvent& m){
    int x = m.getPosition().getX();
    MouseCursor mc = getMouseCursor();
    if (mc == MouseCursor::LeftRightResizeCursor){
        if (x < 400 & x >= 100){
            setSize(x, getHeight());
            getParentComponent()->resized();
        }
    }
}

bool SampleFileBrowser::isFileSuitable(const File& f) const{
    if (f.getFileExtension() == ".lua"){
        return true;
    }else if (f.getFileExtension() == ".wav"){
        return true;
    }else if (f.getFileExtension() == ".aif"){
        return true;
    }else if (f.getFileExtension() == ".ogg"){
        return true;
    }
    return false;
}
