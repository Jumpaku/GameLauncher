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

    std::shared_ptr<Panel> detectMouseOver()const
    {
        std::vector<std::shared_ptr<Panel>> copied(panels.size());
        auto end = std::copy_if(panels.begin(), panels.end(), copied.begin(), [](auto panel) {
            return Mouse::Ray().intersects(panel->getPlane());
        });
        copied.erase(end, copied.end());
        auto itr = std::min_element(copied.begin(), copied.end(), [](auto p0, auto p1) {
            return p0->getPlane().z < p1->getPlane().z;
        });
        return itr == copied.end() ? nullptr : *itr;
    }

    void updateState()
    {
        std::transform(panels.begin(), panels.end(), panels.begin(), [](auto p) {
            p->setMouseOver(false);
            return p;
        });
        std::shared_ptr<Panel> panel = detectMouseOver();
        if (panel) {
            panel->setMouseOver(true);
        }
        
        std::transform(panels.begin(), panels.end(), panels.begin(), [](auto p) {
            p->setSelected(false);
            return p;
        });


        std::for_each(panels.begin(), panels.end(), [](auto p) {
            p->update();
        });

        switch (getState()) {
        case INIT: {
            auto draged = std::find_if(panels.begin(), panels.end(), [](auto p)->bool{
                return p->getState() == Panel::DRAGGED;
            });
            auto clicked = std::find_if(panels.begin(), panels.end(), [](auto p)->bool{
                return (!p->isSelected()) && p->getState() == Panel::RELEASED;
            });
            auto selected = std::find_if(panels.begin(), panels.end(), [](auto p)->bool{
                return (!p->isSelected()) && p->getState() == Panel::RELEASED;
            });

            if (draged != panels.end()) {
                setState(State::DRAG);
            }
            else if (clicked != panels.end()) {
                setState(State::ANIME);
                animeStopWatch.start();
                int i = clicked - panels.begin();
                animeInfo = {
                    parameter,
                    parameter - clamp(parameter + delta(i, panels.size())),
                    true
                };
            }
            else if (selected != panels.end()) {
                setState(State::DONE);
            }
            break;
        }
        case DRAG: {
            auto dropped = std::find_if(panels.begin(), panels.end(), [](auto p)->bool{
                return p->getState() == Panel::DROPPED;
            });

            if (dropped != panels.end()) {
                setState(State::ANIME);
                animeStopWatch.start();
                auto center = std::min_element(panels.begin(), panels.end(), [](auto p0, auto p1)->bool {
                    return Math::Abs(p0->getPlane().x) < Math::Abs(p1->getPlane().x);
                });
                int i = center - panels.begin();
                animeInfo = {
                    parameter,
                    parameter - clamp(parameter + delta(i, panels.size())),
                    true
                };
            }
            break;
        }
        case ANIME:
            if (!animeInfo.continues) {
                setState(State::INIT);
            }
            break;
        case DONE:
            break;
        default:
            break;
        }
    }

    void update()
    {
        switch (getState()) {
        case PanelManager::INIT:
            break;
        case PanelManager::DRAG:
            parameter += Mouse::DeltaF().x / Panel::width;
            break;
        case PanelManager::ANIME: {
            double time = Min(1.0,
                animeStopWatch.ms() / Math::Abs(animeInfo.beginParam - animeInfo.endParam) /200);
            parameter = EaseInOut(
                animeInfo.beginParam, animeInfo.endParam, Easing::Sine, time);
            if (time >= 1.0) {
                animeInfo.continues = false;
                animeStopWatch.reset();
            }
            break;
        }
        case PanelManager::DONE: {
            auto center = std::min_element(panels.begin(), panels.end(), [](auto p0, auto p1)->bool {
                return Math::Abs(p0->getPlane().x) < Math::Abs(p1->getPlane().x);
            });
            int i = center - panels.begin();
            Print(L"launch ", i);
            break;
        }
        default:
            break;
        }
    }
   
    double clamp(double t)
    {
        return t > 1.0 ? (-1.0 + Math::Fmod(t + 1, 2.0)) :
            t < -1.0 ? (1 + Math::Fmod(t + 1, 2.0)) :
            t;
    }

    double delta(int index, int n)
    {
        return index * 2.0 / n;
    }
    
    void updateAndDraw()
    {
        Graphics3D::SetCamera(camera);

        /*double d = Input::KeyRight.pressed ? 0.01 : 0;
        Input::KeyLeft.pressed ? d -= 0.01 : d;
        parameter += d;*/
         
        for (int i = 0; i < panels.size(); ++i) {
            panels[i]->updateAndDraw(clamp(parameter + delta(i, panels.size())));
        }

        updateState();
        update();
    }
};
