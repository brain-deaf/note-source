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

class ScriptBin;
class WaveBin;

class InstrumentTabWindow : public TabbedComponent
{
public:
    InstrumentTabWindow(InstrumentComponent& instrument, TabbedButtonBar::Orientation orientation);
	~InstrumentTabWindow()
	{
		mainPage = nullptr;
		mappingEditor = nullptr;
		fxbin = nullptr;
		transformBin = nullptr;
		scriptBin = nullptr;
	}
    MappingEditorBin* getMappingEditorBin(){return mappingEditor.get();}
    WaveBin* getWaveBin(){return waveBin.get();}
    FxBin* getFxBin(){return fxbin.get();}
    TransformBin* getTransformBin(){return transformBin.get();}
    MainPage* getMainPage(){return mainPage.get();}
    ScriptBin* getScriptBin(){return scriptBin.get();}
    void currentTabChanged(int, const String&);
private:
	ScopedPointer<MappingEditorBin> mappingEditor;
    ScopedPointer<MainPage> mainPage;
    ScopedPointer<WaveBin> waveBin;
    ScopedPointer<FxBin> fxbin;
    ScopedPointer<TransformBin> transformBin;
    ScopedPointer<ScriptBin> scriptBin;
    //ScopedPointer<TextButton> zoomButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentTabWindow)
};

#endif  // INSTRUMENTTABWINDOW_H_INCLUDED

