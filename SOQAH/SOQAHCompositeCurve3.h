#pragma once

#include "./SOQAHArcs3.h"
#include "../Core/Colors4.h"

#include <vector>

namespace cagd
{

class SOQAHCompositeCurve3
{
public:
    enum class Direction
    {
        LEFT  = 0,
        RIGHT = 1
    };

    struct ArcAttributes
    {
        SOQAHArcs3*         _arc{new SOQAHArcs3()};
        Color4              _arc_color{};

        GenericCurve3*      _img{};
        Color4              _img_color_0{};
        Color4              _img_color_1{};
        Color4              _img_color_2{};

        ArcAttributes*      _left{};
        ArcAttributes*      _right{};

        GLboolean GenerateImage(GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag = GL_STATIC_DRAW);
    };

    SOQAHCompositeCurve3() = delete;
    SOQAHCompositeCurve3(const SOQAHCompositeCurve3& rhs) = delete;

    SOQAHCompositeCurve3(GLuint arcCount = 500);

    // Returns a pointer to the newly added arc
    ArcAttributes* AppendArc();

    DCoordinate3& GetArcPoint(GLuint arc_index, GLuint point_ind);
    DCoordinate3 GetArcPoint(GLuint arc_index, GLuint point_ind) const;

    size_t GetArcCount() const;

    GLboolean UpdateVBODatas(GLenum usage_flag = GL_STATIC_DRAW);
    GLboolean GenerateImages(GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag = GL_STATIC_DRAW);
    GLboolean UpdateVBOs(GLenum usage_flag = GL_STATIC_DRAW);

    GLboolean Render(GLuint order, GLboolean renderControlPoints = GL_FALSE) const;

    GLboolean JoinArcs(GLuint ind1, Direction dir1, GLuint ind2, Direction dir2);
    void RefreshJoins(ArcAttributes* arc_left, ArcAttributes* arc_between, ArcAttributes* arc_right);

private:
    std::vector<ArcAttributes*>  _arcs;
};

}
