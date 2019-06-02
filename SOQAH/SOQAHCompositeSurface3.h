#pragma once

#include "SOQAHPatch3.h"
#include "../Core/Materials.h"

#include <vector>

namespace cagd
{

class SOQAHCompositeSurface3
{
public:
    enum class Direction
    {
        NORTH       = 0,
        NORTH_EAST  = 1,
        EAST        = 2,
        SOUTH_EAST  = 3,
        SOUTH       = 4,
        SOUTH_WEST  = 5,
        WEST        = 6,
        NORTH_WEST  = 7
    };

    struct PatchAttributes
    {
        SOQAHPatch3*                    _patch{new SOQAHPatch3()};

        RowMatrix<GenericCurve3*>*      _u_lines;
        RowMatrix<GenericCurve3*>*      _v_lines;

        TriangulatedMesh3*              _before_interpolation{};
        TriangulatedMesh3*              _after_interpolation{};

        PatchAttributes*                _north{};
        PatchAttributes*                _north_east{};
        PatchAttributes*                _east{};
        PatchAttributes*                _south_east{};
        PatchAttributes*                _south{};
        PatchAttributes*                _south_west{};
        PatchAttributes*                _west{};
        PatchAttributes*                _north_west{};

        GLboolean UpdatePatch
            (
            GLuint iso_line_count = 3,
            GLuint maximum_order_of_derivatives = 1,
            GLuint div_point_count = 30,
            GLenum usage_flag = GL_STATIC_DRAW
            );

        GLboolean RenderPatch();
    };

    SOQAHCompositeSurface3(GLuint patch_count = 500);

    PatchAttributes* AppendPatch();

    GLboolean UpdatePatches
        (
        GLuint iso_line_count = 3,
        GLuint maximum_order_of_derivatives = 1,
        GLuint div_point_count = 30,
        GLenum usage_flag = GL_STATIC_DRAW
        );

    GLboolean RenderPatches();

    size_t GetPatchCount() const;

    GLboolean GetPatchPoint(GLuint patch_index, GLuint point_ind_1, GLuint point_ind_2, DCoordinate3& point);

private:
    std::vector<PatchAttributes*>   _patches;
};

}
