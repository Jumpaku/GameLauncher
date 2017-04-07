#include "Panel.h"

double const Panel::width = 300;

double const Panel::depth = 100;

Panel::Panel(String file)
        :texture_(file, TextureDesc::For3D)
    {}

void Panel::draw(double t)const
{
    makePlane(t)
        .drawForward(texture_, AlphaF(computeAlpha(t)));
}
