#include "PanelSelector.h"

void PanelSelector::updatePanels()
{
    auto planes = makePlanes();

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
        auto panelState = Panel::transition(
            panels[i]->getState(), i == index, Input::MouseL.pressed, Math::Abs(Mouse::DeltaF().x) > 1);
        panels[i]->setState(panelState);
    };
}

void PanelSelector::startAnimation(int index)
{
    animation.emplace(Animation{
        parameter,
        parameter - clamp(parameter + delta(index, panels.size()))
    });
}

PanelSelector::State PanelSelector::transition(
    PanelSelector::State state, Panel::State anyPanelState, Panel::State centerPanelState, Optional<Animation> const &animation)
{
    switch (state) {
    case State::INITIAL:
        if (centerPanelState == Panel::State::RELEASED) {
            return State::FIRE_EVENT;
        }
        else if (anyPanelState == Panel::State::RELEASED) {
            return State::ROTATE_CLICKED;
        }
        else if (anyPanelState == Panel::State::DRAGGED) {
            return State::DRAG_ROTATE;
        }
        break;
    case State::DRAG_ROTATE:
        if (anyPanelState == Panel::State::DROPPED) {
            return State::ROTATE_DROPPED;
        }
        break;
    case State::ROTATE_DROPPED:
        return State::ANIME_ROTATE;
        break;
    case State::ROTATE_CLICKED:
        return State::ANIME_ROTATE;
        break;
    case State::ANIME_ROTATE:
        if (animation->hasFinished()) {
            return State::INITIAL;
        }
        break;
    case State::FIRE_EVENT:
        break;
    default:
        break;
    }
    return state;
}

std::vector<Plane> PanelSelector::makePlanes() const
{
    std::vector<Plane> planes(panels.size());
    for (int i = 0; i < panels.size(); ++i) {
        planes[i] = panels[i]->makePlane(getParamOfIndex(i));
    }
    return planes;
}

Panel::State PanelSelector::checkAnyPanelState() const
{
    if (std::any_of(panels.begin(), panels.end(), [](auto p) {
        return p->getState() == Panel::State::DRAGGED;
    })) {
        return Panel::State::DRAGGED;
    }
    else if (std::any_of(panels.begin(), panels.end(), [](auto p) {
        return p->getState() == Panel::State::DROPPED;
    })) {
        return Panel::State::DROPPED;
    }
    else if (std::any_of(panels.begin(), panels.end(), [](auto p) {
        return p->getState() == Panel::State::RELEASED;
    })) {
        return Panel::State::RELEASED;
    }
    else if (std::any_of(panels.begin(), panels.end(), [](auto p) {
        return p->getState() == Panel::State::RELEASED;
    })) {
        return Panel::State::RELEASED;
    }
    else {
        return Panel::State::LEFT;
    }
}

int PanelSelector::getCenterIndex() const
{
    auto planes = makePlanes();
    auto itr = std::min_element(planes.begin(), planes.end(), [](auto const &a, auto const &b) {
        return Math::Abs(a.x) < Math::Abs(b.x);
    });
    return itr - planes.begin();
}

void PanelSelector::update()
{
    updatePanels();
    state = transition(state,
        checkAnyPanelState(),
        panels[getCenterIndex()]->getState(),
        animation);

    switch (state) {
    case PanelSelector::State::INITIAL:
        animation = none;
        break;
    case PanelSelector::State::DRAG_ROTATE:
        parameter += Mouse::DeltaF().x / Panel::WIDTH;
        break;
    case PanelSelector::State::ROTATE_DROPPED: {
        auto planes = makePlanes();
        auto center = std::min_element(planes.begin(), planes.end(), [](auto const &p0, auto const &p1)->bool {
            return Abs(p0.x) < Abs(p1.x);
        });
        startAnimation(center - planes.begin());
        break;
    }
    case PanelSelector::State::ROTATE_CLICKED: {
        auto clicked = std::find_if(panels.begin(), panels.end(), [](auto p)->bool {
            return p->getState() == Panel::State::RELEASED;
        });
        startAnimation(clicked - panels.begin());
        break;
    }
    case PanelSelector::State::ANIME_ROTATE:
        parameter = animation->getParameter();
        break;
    case PanelSelector::State::FIRE_EVENT:
        onSelected(getCenterIndex());
        break;
    default:
        break;
    }
}

void PanelSelector::draw()const
{
    Graphics3D::SetCamera(camera);

    //Print(L"" , static_cast<int>(state));

    for (int i = 0; i < panels.size(); ++i) {
        panels[i]->draw(getParamOfIndex(i));
    }
}


