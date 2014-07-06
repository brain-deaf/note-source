#ifndef MAINPAGE_H
#define MAINPAGE_H 

#include "../JuceLibraryCode/JuceHeader.h"
#include "Adsr.h"

class InstrumentComponent;

class MainPage : public Component {
    Array<ScopedPointer<Label>> labels;
    InstrumentComponent& instrument;
public:
    MainPage(InstrumentComponent& i);
    ~MainPage();
    void changeListenerCallback(ChangeBroadcaster * src);
    void broughtToFront() override;
    void resized();
private:
    Adsr* adsr;

};

#endif /* MAINPAGE_H */
