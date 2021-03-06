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

        TriangulatedMesh3*              _image_of_patch{};

        PatchAttributes*                _north{};
        PatchAttributes*                _north_east{};
        PatchAttributes*                _east{};
        PatchAttributes*                _south_east{};
        PatchAttributes*                _south{};
        PatchAttributes*                _south_west{};
        PatchAttributes*                _west{};
        PatchAttributes*                _north_west{};

        GLuint                          _materialIndex{0};

        GLboolean UpdatePatch
            (
            GLuint iso_line_count = 3,
            GLuint maximum_order_of_derivatives = 1,
            GLuint div_point_count = 30,
            GLenum usage_flag = GL_STATIC_DRAW
            );

        GLboolean RenderPatch(GLboolean renderControlNet = GL_FALSE);
        void ApplyMaterial(GLuint materialIndex);
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

    void SetMaterialIndex(GLuint patchIndex, GLuint materialIndex);
    GLboolean RenderPatches(GLboolean renderControlNet = GL_FALSE);

    int GetPatchCount() const;

    GLboolean GetPatchPoint(GLuint patch_index, GLuint point_ind_1, GLuint point_ind_2, DCoordinate3& point);
    GLboolean SetPatchPoint(GLuint patch_index, GLuint point_ind_1, GLuint point_ind_2, const DCoordinate3& point);

    GLboolean JoinPatches(GLuint ind1, Direction dir1, GLuint ind2, Direction dir2);
    GLboolean ContinuePatch(GLuint ind, Direction dir);
    GLboolean MergePatches(GLuint ind1, Direction dir1, GLuint ind2, Direction dir2);

    GLboolean RefreshNeighbours(GLuint ind);
private:
    std::vector<PatchAttributes*>   _patches;
};

}
