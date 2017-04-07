#pragma once

#include<vector>
#include<Siv3D.hpp>
#include"Panel.h"

class PanelManager
{
public:
    enum State
    {
        INIT,
        DRAG,
        ANIME,
        DONE,
    };

    struct AnimeInfo
    {
        double beginParam;
        double endParam;
        bool continues;
    };

private:
    std::vector<std::shared_ptr<Panel>> panels;

    Camera const camera;
    
    double parameter = 0;

    State state = INIT;

    Stopwatch animeStopWatch;

    AnimeInfo animeInfo;

    std::function<void(int)> onSelected = [](int i) {Print(L"launch ", i);};

public:
    PanelManager()
        :panels({
            std::make_shared<Panel>(L"Asset/Title0.png"),
            std::make_shared<Panel>(L"Asset/Title1.png"),
            std::make_shared<Panel>(L"Asset/Title2.png"),
            std::make_shared<Panel>(L"Asset/Title3.png"),
            std::make_shared<Panel>(L"Asset/Title4.png"),
            std::make_shared<Panel>(L"Asset/Title5.png"),
        }),
        camera({ 0, 0, -300 }, { 0, 0, 1 }, { 0, 1, 0 }, Camera().fovDegree, Camera().nearClip)
    {}

    State getState()const { return state; }
    void setState(State state) { this->state = state; }

    void updatePanels();
    
    void setCenterSelected();
    
    void startAnimation(int index);

    void continueAnimation();

    void fireOnSelected()const;

    void updateState();

    void update();
   
    static double clamp(double t)
    {
        return t > 1.0 ? (-1.0 + Math::Fmod(t + 1, 2.0)) :
            t < -1.0 ? (1 + Math::Fmod(t + 1, 2.0)) :
            t;
    }

    static double delta(int index, int n)
    {
        return index * 2.0 / n;
    }

    double getParamOfIndex(int index)const
    {
        return clamp(parameter + delta(index, panels.size()));
    }

    /*const &Panel getTop()const
    {
        auto center = std::min_element(panels.begin(), panels.end(), [](auto p0, auto p1)->bool {
            return Math::Abs(p0->getPlane().x) < Math::Abs(p1->getPlane().x);
        });

        return **center;
    }*/
    
    void draw()const;
};
