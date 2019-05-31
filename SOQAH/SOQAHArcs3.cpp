#include "SOQAHArcs3.h"

#include <iostream>
using namespace std;

using namespace cagd;


// special constructor
SOQAHArcs3::SOQAHArcs3(
        GLdouble alpha,
        GLuint data_count,
        GLenum data_usage_flag)
    : LinearCombination3(0.0, alpha, data_count, data_usage_flag)
    , _alpha(alpha)
    , _data_count(data_count)
{
}

GLboolean SOQAHArcs3::BlendingFunctionValues(GLdouble u, RowMatrix<GLdouble> &values) const
{
    values.ResizeColumns(_data_count);
    if (u < 0 || u > _alpha)
    {
        return GL_FALSE;
    }
    values(0) = _blending_function_util.blendingFunction00(u);
    values(1) = _blending_function_util.blendingFunction01(u);
    values(2) = _blending_function_util.blendingFunction02(u);
    values(3) = _blending_function_util.blendingFunction03(u);
    return GL_TRUE;
}

GLboolean SOQAHArcs3::CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, LinearCombination3::Derivatives &d) const
{
    d.ResizeRows(max_order_of_derivatives + 1);
    d.LoadNullVectors();

    Matrix<GLdouble> dF(max_order_of_derivatives + 1, _data.GetRowCount());

    // We always evaluate the function's value (0th order derivative)
    dF(0, 0) = _blending_function_util.blendingFunction00(u);
    dF(0, 1) = _blending_function_util.blendingFunction01(u);
    dF(0, 2) = _blending_function_util.blendingFunction02(u);
    dF(0, 3) = _blending_function_util.blendingFunction03(u);

    // Evaluate the 1st order of derivatives if required
    if (max_order_of_derivatives >= 1)
    {
        dF(1, 0) = _blending_function_util.blendingFunction10(u);
        dF(1, 1) = _blending_function_util.blendingFunction11(u);
        dF(1, 2) = _blending_function_util.blendingFunction12(u);
        dF(1, 3) = _blending_function_util.blendingFunction13(u);
    }

    // Evaluate the 2nd order of derivatives if required
    if(max_order_of_derivatives >= 2)
    {
        dF(2, 0) = _blending_function_util.blendingFunction20(u);
        dF(2, 1) = _blending_function_util.blendingFunction21(u);
        dF(2, 2) = _blending_function_util.blendingFunction22(u);
        dF(2, 3) = _blending_function_util.blendingFunction23(u);
    }


    for (GLuint order = 0; order <= max_order_of_derivatives; order++)
    {
        for (GLuint i = 0; i <= 3; i++)
        {
            d[order] += _data[i] * dF(order, i);
        }
    }

    return GL_TRUE;
}


// gette/setter for alpha
GLdouble SOQAHArcs3::GetAlpha()
{
    return _alpha;
}

GLboolean SOQAHArcs3::SetAlpha(GLdouble alpha)
{
    if (alpha <= 0)
    {
        return GL_FALSE;
    }
    _alpha = alpha;
    return GL_TRUE;
}
