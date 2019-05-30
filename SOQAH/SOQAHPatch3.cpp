#include "SOQAHPatch3.h"
#include <iostream>

using namespace std;
using namespace cagd;

SOQAHPatch3::SOQAHPatch3(GLdouble alpha)
    : TensorProductSurface3(0.0, alpha, 0.0, alpha, 4, 4)
    , _alpha(alpha)
{
}

GLboolean SOQAHPatch3::UBlendingFunctionValues(GLdouble u_knot, RowMatrix<GLdouble> &blending_values) const
{
    if(u_knot < 0.0 || u_knot > _alpha)
    {
        return GL_FALSE;
    }
    blending_values.ResizeColumns(4);

    blending_values(0) = blendingFunction00(u_knot);
    blending_values(1) = blendingFunction01(u_knot);
    blending_values(2) = blendingFunction02(u_knot);
    blending_values(3) = blendingFunction03(u_knot);

    return GL_TRUE;
}

GLboolean SOQAHPatch3::VBlendingFunctionValues(GLdouble v_knot, RowMatrix<GLdouble> &blending_values) const
{
    if(v_knot < 0.0 || v_knot > _alpha)
    {
        return GL_FALSE;
    }
    blending_values.ResizeColumns(4);

    blending_values(0) = blendingFunction00(v_knot);
    blending_values(1) = blendingFunction01(v_knot);
    blending_values(2) = blendingFunction02(v_knot);
    blending_values(3) = blendingFunction03(v_knot);

    return GL_TRUE;
}

GLboolean SOQAHPatch3::CalculatePartialDerivatives
    (
    GLuint max_order_of_derivatives,
    GLdouble u,
    GLdouble v,
    PartialDerivatives &partial_derivatives
    ) const
{
    if(u < 0.0 || u > _alpha || v < 0.0 || v > _alpha || max_order_of_derivatives > 1)
    {
        return GL_FALSE;
    }

    RowMatrix<GLdouble> u_blending_values_0(4), u_blending_values_1(4);
    u_blending_values_0(0) = blendingFunction00(u);
    u_blending_values_0(1) = blendingFunction01(u);
    u_blending_values_0(2) = blendingFunction02(u);
    u_blending_values_0(3) = blendingFunction03(u);

    u_blending_values_1(0) = blendingFunction10(u);
    u_blending_values_1(1) = blendingFunction11(u);
    u_blending_values_1(2) = blendingFunction12(u);
    u_blending_values_1(3) = blendingFunction13(u);

    RowMatrix<GLdouble> v_blending_values_0(4), v_blending_values_1(4);
    v_blending_values_0(0) = blendingFunction00(v);
    v_blending_values_0(1) = blendingFunction01(v);
    v_blending_values_0(2) = blendingFunction02(v);
    v_blending_values_0(3) = blendingFunction03(v);

    v_blending_values_1(0) = blendingFunction10(v);
    v_blending_values_1(1) = blendingFunction11(v);
    v_blending_values_1(2) = blendingFunction12(v);
    v_blending_values_1(3) = blendingFunction13(v);

    partial_derivatives.ResizeRows(2);
    partial_derivatives.LoadNullVectors();

    for(GLuint row = 0; row < 4; ++row)
    {
        DCoordinate3 aux_d0_v, aux_d1_v;
        for(GLuint column = 0; column < 4; ++column)
        {
            aux_d0_v += _data(row, column) * v_blending_values_0(column);
            aux_d1_v += _data(row, column) * v_blending_values_1(column);
        }
        partial_derivatives(0, 0) += aux_d0_v * u_blending_values_0(row);
        partial_derivatives(1, 0) += aux_d0_v * u_blending_values_1(row);
        partial_derivatives(1, 1) += aux_d1_v * u_blending_values_0(row);
    }

    return GL_TRUE;
}

void SOQAHPatch3::set_alpha(double alpha)
{
    _alpha = alpha;
    SetUInterval(0, _alpha);
    SetVInterval(0, _alpha);
}

GLdouble SOQAHPatch3::get_alpha()
{
    return _alpha;
}

GLdouble SOQAHPatch3::blendingFunction00(GLdouble u) const
{
    return blendingFunction03(_alpha - u);
}

GLdouble SOQAHPatch3::blendingFunction01(GLdouble u) const
{
    return blendingFunction02(_alpha - u);
}

GLdouble SOQAHPatch3::blendingFunction02(GLdouble u) const
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

GLdouble SOQAHPatch3::blendingFunction03(GLdouble u) const
{
    return getConstant4() * (2 * cosh(u) - u * u - 2);
}

// 1st order derivatives
GLdouble SOQAHPatch3::blendingFunction10(GLdouble u) const
{
    return -1.0 * blendingFunction13(_alpha - u);
}

GLdouble SOQAHPatch3::blendingFunction11(GLdouble u) const
{
    return -1.0 * blendingFunction12(_alpha - u);
}

GLdouble SOQAHPatch3::blendingFunction12(GLdouble u) const
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

GLdouble SOQAHPatch3::blendingFunction13(GLdouble u) const
{
    return getConstant4() * (2 * sinh(u) - 2 * u);
}

// 2nd order derivatives
GLdouble SOQAHPatch3::blendingFunction20(GLdouble u) const
{
    return blendingFunction23(_alpha - u);
}

GLdouble SOQAHPatch3::blendingFunction21(GLdouble u) const
{
    return blendingFunction22(_alpha - u);
}

GLdouble SOQAHPatch3::blendingFunction22(GLdouble u) const
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

GLdouble SOQAHPatch3::blendingFunction23(GLdouble u) const
{
    return getConstant4() * (2 * cosh(u) - 2);
}

// TODO: move these methods to a common class to be used by SOQAHArcs3 and SOQAHPatch3
GLdouble SOQAHPatch3::getConstant2() const
{
    GLdouble a2 = _alpha * _alpha;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);

    GLdouble c2_s = (2 * _alpha * sha - 4 * cha + 4);
    GLdouble c2_gy_n = (4 * cha + a2+ a2* cha - 4 * _alpha * sha - 4);
    GLdouble c2 = c2_s / (c2_gy_n * c2_gy_n);

    return c2;
}

GLdouble SOQAHPatch3::getConstant3() const
{
    GLdouble a2 = _alpha * _alpha;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);

    GLdouble c3 = (2 * (sha - _alpha)) / ((4 * cha + a2 + a2 * cha - 4 * _alpha * sha - 4) * (a2 - 2 * cha + 2));

    return c3;
}

GLdouble SOQAHPatch3::getConstant4() const
{
    GLdouble c4 = 1.0 / (2 * cosh(_alpha) - _alpha * _alpha - 2);
    return c4;
}

