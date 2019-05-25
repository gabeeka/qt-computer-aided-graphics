#include "SOQAHArcs3.h"

using namespace cagd;


// special constructor
cagd::SOQAHArcs3::SOQAHArcs3::SOQAHArcs3(
        GLdouble u_min,
        GLdouble u_max,
        GLdouble alpha,
        GLuint data_count,
        GLenum data_usage_flag)
    : LinearCombination3(u_min, u_max, data_count, data_usage_flag)
    , _alpha(alpha)
{
}

GLdouble cagd::SOQAHArcs3::SOQAHArcs3::GetAlpha()
{
    return _alpha;
}

void cagd::SOQAHArcs3::SOQAHArcs3::SetAlpha(GLdouble alpha)
{
    if (alpha <= 0)
    {
        return GL_FALSE;
    }
    _alpha = alpha;
    return GL_TRUE;
}
