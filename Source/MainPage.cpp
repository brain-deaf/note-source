#include "MainPage.h"
#include "InstrumentComponent.h"

MainPage::MainPage(InstrumentComponent& i) : Component(), instrument(i), adsr(new Adsr()) {
    addAndMakeVisible(adsr);
    //adsr->setBounds(100, 5, 400, 150);
} 
MainPage::~MainPage() {
    delete adsr;
}

void MainPage::resized(){
   
}

void MainPage::broughtToFront() {
    adsr->setBounds(100, 5, 800, 350);
    /*deleteAllChildren();
    int c = 0;
    for(auto i : instrument.getSampleNames()) {
        auto l = new Label("", i);
        l->setBounds(0,c,200,200);
        addAndMakeVisible(l);
        c+=15;
    }*/
}
