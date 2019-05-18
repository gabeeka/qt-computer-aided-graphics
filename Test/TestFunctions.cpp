#include <cmath>
#include "TestFunctions.h"
#include "../Core/Constants.h"

using namespace cagd;
using namespace std;

GLdouble spiral_on_cone::u_min = -6.2831;
GLdouble spiral_on_cone::u_max = +6.2831;

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

GLdouble torus::u_min = 0.0;
GLdouble torus::u_max = 18.8493;

DCoordinate3 torus::d0(GLdouble u)
{
    return DCoordinate3((2+cos(2*u/3))* cos(u), (2+cos(2*u/3))*sin(u), sin(2*u/3));
}

DCoordinate3 torus::d1(GLdouble u)
{
    return DCoordinate3(-(2+cos(2*u/3))*sin(u)-2.0/3*sin(2*u/3)*cos(u), (2+cos(2*u/3))*cos(u)-2.0/3*sin(2*u/3)*sin(u), 2.0/3*cos(2*u/3));
}

GLdouble ellipse::u_min = -2*PI;
GLdouble ellipse::u_max = 2*PI;

DCoordinate3 ellipse::d0(GLdouble u)
{
    return DCoordinate3(6 * cos(u), 4* sin(u), 0);
}

GLdouble hypo::u_min = -3.0;
GLdouble hypo::u_max = +3.0;

DCoordinate3 hypo::d0(GLdouble u)
{
    return DCoordinate3(5*cos(u)+cos(5*u), 5*sin(u)-sin(5*u), 0);
}

DCoordinate3 hypo::d1(GLdouble u)
{
    return DCoordinate3(-5*sin(u)-5*sin(5*u), 5*cos(u)-5*cos(5*u), 0);
}

DCoordinate3 hypo::d2(GLdouble u)
{
    return DCoordinate3(-5*cos(u)-25*cos(5*u), -5*sin(u)+25*sin(5*u), 0);
}

GLdouble lissajou::u_min = -1.0;
GLdouble lissajou::u_max = +1.0;

DCoordinate3 lissajou::d0(GLdouble u)
{
    return DCoordinate3(sin(5*u + 1.5707), sin(4*u), 0);
}

DCoordinate3 lissajou::d1(GLdouble u)
{
    return DCoordinate3(5*cos(5*u + 1.5707), 4*cos(4*u), 0);
}

DCoordinate3 lissajou::d2(GLdouble u)
{
    return DCoordinate3(-25*sin(5*u + 1.5707), -16*sin(4*u), 0);
}


GLdouble sphere::u_min=0;
GLdouble sphere::u_max=PI;
GLdouble sphere::v_min=0;
GLdouble sphere::v_max=TWO_PI;
GLdouble sphere::r=1.0;

DCoordinate3 sphere::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3(r*sin(u)*cos(v),r*sin(u)*sin(v),r*cos(u));
}

DCoordinate3 sphere::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(r*cos(u)*cos(v),r*cos(u)*sin(v),-(r*sin(u)));
}

DCoordinate3 sphere::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(-r*sin(u)*sin(v),r*sin(u)*cos(v),0);
}


GLdouble chelicoid::u_min = 0;
GLdouble chelicoid::u_max = 2;
GLdouble chelicoid::v_min = 0;
GLdouble chelicoid::v_max = 3*TWO_PI;

DCoordinate3 chelicoid::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3((2+u)*cos(v),(2+u)*sin(v),v);
}

DCoordinate3 chelicoid::d10(GLdouble /* u */, GLdouble v)
{
    return DCoordinate3(cos(v),sin(v),v);
}

DCoordinate3 chelicoid::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(-(2+u)*sin(v),(2+u)*cos(v),1);
}

GLdouble hyperboloid::u_min=0;
GLdouble hyperboloid::u_max=3;
GLdouble hyperboloid::v_min=0;
GLdouble hyperboloid::v_max=TWO_PI;

