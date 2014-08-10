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

class SampleFileBrowser : public FileBrowserComponent
{
public:
    SampleFileBrowser();
    ~SampleFileBrowser(){}
    void paint(Graphics&);
    void mouseMove(const MouseEvent&);
    void mouseDrag(const MouseEvent&);
    bool isFileSuitable(const File&)const override;
private:
};





#endif  // SAMPLEFILEBROWSER_H_INCLUDED
