/*
  ==============================================================================

    SampleFileBrowser.h
    Created: 7 Aug 2014 12:14:03pm
    Author:  patrick

  ==============================================================================
*/

#ifndef SAMPLEFILEBROWSER_H_INCLUDED
#define SAMPLEFILEBROWSER_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "FilePlayer.h"

class DragButton;

class SampleFileBrowser : public FileBrowserComponent
{
public:
    SampleFileBrowser();
    ~SampleFileBrowser(){}
    void paint(Graphics&);
    void mouseMove(const MouseEvent&);
    void mouseDrag(const MouseEvent&);
    bool isFileSuitable(const File&)const override;
    void fileDoubleClicked(const File&)override;
    void setDragButton(DragButton* d){dragButton=d;}
    void fileClicked(const File&, const MouseEvent&) override;
private:
    ScopedPointer<FilePlayer> player;
    String filename;
    DragButton* dragButton;
    int height;
};





#endif  // SAMPLEFILEBROWSER_H_INCLUDED
