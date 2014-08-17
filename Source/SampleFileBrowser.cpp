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
#include "SampleBrowserBin.h"

SampleFileBrowser::SampleFileBrowser() : FileBrowserComponent(FileBrowserComponent::openMode|
                                         FileBrowserComponent::canSelectFiles|
                                         FileBrowserComponent::canSelectDirectories|
                                         FileBrowserComponent::useTreeView|
                                         FileBrowserComponent::canSelectMultipleItems,
                                         File::getCurrentWorkingDirectory(),
                                         nullptr,
                                         nullptr),
                                         dragButton(nullptr),
                                         player(new FilePlayer(""))
{      

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
            getParentComponent()->setSize(x, getParentComponent()->getHeight());
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

void SampleFileBrowser::fileDoubleClicked(const File& f){
    if (!f.isDirectory() && filename != f.getFullPathName() && f.getFileExtension() != ".lua"){
        filename = f.getFullPathName();
        player = new FilePlayer(filename);
        player->changeState(FilePlayer::Starting);
    }else if (f.getFullPathName() == filename){
        player->toggleState();
    }
}

void SampleFileBrowser::fileClicked(const File& f, const MouseEvent& e){
    if (dragButton != nullptr){
        dragButton->getFileList().clear();
        
        for (int i=0; i<getNumSelectedFiles(); i++){
            if (!(getSelectedFile(i).isDirectory())){
                dragButton->getFileList().add(getSelectedFile(i).getFullPathName());
            }
        }
    }
}
