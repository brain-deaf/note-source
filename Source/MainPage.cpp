#include "MainPage.h"
#include "InstrumentComponent.h"

MainPage::MainPage(InstrumentComponent& i) : Component(), instrument(i){}

MainPage::~MainPage() {}

void MainPage::resized(){}

void MainPage::broughtToFront() {}
