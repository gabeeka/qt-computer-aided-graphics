#pragma once

#include "../Core/LinearCombination3.h"


namespace cagd
{
    //! @brief Class definition for Second order/quartic algebraic-hyperbolic arc
    class SOQAHArcs3 : public LinearCombination3
    {
    public:
        // special constructor
        SOQAHArcs3(
                GLdouble alpha = 1.0,
                GLuint data_count = 4,
                GLenum data_usage_flag = GL_STATIC_DRAW);

        GLboolean BlendingFunctionValues(GLdouble u, RowMatrix<GLdouble> &values) const;
        GLboolean CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, Derivatives &d) const;

        // gette/setter for alpha
        GLdouble GetAlpha();
        GLboolean SetAlpha(GLdouble alpha);

    protected:
        GLdouble _alpha{0.0};
        GLuint   _data_count{4};

    private:
        GLdouble blendingFunction00(GLdouble u) const;
        GLdouble blendingFunction01(GLdouble u) const;
        GLdouble blendingFunction02(GLdouble u) const;
        GLdouble blendingFunction03(GLdouble u) const;
    };
}
