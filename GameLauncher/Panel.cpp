#include "Panel.h"

double const Panel::width = 300;

double const Panel::depth = 100;

Panel::Panel(String file)
        :plane_(Vec3::Zero, { 128, 72 }, Quaternion::Pitch(-Pi/2)), texture_(file, TextureDesc::For3D)
    {}

Plane Panel::getPlane() const
{
    return plane_;
}

void Panel::update()
{
    switch (getState()) {
    case State::LEFT:
        if (isMouseOver() && !Input::MouseL.pressed) {
            setState(State::OVER);
        }
        break;
    case State::OVER:
        if (Input::MouseL.pressed) {
            setState(State::PRESSED);
        }
        else if (!isMouseOver()) {
            setState(State::LEFT);
        }
        break;
    case State::PRESSED:
        if (Input::MouseL.pressed && Math::Abs(Mouse::DeltaF().x) > 1) {
            setState(State::DRAGGED);
        }
        else if (Input::MouseL.released) {
            setState(State::RELEASED);
        }
        break;
    case State::DRAGGED:
        if (Input::MouseL.released) {
            setState(State::DROPPED);
        }
        break;
    case State::DROPPED:
        setState(State::LEFT);
        break;
    case State::RELEASED:
        setState(State::OVER);
        break;
    default:
        break;
    }
}

double Panel::x(double t) 
{
    double a = 4;
    double b = width / 2;
    double c = 0;
    return b*(2 / (1 + Math::Exp(-a*t)) - 1) + c;
}

double Panel::y(double t) 
{
    double a = Pi / 2;
    double b = 10;
    double c = -10;
    return b*Math::Cos(a*t)*Math::Cos(a*t) + c;
}

double Panel::z(double t) 
{
    double a = 20;
    double b = depth;
    double c = -depth;
    return b * (-2 / (Math::Exp(a * t) + Math::Exp(-a * t)) + 1) + c;
}

double Panel::angle(double t) 
{
    double a = 1;
    double b = Pi / 2 * 1.15;
    double c = 0;
    t = t*a;
    return b*((t < 0) ? -Math::Sqrt(1 - (t + 1)*(t + 1)) : Math::Sqrt(1 - (t - 1)*(t - 1))) + c;
}

double Panel::alpha(double t) 
{
    double a = 15;
    double b = 0.3;
    double c = 0.7;
    return b * 2 / (Math::Exp(a * t) + Math::Exp(-a * t)) + c;
}

void Panel::updateAndDraw(double t)
{
    plane_ = Plane({
        x(t)
        , y(t)
        , z(t)
    }, plane_.size, Quaternion::Pitch(-Pi / 2).yaw(
        angle(t)
    ));
    plane_.drawForward(texture_, AlphaF(alpha(t)));
}
