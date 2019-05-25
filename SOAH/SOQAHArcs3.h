#pragma once

#include "../Core/LinearCombination3.h"


namespace cagd
{
    class SOQAHArcs3 : public LinearCombination3
    {
    public:
        // special constructor
        SOQAHArcs3::SOQAHArcs3(
                GLdouble u_min,
                GLdouble u_max,
                GLdouble alpha,
                GLuint data_count = 4,
                GLenum data_usage_flag = GL_STATIC_DRAW);

        // gette/setter for alpha
        GLdouble SOQAHArcs3::GetAlpha();
        void SOQAHArcs3::SetAlpha(GLdouble alpha);

    protected:
        GLdouble _alpha{0.0};
    };
}
