#pragma once

#include "../Core/TensorProductSurfaces3.h"

namespace cagd
{
    class SOQAHPatch3: public TensorProductSurface3
    {
    protected:
        GLdouble _alpha;
    public:
        SOQAHPatch3(GLdouble alpha = 1.0);

        GLboolean UBlendingFunctionValues(GLdouble u_knot, RowMatrix<GLdouble> &blending_values) const;
        GLboolean VBlendingFunctionValues(GLdouble u_knot, RowMatrix<GLdouble> &blending_values) const;
        GLboolean CalculatePartialDerivatives(GLuint max_order_of_derivatives, GLdouble u, GLdouble v, PartialDerivatives& partial_derivatives) const;

        void set_alpha(double alpha);
        GLdouble get_alpha();
    private:
        GLdouble getConstant2() const;
        GLdouble getConstant3() const;
        GLdouble getConstant4() const;

        GLdouble blendingFunction00(GLdouble u) const;
        GLdouble blendingFunction01(GLdouble u) const;
        GLdouble blendingFunction02(GLdouble u) const;
        GLdouble blendingFunction03(GLdouble u) const;

        GLdouble blendingFunction10(GLdouble u) const;
        GLdouble blendingFunction11(GLdouble u) const;
        GLdouble blendingFunction12(GLdouble u) const;
        GLdouble blendingFunction13(GLdouble u) const;

        GLdouble blendingFunction20(GLdouble u) const;
        GLdouble blendingFunction21(GLdouble u) const;
        GLdouble blendingFunction22(GLdouble u) const;
        GLdouble blendingFunction23(GLdouble u) const;
    };
}
