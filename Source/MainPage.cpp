#include "MainPage.h"
#include "InstrumentComponent.h"

MainPage::MainPage(InstrumentComponent& i) : Component{}, instrument{i} {} 
MainPage::~MainPage() {
    deleteAllChildren();
}

void MainPage::broughtToFront() {
    deleteAllChildren();
    int c = 0;
    for(auto i : instrument.getSampleNames()) {
        auto l = new Label("", i);
        l->setBounds(0,c,200,200);
        addAndMakeVisible(l);
        c+=15;
    }
}
