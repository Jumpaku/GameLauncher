#pragma once

#include<Siv3D.hpp>
#include<memory>
#include<algorithm>

/*class PanelView
{
    enum State
    {
        LEFT,
        SELECTED,

    };
    enum PanelsState
    {
        STOPPING,
        ROTATING,
        SELECTED,
    };
    Texture const texture;
    Size const size;
public:
    void setState(State state);
    void renderPanel(Vec3 center, double theta, double alpha)const{
        Plane(center, size, Quaternion::Pitch(-Pi / 2).yaw(theta))
            .drawForward(texture, AlphaF(alpha));
    }
};

class PanelMovement
{
    double const delta;
    std::shared_ptr<PanelView> const view;
public:
    PanelMovement(std::shared_ptr<PanelView> view, int id, int n)
        :view(view), delta((2.0*id) / n){}

    double clamp(double t)const
    {
        return
            t > 1 ? fmod(t + 1, 2.0) - 1 :
            t < -1 ? fmod(t + 1, 2.0) + 1 :
            t;
    }
    double delta(int index, int n)
    {
        return ;
    }
    double x(double t)const;

    double y(double t)const;

    double z(double t)const;

    double angle(double t) const;

    double alpha(double t) const;
    void update(double parameter)
    {
        double t = clamp(parameter + delta);
        view->renderPanel({ x(t), y(t), z(t) }, angle(t), alpha(t));
    }

};*/

class Panel
{
public:
    enum State
    {
        LEFT,
        OVER,
        PRESSED,
        DRAGGED,
        DROPPED,
        RELEASED,
    };

    static double const width;

    static double const depth;
private:
    Plane plane_;
    
    const Texture texture_;

    State state = LEFT;

    bool selected = false;

    bool mouseOver = false;
public:
    Panel(String file);

    Plane getPlane()const;

    State getState() const { return state; }
    void setState(State state) { this->state = state; }

    bool isMouseOver() const { return mouseOver; }
    void setMouseOver(bool mouseOver) { this->mouseOver = mouseOver; }

    bool isSelected()const { return selected; }
    void setSelected(bool selected) { this->selected = selected; }

    void update()
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



    double x(double t)const;

    double y(double t)const;

    double z(double t)const;

    double angle(double t) const;

    double alpha(double t) const;

    void updateAndDraw(double t);
};
