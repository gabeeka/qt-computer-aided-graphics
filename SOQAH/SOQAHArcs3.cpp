#include "SOQAHArcs3.h"

using namespace cagd;


// special constructor
SOQAHArcs3::SOQAHArcs3(
        GLdouble u_min,
        GLdouble u_max,
        GLdouble alpha,
        GLuint data_count,
        GLenum data_usage_flag)
    : LinearCombination3(u_min, u_max, data_count, data_usage_flag)
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
    values(0) = blendingFunction01(u);
    values(0) = blendingFunction02(u);
    values(0) = blendingFunction03(u);
    return GL_TRUE;
}

GLboolean SOQAHArcs3::CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, LinearCombination3::Derivatives &d) const
{
    d.ResizeRows(max_order_of_derivatives + 1);
    d.LoadNullVectors();

    Matrix<GLdouble> dF(max_order_of_derivatives + 1, _data.GetRowCount());

    if (max_order_of_derivatives == 0)
    {
        dF(0, 0) = blendingFunction00(u);
        dF(0, 1) = blendingFunction01(u);
        dF(0, 2) = blendingFunction02(u);
        dF(0, 3) = blendingFunction03(u);
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

    GLdouble k = c3 * (2 * (_alpha - u) + 2 * sinh(_alpha - u) + (2 * shu - sha) + a2 * (shu - u) + u2 * (_alpha - sha)
                + 2 * (u * cha - _alpha * chu));

    return 0.5 * h + k;
}

GLdouble SOQAHArcs3::blendingFunction03(GLdouble u) const
{
    GLdouble c4 = 1 / (2 * cosh(_alpha) - _alpha * _alpha - 2);
    return c4 * (2 * cosh(u) - u * u - 2);
}
