#include "BlendingFunctionUtil.h"

using namespace std;

using namespace cagd;

BlendingFunctionUtil::BlendingFunctionUtil(GLdouble alpha)
{
    _alpha = alpha;
}

GLdouble BlendingFunctionUtil::blendingFunction00(GLdouble u) const
{
    return blendingFunction03(_alpha - u);
}

GLdouble BlendingFunctionUtil::blendingFunction01(GLdouble u) const
{
    return blendingFunction02(_alpha - u);
}

GLdouble BlendingFunctionUtil::blendingFunction02(GLdouble u) const
{
    // forward calculation in order to increas the speed of the formula's evaluation
    GLdouble a2 = _alpha * _alpha;
    GLdouble u2 = u * u;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);
    GLdouble shu = sinh(u);
    GLdouble chu = cosh(u);

    GLdouble h = getConstant2() * (a2 * chu + 2 * u2 * cha + a2 * cosh(_alpha - u) + 2 * u * _alpha - a2 - a2 * cha - 2 * _alpha * shu
                - 2 * _alpha * sinh(_alpha - u) + 2 * _alpha * sha - 2 * u2 + u * a2 * sha - u2 * _alpha * sha - 2 * u * _alpha * cha);

    GLdouble k = getConstant3() * (2 * (_alpha - u) + 2 * sinh(_alpha - u) + 2 * (shu - sha) + a2 * (shu - u) + u2 * (_alpha - sha)
                + 2 * (u * cha - _alpha * chu));

    return 0.5 * h + k;
}

GLdouble BlendingFunctionUtil::blendingFunction03(GLdouble u) const
{
    return getConstant4() * (2 * cosh(u) - u * u - 2);
}

// 1st order derivatives
GLdouble BlendingFunctionUtil::blendingFunction10(GLdouble u) const
{
    return -1.0 * blendingFunction13(_alpha - u);
}

GLdouble BlendingFunctionUtil::blendingFunction11(GLdouble u) const
{
    return -1.0 * blendingFunction12(_alpha - u);
}

GLdouble BlendingFunctionUtil::blendingFunction12(GLdouble u) const
{
    // forward calculation in order to increas the speed of the formula's evaluation
    GLdouble a2 = _alpha * _alpha;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);
    GLdouble shu = sinh(u);
    GLdouble chu = cosh(u);

    GLdouble k = getConstant3() * (-2 -2*cosh(_alpha - u) + 2*chu + a2*(chu - 1) + 2*u*(_alpha - sha) + 2*(cha - _alpha*(shu)));

    GLdouble h = getConstant2() * (a2*shu + a2*sha - a2*sinh(_alpha - u) + 2*_alpha + 2*_alpha*cosh(_alpha - u) - 2*_alpha*u*sha
                                   - 2*_alpha*chu - 2*_alpha*cha + 4*u*cha - 4*u);

    return 0.5 * h + k;
}

GLdouble BlendingFunctionUtil::blendingFunction13(GLdouble u) const
{
    return getConstant4() * (2 * sinh(u) - 2 * u);
}

// 2nd order derivatives
GLdouble BlendingFunctionUtil::blendingFunction20(GLdouble u) const
{
    return blendingFunction23(_alpha - u);
}

GLdouble BlendingFunctionUtil::blendingFunction21(GLdouble u) const
{
    return blendingFunction22(_alpha - u);
}

GLdouble BlendingFunctionUtil::blendingFunction22(GLdouble u) const
{
    // forward calculation in order to increas the speed of the formula's evaluation
    GLdouble a2 = _alpha * _alpha;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);
    GLdouble shu = sinh(u);
    GLdouble chu = cosh(u);

    GLdouble k = getConstant3() * (2*sinh(_alpha - u) + 2*shu + a2*shu - 2*_alpha*chu + 2*(_alpha - sha));

    GLdouble h = getConstant2() * (a2*chu + a2*cosh(_alpha - u) - 2*_alpha*shu - 2*_alpha*sinh(_alpha - u) - 2*_alpha*sha + 4*cha - 4);
    return 0.5 * h + k;
}

GLdouble BlendingFunctionUtil::blendingFunction23(GLdouble u) const
{
    return getConstant4() * (2 * cosh(u) - 2);
}

GLdouble BlendingFunctionUtil::getConstant2() const
{
    GLdouble a2 = _alpha * _alpha;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);

    GLdouble c2_s = (2 * _alpha * sha - 4 * cha + 4);
    GLdouble c2_gy_n = (4 * cha + a2+ a2* cha - 4 * _alpha * sha - 4);
    GLdouble c2 = c2_s / (c2_gy_n * c2_gy_n);

    return c2;
}

GLdouble BlendingFunctionUtil::getConstant3() const
{
    GLdouble a2 = _alpha * _alpha;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);

    GLdouble c3 = (2 * (sha - _alpha)) / ((4 * cha + a2 + a2 * cha - 4 * _alpha * sha - 4) * (a2 - 2 * cha + 2));

    return c3;
}

GLdouble BlendingFunctionUtil::getConstant4() const
{
    GLdouble c4 = 1.0 / (2 * cosh(_alpha) - _alpha * _alpha - 2);
    return c4;
}
