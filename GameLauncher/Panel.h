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

    bool isTop_ = false;

    bool mouseOver = false;
public:
    Panel(String file);

    Plane getPlane()const;

    State getState() const { return state; }
    void setState(State state) { this->state = state; }

    bool isMouseOver() const { return mouseOver; }
    void setMouseOver(bool mouseOver) { this->mouseOver = mouseOver; }

    bool isTop()const { return isTop_; }
    void setIsTop(bool selected) { this->isTop_ = selected; }
    
    static void transition(
        std::shared_ptr<Panel> p, bool mouseOver, bool pressed, bool moved)
    {
        switch (p->getState()) {
        case Panel::LEFT:
            if (mouseOver && !pressed) {
                p->setState(Panel::OVER);
            }
            break;
        case Panel::OVER:
            if (pressed) {
                p->setState(Panel::PRESSED);
            }
            else if (!mouseOver) {
                p->setState(Panel::LEFT);
            }
            break;
        case Panel::PRESSED:
            if (pressed && moved) {
                p->setState(Panel::DRAGGED);
            }
            else if (!pressed) {
                p->setState(Panel::RELEASED);
            }
            break;
        case Panel::DRAGGED:
            if (!pressed) {
                p->setState(Panel::DROPPED);
            }
            break;
        case Panel::DROPPED:
            p->setState(Panel::LEFT);
            break;
        case Panel::RELEASED:
            p->setState(Panel::OVER);
            break;
        default:
            break;
        }
    }

    void update();

    static double x(double t);

    static double y(double t);

    static double z(double t);

    static double angle(double t);

    static double alpha(double t);

    void updateAndDraw(double t);
};
