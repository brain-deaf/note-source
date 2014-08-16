/*
  ==============================================================================

    SampleBrowserBin.h
    Created: 10 Aug 2014 9:54:42am
    Author:  patrick

  ==============================================================================
*/

#ifndef SAMPLEBROWSERBIN_H_INCLUDED
#define SAMPLEBROWSERBIN_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleFileBrowser.h"

class DragButton : public TextButton
{
public:
    DragButton() : TextButton("Drag Selection"), files(){}
    ~DragButton(){}
    void mouseDrag(const MouseEvent& e){
        static DragAndDropContainer* d = DragAndDropContainer::findParentDragContainerFor(this);
        d->startDragging("file name", this);
    }
    Array<String>& getFileList(){return files;}
private:
    Array<String> files;
};
        

class SampleBrowserBin : public Component
{
public:
    SampleBrowserBin();
    ~SampleBrowserBin(){}
    SampleFileBrowser* getBrowser(){return browser.get();}
    void resized();
private:
    ScopedPointer<DragButton> dragButton;
    ScopedPointer<SampleFileBrowser> browser;
    int buttonHeight;
};




#endif  // SAMPLEBROWSERBIN_H_INCLUDED
