#pragma once

#include<Siv3D.hpp>
#include<algorithm>

class Panel
{
    Plane plane_;
    
    const Texture texture_;

    const double width = 300;
    const double depth = 100;
public:
    Panel(String file)
        :plane_(Vec3::Zero, { 128, 72 }, Quaternion::Pitch(-Pi/2)), texture_(file, TextureDesc::For3D)
    {}

    Plane const & getPlane()const
    {
        return plane_;
    }

    double x(double t)const
    {
        double a = 4;
        double b = width/2;
        double c = 0;
        return b*(2/(1+Math::Exp(-a*t))-1) + c;
    }

    double y(double t)const 
    {
        double a = Pi / 2;
        double b = 10;
        double c = -10;
        return b*Math::Cos(a*t)*Math::Cos(a*t) + c;
    }


    double z(double t)const
    {
        double a = 20;
        double b = depth;
        double c = -depth;
        return b * (-2 / (Math::Exp(a * t) + Math::Exp(-a * t)) + 1) + c;
    }

    double angle(double t) const
    {
        double a = 1;
        double b = Pi/2*1.15;
        double c = 0;
        t = t*a;
        return b*((t < 0) ? -Math::Sqrt(1 - (t + 1)*(t + 1)) : Math::Sqrt(1 - (t - 1)*(t - 1))) + c;
    }

    double alpha(double t) const
    {
        double a = 15;
        double b = 0.7;
        double c = 0.3;
        return b * 2 / (Math::Exp(a * t) + Math::Exp(-a * t)) + c;
    }

    void updateAndDraw(double t)
    {
        plane_ = Plane({
            x(t)
            , y(t)
            , z(t)
        }, plane_.size, Quaternion::Pitch(-Pi/2).yaw(
            angle(t)
        ));
        plane_.drawForward(texture_, AlphaF(alpha(t)));
    }

    Optional<Vec3> isPointed()const
    {
        return Mouse::Ray().intersectsAt(plane_);
    }
};
