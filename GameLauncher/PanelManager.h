#pragma once

#include<vector>
#include<Siv3D.hpp>
#include"Panel.h"

class PanelManager
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
    
    void startAnimation(int index);

    static State transition(
        PanelManager::State state, Panel::State anyPanelState, Panel::State centerPanelState, Optional<Animation> const &animation);

    std::vector<Plane> makePlanes()const
    {
        std::vector<Plane> planes(panels.size());
        for (int i = 0; i < panels.size(); ++i) {
            planes[i] = panels[i]->makePlane(getParamOfIndex(i));
        }
        return planes;
    }

    Panel::State checkAnyPanelState()const
    {
        if (std::any_of(panels.begin(), panels.end(), [](auto p) {
            return p->getState() == Panel::State::DRAGGED;
        }))
        {
            return Panel::State::DRAGGED;
        }
        else if (std::any_of(panels.begin(), panels.end(), [](auto p) {
            return p->getState() == Panel::State::DROPPED;
        }))
        {
            return Panel::State::DROPPED;
        }
        else if (std::any_of(panels.begin(), panels.end(), [](auto p) {
            return p->getState() == Panel::State::RELEASED;
        }))
        {
            return Panel::State::RELEASED;
        }
        else if (std::any_of(panels.begin(), panels.end(), [](auto p) {
            return p->getState() == Panel::State::RELEASED;
        }))
        {
            return Panel::State::RELEASED;
        }
        else {
            return Panel::State::LEFT;
        }
    }

    int getCenterIndex()const
    {
        auto planes = makePlanes();
        auto itr = std::min_element(planes.begin(), planes.end(), [](auto const &a, auto const &b) {
            return Math::Abs(a.x) < Math::Abs(b.x);
        });
        return itr - planes.begin();
    }

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
