#pragma once

#include<vector>
#include<Siv3D.hpp>
#include"Panel.h"

class PanelManager
{
    std::vector<Panel> panels;
    Camera camera;
    double t;

public:
    PanelManager()
        :panels({
        Panel(L"Asset/Title0.png"),
        Panel(L"Asset/Title1.png"),
        Panel(L"Asset/Title2.png"),
        Panel(L"Asset/Title3.png"),
        Panel(L"Asset/Title4.png"),
        /*Panel(L"Asset/Title5.png"),
        Panel(L"Asset/Title6.png"),
        Panel(L"Asset/Title7.png"),
        Panel(L"Asset/Title8.png"),
        Panel(L"Asset/Title9.png"),*/
    })
    {
        camera.lookat = { 0, 0, 1 };
        camera.pos = { 0, 0, -300 };
        camera.up = { 0, 1, 0 }; 
    }

    int detectPanelPointing()
    {
        int index = -1;
        double minZ = 100;
        for (int i = 0; i < panels.size(); ++i){
            Mouse::Ray().intersectsAt(panels[i].getPlane()).then([&index, &minZ, i](Vec3 v) {
                if (minZ > v.z) {
                    index = i;
                    minZ = v.z;
                }
            });
        }
        return index;
    }
   
    double clamp_m1_1(double t)
    {
        return t > 1.0 ? (-1.0 + Math::Fmod(t + 1, 2.0)) :
            t < -1.0 ? (1 + Math::Fmod(t + 1, 2.0)) :
            t;
    }
    
    void updateAndDraw()
    {
        Graphics3D::SetCamera(camera);

        double d = Input::KeyRight.pressed ? 0.01 : 0;
        Input::KeyLeft.pressed ? d -= 0.01 : d;
        t += d;
         
        for (int i = 0; i < panels.size(); ++i) {
            panels[i].updateAndDraw(clamp_m1_1(t + i * 2.0 / panels.size()));
        }
    }
};
