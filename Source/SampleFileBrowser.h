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

class SampleFileFilter : public FileFilter, public DragAndDropContainer
{
public:
    SampleFileFilter(const String& description) : FileFilter(description){}
    ~SampleFileFilter(){}
    bool isFileSuitable(const File& f) const {return true;}
    bool isDirectorySuitable(const File& f) const {return true;}
};

class SampleFileBrowser : public FileBrowserComponent
{
public:
    SampleFileBrowser();
    ~SampleFileBrowser(){delete fileFilter;}
    void paint(Graphics&);
    void mouseMove(const MouseEvent&);
    void mouseDrag(const MouseEvent&);
private:
    SampleFileFilter* fileFilter;
};





#endif  // SAMPLEFILEBROWSER_H_INCLUDED
