/*
  ==============================================================================

    InstrumentTabWindow.h
    Created: 9 May 2014 6:26:40pm
    Author:  patrick

  ==============================================================================
*/

#ifndef INSTRUMENTTABWINDOW_H_INCLUDED
#define INSTRUMENTTABWINDOW_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainPage.h"
#include "MappingEditorBin.h"
#include "FxBin.h"
#include "TransformBin.h"
#include "WaveBin.h"
#include "ScriptBin.h"

class InstrumentTabWindow : public TabbedComponent
{
public:
    InstrumentTabWindow(InstrumentComponent& instrument, TabbedButtonBar::Orientation orientation);
    MappingEditorBin* getMappingEditorBin(){return mappingEditor;}
    WaveBin* getWaveBin(){return waveBin;}
    FxBin* getFxBin(){return fxbin;}
    TransformBin* getTransformBin(){return transformBin;}
    MainPage* getMainPage(){return mainPage;}
    ScriptBin* getScriptBin(){return scriptBin;}
    void currentTabChanged(int, const String&);
private:
    ScopedPointer<MainPage> mainPage;
    ScopedPointer<MappingEditorBin> mappingEditor;
    ScopedPointer<WaveBin> waveBin;
    ScopedPointer<FxBin> fxbin;
    ScopedPointer<TransformBin> transformBin;
    ScopedPointer<ScriptBin> scriptBin;
    ScopedPointer<TextButton> zoomButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentTabWindow)
};

#endif  // INSTRUMENTTABWINDOW_H_INCLUDED

