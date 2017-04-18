#include "Panel.h"

double const Panel::WIDTH = 300;

double const Panel::DEPTH = 100;

Panel::Panel(String file, Size size)
        :texture(file, TextureDesc::For3D), size(size)
    {}

Panel::State Panel::transition(State const state, bool const mouseOver, bool const pressed, bool const moved)
{
    switch (state) {
    case State::LEFT:
        if (mouseOver && !pressed) {
            return State::OVER;
        }
        break;
    case State::OVER:
        if (pressed) {
            return State::PRESSED;
        }
        else if (!mouseOver) {
            return State::LEFT;
        }
        break;
    case State::PRESSED:
        if (pressed && moved) {
            return State::DRAGGED;
        }
        else if (!pressed) {
            return State::RELEASED;
        }
        break;
    case State::DRAGGED:
        if (!pressed) {
            return State::DROPPED;
        }
        break;
    case State::DROPPED:
        return State::LEFT;
        break;
    case State::RELEASED:
        return State::OVER;
        break;
    default:
        break;
    }
    return state;
}

void Panel::draw(double t)const
{
    makePlane(t)
        .drawForward(texture, AlphaF(computeAlpha(t)));
}
