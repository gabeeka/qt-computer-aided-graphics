#pragma once

#include "../Core/LinearCombination3.h"

namespace cagd
{
    class BlendingFunctionUtil
    {
    public:
        // special constructor
        BlendingFunctionUtil(GLdouble alpha = 1.0);

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
    protected:
        GLdouble _alpha{0.0};
    };
}
