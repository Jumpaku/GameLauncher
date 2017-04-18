#pragma once

#include<Siv3D.hpp>
#include<memory>
#include<algorithm>

class Panel
{
public:
    enum class State
    {
        LEFT,
        OVER,
        PRESSED,
        DRAGGED,
        DROPPED,
        RELEASED,
    };

    static double const WIDTH;

    static double const DEPTH;
private:
    const Texture texture;

    const Size size;

    State state = State::LEFT;
public:
    Panel(String file, Size size);

    State getState() const { return state; }
    void setState(State state) { this->state = state; }

    static State transition(
        State const state, bool const mouseOver, bool const pressed, bool const moved);


    Plane makePlane(double t)const
    {
        return Plane({
            computeX(t)
            , computeY(t)
            , computeZ(t)
        }, size, Quaternion::Pitch(-Pi / 2).yaw(
            computeAngle(t)
        ));
    }

    static double computeX(double t)
    {
        double a = 4;
        double b = WIDTH / 2;
        double c = 0;
        return b*(2 / (1 + Math::Exp(-a*t)) - 1) + c;
    }

    static double computeY(double t)
    {
        double a = Pi / 2;
        double b = 10;
        double c = -10;
        return b*Math::Cos(a*t)*Math::Cos(a*t) + c;
    }

    static double computeZ(double t)
    {
        double a = 20;
        double b = DEPTH;
        double c = -DEPTH;
        return b * (-2 / (Math::Exp(a * t) + Math::Exp(-a * t)) + 1) + c;
    }

    static double computeAngle(double t)
    {
        double a = 1;
        double b = Pi / 2 * 1.15;
        double c = 0;
        t = t*a;
        return b*((t < 0) ? -Math::Sqrt(1 - (t + 1)*(t + 1)) : Math::Sqrt(1 - (t - 1)*(t - 1))) + c;
    }

    static double computeAlpha(double t)
    {
        double a = 15;
        double b = 0.3;
        double c = 0.7;
        return b * 2 / (Math::Exp(a * t) + Math::Exp(-a * t)) + c;
    }

    void draw(double t)const;
};
