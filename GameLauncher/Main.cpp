
#include<Siv3D.hpp>
#include"PanelManager.h"


void Main()
{
    PanelManager pm;
    while (System::Update()) {
        ClearPrint();
        pm.updateAndDraw();
    }
}
