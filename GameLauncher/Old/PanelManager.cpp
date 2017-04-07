#include "PanelManager.h"

void PanelManager::updatePanels()
{
    int index = -1;
    Optional<double> minZ = none;
    for (int i = 0; i < panels.size(); ++i) {
        auto z = Mouse::Ray()
            .intersectsAt(panels[i]->makePlane(getParamOfIndex(i)))
            .then([](Vec3 v) {return v.z;});

        if (z) {
            if (index == -1) {
                index = i;
                minZ = z;
            }
            else {
                index = z.value() < minZ.value() ? i : index;
                minZ = std::min(z.value(), minZ.value());
            }
        }
    }

    for (int i = 0; i < panels.size(); ++i) {
        Panel::transition(
            panels[i], i == index, Input::MouseL.pressed, Math::Abs(Mouse::DeltaF().x) > 1);
    };
}

void PanelManager::setCenterSelected()
{
    auto center = std::min_element(panels.begin(), panels.end(), [](auto p0, auto p1)->bool {
        return Math::Abs(p0->getPlane().x) < Math::Abs(p1->getPlane().x);
    });
    std::transform(panels.begin(), panels.end(), panels.begin(), [](auto p) {
        p->setIsTop(false);
        return p;
    });
    if (center != panels.end()) {
        (*center)->setIsTop(true);
    }
}

void PanelManager::startAnimation(int index)
{
    animeStopWatch.start();
    animeInfo = {
        parameter,
        parameter - clamp(parameter + delta(index, panels.size())),
        true
    };
}

void PanelManager::continueAnimation()
{
    double time = Min(1.0,
        animeStopWatch.ms() / Math::Abs(animeInfo.beginParam - animeInfo.endParam) / 200);
    parameter = EaseInOut(
        animeInfo.beginParam, animeInfo.endParam, Easing::Sine, time);
    if (time >= 1.0) {
        animeInfo.continues = false;
        animeStopWatch.reset();
    }
}

void PanelManager::fireOnSelected() const
{
    auto center = std::min_element(panels.begin(), panels.end(), [](auto p0, auto p1)->bool {
        return Math::Abs(p0->getPlane().x) < Math::Abs(p1->getPlane().x);
    });
    onSelected(center - panels.begin());
}

void PanelManager::updateState()
{
    setCenterSelected();
    updatePanels();

    switch (getState()) {
    case INIT:
        if (std::any_of(panels.begin(), panels.end(), [](auto p)->bool {
            return (p->isTop()) && p->getState() == Panel::RELEASED;
        })) {
            setState(State::DONE);
        }
        else if (std::any_of(panels.begin(), panels.end(), [](auto p)->bool {
            return p->getState() == Panel::DRAGGED;
        })) {
            setState(State::DRAG);
        }
        else if (std::any_of(panels.begin(), panels.end(), [](auto p)->bool {
            return (!p->isTop()) && p->getState() == Panel::RELEASED;
        })) {
            setState(State::ANIME);
            auto clicked = std::find_if(panels.begin(), panels.end(), [](auto p)->bool {
                return (!p->isTop()) && p->getState() == Panel::RELEASED;
            });
            startAnimation(clicked - panels.begin());
        }
        break;
    case DRAG:
        if (std::any_of(panels.begin(), panels.end(), [](auto p)->bool {
            return p->getState() == Panel::DROPPED;
        })) {
            setState(State::ANIME);
            auto center = std::min_element(panels.begin(), panels.end(), [](auto p0, auto p1)->bool {
                return Math::Abs(p0->getPlane().x) < Math::Abs(p1->getPlane().x);
            });
            startAnimation(center - panels.begin());
        }
        break;
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

void PanelManager::update()
{
    updateState();

    switch (getState()) {
    case PanelManager::INIT:
        break;
    case PanelManager::DRAG:
        parameter += Mouse::DeltaF().x / Panel::width;
        break;
    case PanelManager::ANIME:
        continueAnimation();
        break;
    case PanelManager::DONE:
        fireOnSelected();
        break;
    default:
        break;
    }
}

void PanelManager::draw()const
{
    Graphics3D::SetCamera(camera);

    for (int i = 0; i < panels.size(); ++i) {
        panels[i]->updateAndDraw(getParamOfIndex(i));
    }
}


