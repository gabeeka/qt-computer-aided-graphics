#include "SOQAHCompositeSurface3.h"

using namespace cagd;

GLboolean SOQAHCompositeSurface3::PatchAttributes::UpdatePatch
    (
    GLuint iso_line_count,
    GLuint maximum_order_of_derivatives,
    GLuint div_point_count,
    GLenum usage_flag
    )
{
    _u_lines = _patch->GenerateUIsoparametricLines(iso_line_count, maximum_order_of_derivatives, div_point_count);
    _v_lines = _patch->GenerateVIsoparametricLines(iso_line_count, maximum_order_of_derivatives, div_point_count);

    GLboolean ok = GL_TRUE;

    // Update VBOs for iso parametric lines
    for(GLuint i = 0; i < _u_lines->GetColumnCount(); i++)
    {
        if((*_u_lines)[i])
        {
            ok = ok && (*_u_lines)[i]->UpdateVertexBufferObjects(1.0, usage_flag);
        }
    }
    if (!ok) throw std::runtime_error("Failed to update VBOs for U lines!");

    for(GLuint i = 0; i < _v_lines->GetColumnCount(); i++)
    {
        if((*_v_lines)[i])
        {
            ok = ok && (*_v_lines)[i]->UpdateVertexBufferObjects(1.0, usage_flag);
        }
    }
    if (!ok) throw std::runtime_error("Failed to update VBOs for V lines!");

    // Generate the mesh (image) of the surface patch
    ok = ok && (_before_interpolation = _patch->GenerateImage(30, 30, GL_STATIC_DRAW));
    if (!ok) throw std::runtime_error("Failed to generate image of patch!");
    // Update the VBOs of the image of patch
    ok = ok && (_before_interpolation->UpdateVertexBufferObjects(usage_flag));
    if (!ok) throw std::runtime_error("Failed to updathe the VBOs of the image of patch!");

    // define an interpolation problem:
    // 1: create a knot vector in u−direction
    RowMatrix<GLdouble> u_knot_vector(4);
    u_knot_vector(0) = 0.0;
    u_knot_vector(1) = 1.0 / 3.0;
    u_knot_vector(2) = 2.0 / 3.0;
    u_knot_vector(3) = 1.0;

    // 2: create a knot vector in v−direction
    ColumnMatrix<GLdouble> v_knot_vector(4);
    v_knot_vector(0) = 0.0;
    v_knot_vector(1) = 1.0 / 3.0;
    v_knot_vector(2) = 2.0 / 3.0;
    v_knot_vector(3) = 1.0;

    // 3: define a matrix of datapoints, e.g. set them to the original controlpoints
    Matrix<DCoordinate3> data_points_to_interpolate(4, 4);
    for(GLuint row = 0; row < 4; ++row)
    {
        for(GLuint column = 0; column < 4; ++column)
        {
            _patch->GetData(row, column, data_points_to_interpolate(row, column));
        }
    }

    // 4: solve the interpolation problem and generate the mesh of the interpolating patch
    ok = ok && (_patch->UpdateDataForInterpolation(u_knot_vector, v_knot_vector, data_points_to_interpolate));
    if (!ok) throw std::runtime_error("Failed to update data for interpolation!");

    ok = ok && (_after_interpolation = _patch->GenerateImage(div_point_count, div_point_count, usage_flag));
    if (!ok) throw std::runtime_error("Failed to generate image of patch after interpolation!");

    ok = ok && _after_interpolation->UpdateVertexBufferObjects(usage_flag);
    if (!ok) throw std::runtime_error("Failed to update VBOS after interpolation!");

    return ok;
}

GLboolean SOQAHCompositeSurface3::PatchAttributes::RenderPatch()
{
    GLboolean ok = GL_TRUE;

    MatFBRuby.Apply();
    ok = ok && _before_interpolation->Render();
    if (!ok) throw std::runtime_error("Failed to render the image of patch!");

    if(_after_interpolation)
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);

        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        MatFBTurquoise.Apply();
        ok = ok && _after_interpolation->Render();
        if (!ok) throw std::runtime_error("Failed to render the interpolation of patch!");
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    for(GLuint i = 0; i < _u_lines->GetColumnCount(); i++)
    {
        if((*_u_lines)[i])
        {
            ok = ok && (*_u_lines)[i]->RenderDerivatives(0, GL_LINE_STRIP);
        }
    }
    if (!ok) throw std::runtime_error("Failed to render U lines 0 derivatives!");

    for(GLuint i = 0; i < _u_lines->GetColumnCount(); i++)
    {
        if((*_u_lines)[i])
        {
            ok = ok && (*_u_lines)[i]->RenderDerivatives(1, GL_LINES);
        }
    }
    if (!ok) throw std::runtime_error("Failed to render U lines 1st derivatives!");

    for(GLuint i = 0; i < _v_lines->GetColumnCount(); i++)
    {
        if((*_v_lines)[i])
        {
            ok = ok && (*_v_lines)[i]->RenderDerivatives(0, GL_LINE_STRIP);
        }
    }
    if (!ok) throw std::runtime_error("Failed to render V lines 0 derivatives!");

    return ok;
}

SOQAHCompositeSurface3::SOQAHCompositeSurface3(GLuint patch_count)
{
    _patches.reserve(patch_count);
}

SOQAHCompositeSurface3::PatchAttributes* SOQAHCompositeSurface3::AppendPatch()
{
    _patches.push_back(new SOQAHCompositeSurface3::PatchAttributes());
    return _patches[_patches.size() - 1];
}

GLboolean SOQAHCompositeSurface3::UpdatePatches(GLuint iso_line_count, GLuint maximum_order_of_derivatives, GLuint div_point_count, GLenum usage_flag)
{
    GLboolean ok = GL_TRUE;
    for (auto patch : _patches)
    {
        ok = ok && patch->UpdatePatch(iso_line_count, maximum_order_of_derivatives, div_point_count, usage_flag);
    }
    if (!ok) throw std::runtime_error("Failed to update patches!");
    return ok;
}

GLboolean SOQAHCompositeSurface3::RenderPatches()
{
    GLboolean ok = GL_TRUE;
    for (auto patch : _patches)
    {
        ok = ok && patch->RenderPatch();
    }
    if (!ok) throw std::runtime_error("Failed to render patches!");
    return ok;
}

size_t SOQAHCompositeSurface3::GetPatchCount() const
{
    return _patches.size();
}

GLboolean SOQAHCompositeSurface3::GetPatchPoint(GLuint patch_index, GLuint point_ind_1, GLuint point_ind_2, DCoordinate3& point)
{
    return _patches[patch_index]->_patch->GetData(point_ind_1, point_ind_2, point);
}