DCoordinate3 hyperboloid::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3((1+cosh(u-3.0/2.0))*sin(v),(1+cosh(u-3.0/2.0))*cos(v),sinh(u-3.0/2.0));
}

DCoordinate3 hyperboloid::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(sinh(u-3.0/2.0)*sin(v),sinh(u-3.0/2.0)*cos(v),cosh(u-3.0/2.0));
}

DCoordinate3 hyperboloid::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3((1+cosh(u-3.0/2.0))*cos(v),-(1+cosh(u-3.0/2.0))*sin(v),0.0);
}


GLdouble kleinbottle::u_min=0;
GLdouble kleinbottle::u_max=TWO_PI;
GLdouble kleinbottle::v_min=0;
GLdouble kleinbottle::v_max=TWO_PI;

DCoordinate3 kleinbottle::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3(3*cos(u)+0.5*(1+cos(2*u))*sin(v)-0.5*sin(2*u)*sin(2*v),
                        3*sin(u)+0.5*sin(2*u)*sin(v)-0.5*(1-cos(2*u))*sin(2*v),
                        cos(u)*sin(2*v)+sin(u)*sin(v));
}

DCoordinate3 kleinbottle::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(-3*sin(u)-sin(2*u)*sin(v)-cos(2*u)*sin(2*v),
                        3*cos(u)+cos(2*u)*sin(v)-sin(2*u)*sin(2*v),
                        -sin(u)*sin(2*v)+cos(u)*sin(v));
}

DCoordinate3 kleinbottle::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(-0.5*(1+cos(2*u))*cos(v)-sin(2*u)*cos(2*v),
                        0.5*sin(2*u)*cos(v)-(1-cos(2*u))*cos(2*v),
                        -2*cos(u)*cos(2*v)+sin(u)*cos(v));

}

GLdouble dupincyclide::u_min=0;
GLdouble dupincyclide::u_max=TWO_PI;
GLdouble dupincyclide::v_min=0;
GLdouble dupincyclide::v_max=TWO_PI;

DCoordinate3 dupincyclide::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3((3*(2-6*cos(u)*cos(v))+32*cos(u))/(6-2*cos(u)*cos(v)),
                        (4*sqrt(2)*(sin(u)*(6-3*cos(v))))/(6-2*cos(u)*cos(v)),
                        (4*sqrt(2)*sin(v)*(2*cos(u)-3))/(6-2*cos(u)*cos(v)));
}

DCoordinate3 dupincyclide::d10(GLdouble u, GLdouble v)
{
    GLdouble twoPower = 2.82842712475;
    return DCoordinate3((24*(cos(v)-2)*sin(u))/((cos(v)*cos(u)-3)*(cos(v)*cos(u)-3)),
                        (3*twoPower*(cos(v)-2)*(cos(v)*sin(u)*sin(u)+cos(v)*cos(u)*cos(u)-3*cos(u)))/((cos(v)*cos(u)-3)*(cos(v)*cos(u)-3)),
                        (3*twoPower*(cos(v)-2)*sin(v)*sin(u))/((cos(v)*cos(u)-3)*(cos(v)*cos(u)-3)));
}

DCoordinate3 dupincyclide::d01(GLdouble u, GLdouble v)
{
    GLdouble twoPower = 2.82842712475;
    return DCoordinate3(-(8*cos(u)*(2*cos(u)-3)*sin(v))/((cos(u)*cos(v)-3)*(cos(u)*cos(v)-3)),
                        -(3*twoPower*(2*cos(u)-3)*sin(u)*sin(v))/((cos(u)*cos(v)-3)*(cos(u)*cos(v)-3)),
                        -(twoPower*(2*cos(u)-3)*(cos(u)*sin(v)*sin(v)+cos(u)*cos(v)*cos(v)-3*cos(v)))/((cos(u)*cos(v)-3)*(cos(u)*cos(v)-3)));
}
