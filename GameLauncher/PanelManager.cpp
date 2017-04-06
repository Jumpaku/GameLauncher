#include "PanelManager.h"

void PanelManager::setIfTopMouseOver()
{
    std::transform(panels.begin(), panels.end(), panels.begin(), [](auto p) {
        p->setMouseOver(false);
        return p;
    });

    std::vector<std::shared_ptr<Panel>> copied(panels.size());
    auto end = std::copy_if(panels.begin(), panels.end(), copied.begin(), [](auto panel) {
        return Mouse::Ray().intersects(panel->getPlane());
    });
    auto itr = std::min_element(copied.begin(), end, [](auto p0, auto p1) {
        return p0->getPlane().z < p1->getPlane().z;
    });
    if (itr != end) {
        (*itr)->setMouseOver(true);
    }
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
    setIfTopMouseOver();
    std::for_each(panels.begin(), panels.end(), [](auto p) {
        p->update();
    });

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
        panels[i]->updateAndDraw(clamp(parameter + delta(i, panels.size())));
    }
}
