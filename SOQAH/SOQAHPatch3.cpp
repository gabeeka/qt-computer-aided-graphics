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

    blending_values(0) = _blending_function_util.blendingFunction00(u_knot);
    blending_values(1) = _blending_function_util.blendingFunction01(u_knot);
    blending_values(2) = _blending_function_util.blendingFunction02(u_knot);
    blending_values(3) = _blending_function_util.blendingFunction03(u_knot);

    return GL_TRUE;
}

GLboolean SOQAHPatch3::VBlendingFunctionValues(GLdouble v_knot, RowMatrix<GLdouble> &blending_values) const
{
    if(v_knot < 0.0 || v_knot > _alpha)
    {
        return GL_FALSE;
    }
    blending_values.ResizeColumns(4);

    blending_values(0) = _blending_function_util.blendingFunction00(v_knot);
    blending_values(1) = _blending_function_util.blendingFunction01(v_knot);
    blending_values(2) = _blending_function_util.blendingFunction02(v_knot);
    blending_values(3) = _blending_function_util.blendingFunction03(v_knot);

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
    u_blending_values_0(0) = _blending_function_util.blendingFunction00(u);
    u_blending_values_0(1) = _blending_function_util.blendingFunction01(u);
    u_blending_values_0(2) = _blending_function_util.blendingFunction02(u);
    u_blending_values_0(3) = _blending_function_util.blendingFunction03(u);

    u_blending_values_1(0) = _blending_function_util.blendingFunction10(u);
    u_blending_values_1(1) = _blending_function_util.blendingFunction11(u);
    u_blending_values_1(2) = _blending_function_util.blendingFunction12(u);
    u_blending_values_1(3) = _blending_function_util.blendingFunction13(u);

    RowMatrix<GLdouble> v_blending_values_0(4), v_blending_values_1(4);
    v_blending_values_0(0) = _blending_function_util.blendingFunction00(v);
    v_blending_values_0(1) = _blending_function_util.blendingFunction01(v);
    v_blending_values_0(2) = _blending_function_util.blendingFunction02(v);
    v_blending_values_0(3) =_blending_function_util.blendingFunction03(v);

    v_blending_values_1(0) = _blending_function_util.blendingFunction10(v);
    v_blending_values_1(1) = _blending_function_util.blendingFunction11(v);
    v_blending_values_1(2) = _blending_function_util.blendingFunction12(v);
    v_blending_values_1(3) = _blending_function_util.blendingFunction13(v);

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
