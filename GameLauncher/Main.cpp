
#include<Siv3D.hpp>
#include"PanelSelector.h"


void Main()
{
    PanelSelector selector;
    while (System::Update()) {
        selector.update();
        selector.draw();
    }
}
