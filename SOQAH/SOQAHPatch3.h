#pragma once

#include "../Core/TensorProductSurfaces3.h"
#include "./BlendingFunctionUtil.h"

namespace cagd
{
    class SOQAHPatch3: public TensorProductSurface3
    {
    public:
        SOQAHPatch3(GLdouble alpha = 1.0);

        GLboolean UBlendingFunctionValues(GLdouble u_knot, RowMatrix<GLdouble> &blending_values) const;
        GLboolean VBlendingFunctionValues(GLdouble u_knot, RowMatrix<GLdouble> &blending_values) const;
        GLboolean CalculatePartialDerivatives(GLuint max_order_of_derivatives, GLdouble u, GLdouble v, PartialDerivatives& partial_derivatives) const;

        void set_alpha(double alpha);
        GLdouble get_alpha();
    protected:
        GLdouble _alpha;
    private:
        BlendingFunctionUtil _blending_function_util;
    };
}
