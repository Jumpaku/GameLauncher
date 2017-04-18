#pragma once

#include<vector>
#include<Siv3D.hpp>
#include"Panel.h"

class PanelSelector
{
public:
    enum class State
    {
        INITIAL,
        DRAG_ROTATE,
        ROTATE_DROPPED,
        ROTATE_CLICKED,
        ANIME_ROTATE,
        FIRE_EVENT,
    };

    class Animation
    {
        double const begin;
        double const end;
        Stopwatch timer;
        bool flgFinished = false;
    public:
        Animation(double begin, double end)
            :begin(begin), end(end)
        {
            timer.start();
        }
        
        double getParameter()
        {
            double time = Min(1.0, timer.ms() / Math::Abs(begin - end) / 200);
            flgFinished = time >= 1.0;
            return EaseInOut(begin, end, Easing::Sine, time);
        }
        
        bool hasFinished()const
        {
            return flgFinished;
        }
    };

private:
    std::vector<std::shared_ptr<Panel>> panels;

    Camera const camera;
    
    double parameter = 0;

    State state = State::INITIAL;

    Optional<Animation> animation;

    std::function<void(int)> onSelected = [](int i) {Print(L"launch ", i);};

public:
    PanelSelector()
        :panels({
            std::make_shared<Panel>(L"Asset/Title0.png", Size{ 128, 72 }),
            std::make_shared<Panel>(L"Asset/Title1.png", Size{ 128, 72 }),
            std::make_shared<Panel>(L"Asset/Title2.png", Size{ 128, 72 }),
            std::make_shared<Panel>(L"Asset/Title3.png", Size{ 128, 72 }),
            std::make_shared<Panel>(L"Asset/Title4.png", Size{ 128, 72 }),
            std::make_shared<Panel>(L"Asset/Title5.png", Size{ 128, 72 }),
    }),
        camera({ 0, 0, -300 }, { 0, 0, 1 }, { 0, 1, 0 }, Camera().fovDegree, Camera().nearClip)
    {}

    void updatePanels();
    
    void startAnimation(int index);

    static State transition(
        PanelSelector::State state, Panel::State anyPanelState, Panel::State centerPanelState, Optional<Animation> const &animation);

    std::vector<Plane> makePlanes()const;

    Panel::State checkAnyPanelState()const;

    int getCenterIndex()const;

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

    void draw()const;
};
