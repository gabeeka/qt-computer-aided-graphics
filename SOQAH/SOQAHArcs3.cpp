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
    values(0) = blendingFunction00(u);
    values(1) = blendingFunction01(u);
    values(2) = blendingFunction02(u);
    values(3) = blendingFunction03(u);
    return GL_TRUE;
}

GLboolean SOQAHArcs3::CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, LinearCombination3::Derivatives &d) const
{
    d.ResizeRows(max_order_of_derivatives + 1);
    d.LoadNullVectors();

    Matrix<GLdouble> dF(max_order_of_derivatives + 1, _data.GetRowCount());

    // We always evaluate the function's value (0th order derivative)
    dF(0, 0) = blendingFunction00(u);
    dF(0, 1) = blendingFunction01(u);
    dF(0, 2) = blendingFunction02(u);
    dF(0, 3) = blendingFunction03(u);

    // Evaluate the 1st order of derivatives if required
    if (max_order_of_derivatives >= 1)
    {
        dF(1, 0) = blendingFunction10(u);
        dF(1, 1) = blendingFunction11(u);
        dF(1, 2) = blendingFunction12(u);
        dF(1, 3) = blendingFunction13(u);
    }

    // Evaluate the 2nd order of derivatives if required
    if(max_order_of_derivatives >= 2)
    {
        dF(2, 0) = blendingFunction20(u);
        dF(2, 1) = blendingFunction21(u);
        dF(2, 2) = blendingFunction22(u);
        dF(2, 3) = blendingFunction23(u);
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

GLdouble SOQAHArcs3::blendingFunction00(GLdouble u) const
{
    return blendingFunction03(_alpha - u);
}

GLdouble SOQAHArcs3::blendingFunction01(GLdouble u) const
{
    return blendingFunction02(_alpha - u);
}

GLdouble SOQAHArcs3::blendingFunction02(GLdouble u) const
{
    // forward calculation in order to increas the speed of the formula's evaluation
    GLdouble a2 = _alpha * _alpha;
    GLdouble u2 = u * u;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);
    GLdouble shu = sinh(u);
    GLdouble chu = cosh(u);

    GLdouble c2_s = (2 * _alpha * sha - 4 * cha + 4);
    GLdouble c2_gy_n = (4 * cha + a2+ a2* cha - 4 * _alpha * sha - 4);
    GLdouble c2 = c2_s / (c2_gy_n * c2_gy_n);

    GLdouble c3 = (2 * (sha - _alpha)) / ((4 * cha + a2 + a2 * cha - 4 * _alpha * sha - 4) * (a2 - 2 * cha + 2));

    GLdouble h = c2 * (a2 * chu + 2 * u2 * cha + a2 * cosh(_alpha - u) + 2 * u * _alpha - a2 - a2 * cha - 2 * _alpha * shu
                - 2 * _alpha * sinh(_alpha - u) + 2 * _alpha * sha - 2 * u2 + u * a2 * sha - u2 * _alpha * sha - 2 * u * _alpha * cha);

    GLdouble k = c3 * (2 * (_alpha - u) + 2 * sinh(_alpha - u) + 2 * (shu - sha) + a2 * (shu - u) + u2 * (_alpha - sha)
                + 2 * (u * cha - _alpha * chu));

    return 0.5 * h + k;
}

GLdouble SOQAHArcs3::blendingFunction03(GLdouble u) const
{
    GLdouble c4 = 1.0 / (2 * cosh(_alpha) - _alpha * _alpha - 2);
    return c4 * (2 * cosh(u) - u * u - 2);
}

// 1st order derivatives
GLdouble SOQAHArcs3::blendingFunction10(GLdouble u) const
{
    return -1.0 * blendingFunction13(_alpha - u);
}

GLdouble SOQAHArcs3::blendingFunction11(GLdouble u) const
{
    return -1.0 * blendingFunction12(_alpha - u);
}

GLdouble SOQAHArcs3::blendingFunction12(GLdouble u) const
{
    // forward calculation in order to increas the speed of the formula's evaluation
    GLdouble a2 = _alpha * _alpha;
    GLdouble u2 = u * u;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);
    GLdouble shu = sinh(u);
    GLdouble chu = cosh(u);

    GLdouble c2_s = (2 * _alpha * sha - 4 * cha + 4);
    GLdouble c2_gy_n = (4 * cha + a2+ a2* cha - 4 * _alpha * sha - 4);
    GLdouble c2 = c2_s / (c2_gy_n * c2_gy_n);

    GLdouble c3 = (2 * (sha - _alpha)) / ((4 * cha + a2 + a2 * cha - 4 * _alpha * sha - 4) * (a2 - 2 * cha + 2));

    GLdouble k = c3 * (2 * u * (_alpha - sha) + 2 * cha + a2 * (chu - 1) + 2 * cha - 2);

    GLdouble h = c2 * (4 * u * cha - 4 * u + 2 * _alpha * cosh(_alpha - u) + a2 * shu
                 - a2 * sinh(_alpha - u) - 2 * _alpha * chu + a2 * sha + 2 * _alpha);

    return 0.5 * h + k;
}

GLdouble SOQAHArcs3::blendingFunction13(GLdouble u) const
{
    GLdouble c4 = 1.0 / (2 * cosh(_alpha) - _alpha * _alpha - 2);
    return c4 * (2 * sinh(u) - 2 * u);
}

// 2nd order derivatives
GLdouble SOQAHArcs3::blendingFunction20(GLdouble u) const
{
    return blendingFunction23(_alpha - u);
}

GLdouble SOQAHArcs3::blendingFunction21(GLdouble u) const
{
    return blendingFunction22(_alpha - u);
}

GLdouble SOQAHArcs3::blendingFunction22(GLdouble u) const
{
    // forward calculation in order to increas the speed of the formula's evaluation
    GLdouble a2 = _alpha * _alpha;
    GLdouble u2 = u * u;
    GLdouble sha = sinh(_alpha);
    GLdouble cha = cosh(_alpha);
    GLdouble shu = sinh(u);
    GLdouble chu = cosh(u);

    GLdouble c2_s = (2 * _alpha * sha - 4 * cha + 4);
    GLdouble c2_gy_n = (4 * cha + a2+ a2* cha - 4 * _alpha * sha - 4);
    GLdouble c2 = c2_s / (c2_gy_n * c2_gy_n);

    GLdouble c3 = (2 * (sha - _alpha)) / ((4 * cha + a2 + a2 * cha - 4 * _alpha * sha - 4) * (a2 - 2 * cha + 2));

    GLdouble k = c3 * (2 * shu + a2 * shu + 2 * (_alpha - sha));

    GLdouble h = c2 * (a2 * cosh(_alpha - u) + a2 * chu - 2 * _alpha * sinh(_alpha - u) - 2 * _alpha * shu + 4 * cha - 4);

    return 0.5 * h + k;
}

GLdouble SOQAHArcs3::blendingFunction23(GLdouble u) const
{
    GLdouble c4 = 1.0 / (2 * cosh(_alpha) - _alpha * _alpha - 2);
    return c4 * (2 * cosh(u) - 2);
}
