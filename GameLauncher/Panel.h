#pragma once

#include<Siv3D.hpp>
#include<algorithm>

class Panel
{
    Plane plane_;
    
    const Texture texture_;

    static double const width;

    static double const depth;
public:
    Panel(String file);

    Plane getPlane()const;

    double x(double t)const;

    double y(double t)const;

    double z(double t)const;

    double angle(double t) const;

    double alpha(double t) const;

    void updateAndDraw(double t);

    Optional<Vec3> isPointed()const;
};
