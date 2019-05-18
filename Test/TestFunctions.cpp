#include <cmath>
#include "TestFunctions.h"
#include "../Core/Constants.h"

using namespace cagd;
using namespace std;

GLdouble spiral_on_cone::u_min = -TWO_PI;
GLdouble spiral_on_cone::u_max = +TWO_PI;

DCoordinate3 spiral_on_cone::d0(GLdouble u)
{
    return DCoordinate3(u * cos(u), u * sin(u), u);
}

DCoordinate3 spiral_on_cone::d1(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    return DCoordinate3(c - u * s, s + u * c, 1.0);
}

DCoordinate3 spiral_on_cone::d2(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    return DCoordinate3(-2.0 * s - u * c, 2.0 * c - u * s, 0);
}


// torus knot
GLdouble torus_knot::u_min = 0;
GLdouble torus_knot::u_max = 6 * PI;

DCoordinate3 torus_knot::d0(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    GLdouble c2 = cos(2*u/3), s2 = sin(2*u/3);
    return DCoordinate3((2 + c2) * c, (2 + c2) * s, s2);
}

DCoordinate3 torus_knot::d1(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    GLdouble c2 = cos(2*u/3), s2 = sin(2*u/3);

    GLdouble x = (-(c2 + 2) * s - (2 * s2 * c / 3));
    GLdouble y = (-(c2 + 2) * c - (2 * s2 * s / 3));
    GLdouble z = 2 * c2 / 3;

    return DCoordinate3(x, y, z);
}

DCoordinate3 torus_knot::d2(GLdouble u)
{
    GLdouble x = -(25*cos(5*u/3) + 36*cos(u) + cos(u/3)) / 18;;
    GLdouble y = -(25*sin(5*u/3) + 36*sin(u) + sin(u/3)) / 18;
    GLdouble z = -4 * sin(2*u/3) / 9;

    return DCoordinate3(x, y, z);
}


// lissajous
GLdouble lissajous::u_min = 0;
GLdouble lissajous::u_max = PI/4;

DCoordinate3 lissajous::d0(GLdouble u)
{
    return DCoordinate3(2*cos(10.0*u), 2*cos(3.0*10*u+2.3), 2*cos(2.0*10*u+2.3));
}

DCoordinate3 lissajous::d1(GLdouble u)
{
    return DCoordinate3(-(5*sin(5.0*u)+4.0*sin(4.0*u))/2.0, (5.0*cos(5.0*u)+cos(u))/2.0, 2.0*cos(2.0*u));
}

DCoordinate3 lissajous::d2(GLdouble u)
{
    return DCoordinate3(-(25*cos(5*u)+16*cos(4*u))/2.0, -(25*sin(5*u)+sin(u))/2.0, -4*sin(2*u));
}


// rose
GLdouble rose::u_min = 0;
GLdouble rose::u_max = TWO_PI;

DCoordinate3 rose::d0(GLdouble u)
{
    return DCoordinate3(cos(u)*cos(2*u), sin(u)*sin(2*u), cos(2*u)*cos(2*u));
}

DCoordinate3 rose::d1(GLdouble u)
{
    return DCoordinate3(u,u,u);
}

DCoordinate3 rose::d2(GLdouble u)
{
    return DCoordinate3(u,u,u);
}
