#include "SOQAHCompositeSurface3.h"
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
//    RowMatrix<GLdouble> u_knot_vector(4);
//    u_knot_vector(0) = 0.0;
//    u_knot_vector(1) = 1.0 / 3.0;
//    u_knot_vector(2) = 2.0 / 3.0;
//    u_knot_vector(3) = 1.0;

//    // 2: create a knot vector in v−direction
//    ColumnMatrix<GLdouble> v_knot_vector(4);
//    v_knot_vector(0) = 0.0;
//    v_knot_vector(1) = 1.0 / 3.0;
//    v_knot_vector(2) = 2.0 / 3.0;
//    v_knot_vector(3) = 1.0;

//    // 3: define a matrix of datapoints, e.g. set them to the original controlpoints
//    Matrix<DCoordinate3> data_points_to_interpolate(4, 4);
//    for(GLuint row = 0; row < 4; ++row)
//    {
//        for(GLuint column = 0; column < 4; ++column)
//        {
//            _patch->GetData(row, column, data_points_to_interpolate(row, column));
//        }
//    }

//    // 4: solve the interpolation problem and generate the mesh of the interpolating patch
//    ok = ok && (_patch->UpdateDataForInterpolation(u_knot_vector, v_knot_vector, data_points_to_interpolate));
//    if (!ok) throw std::runtime_error("Failed to update data for interpolation!");

//    ok = ok && (_after_interpolation = _patch->GenerateImage(div_point_count, div_point_count, usage_flag));
//    if (!ok) throw std::runtime_error("Failed to generate image of patch after interpolation!");

//    ok = ok && _after_interpolation->UpdateVertexBufferObjects(usage_flag);
//    if (!ok) throw std::runtime_error("Failed to update VBOS after interpolation!");

    return ok;
}

GLboolean SOQAHCompositeSurface3::PatchAttributes::RenderPatch(GLuint order)
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

    if (order < 1) return ok;

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

GLboolean SOQAHCompositeSurface3::SetPatchPoint(GLuint patch_index, GLuint point_ind_1, GLuint point_ind_2, const DCoordinate3& point)
{
    return _patches[patch_index]->_patch->SetData(point_ind_1, point_ind_2, point);
}

GLboolean SOQAHCompositeSurface3::JoinPatches(GLuint ind1, Direction dir1, GLuint ind2, Direction dir2)
{
    GLboolean ok = GL_TRUE;

    auto* patch1 = _patches[ind1];
    auto* patch2 = _patches[ind2];

    auto* join_patch = AppendPatch();

    if ((dir1 == Direction::EAST && dir2 == Direction::WEST) ||
        (dir1 == Direction::WEST && dir2 == Direction::EAST))
    {
        if (dir1 == Direction::WEST && dir2 == Direction::EAST)
        {
            patch1 = _patches[ind2];
            patch2 = _patches[ind1];
        }
        for (GLuint i = 0; i < 4; ++i)
        {
            // join the first patch's right side
            auto cp3i = patch1->_patch->operator ()(3, i);
            auto cp2i = patch1->_patch->operator ()(2, i);

            // join patch (new patch)
            auto cp0i = join_patch->_patch->operator ()(0, i);
            auto cp1i = join_patch->_patch->operator ()(1, i);

            cp0i = cp3i;
            cp1i = 2 * cp3i - cp2i;

            ok = ok && join_patch->_patch->SetData(0, i, cp0i.x(), cp0i.y(), cp0i.z());
            ok = ok && join_patch->_patch->SetData(1, i, cp1i.x(), cp1i.y(), cp1i.z());
        }

        for (GLuint i = 0; i < 4; ++i)
        {
            // join the first patch's right side
            auto cp0i = patch2->_patch->operator ()(0, i);
            auto cp1i = patch2->_patch->operator ()(1, i);

            // join patch (new patch)
            auto cp3i = join_patch->_patch->operator ()(3, i);
            auto cp2i = join_patch->_patch->operator ()(2, i);

            cp3i = cp0i;
            cp2i = 2 * cp0i - cp1i;

            ok = ok && join_patch->_patch->SetData(3, i, cp3i.x(), cp3i.y(), cp3i.z());
            ok = ok && join_patch->_patch->SetData(2, i, cp2i.x(), cp2i.y(), cp2i.z());
        }
    }

    if ((dir1 == Direction::NORTH && dir2 == Direction::SOUTH) ||
        (dir1 == Direction::SOUTH && dir2 == Direction::NORTH))
    {
        if (dir1 == Direction::SOUTH && dir2 == Direction::NORTH)
        {
            patch1 = _patches[ind2];
            patch2 = _patches[ind1];
        }
        for (GLuint i = 0; i < 4; ++i)
        {
            // join the first patch's north side
            auto cpi3 = patch1->_patch->operator ()(i, 3);
            auto cpi2 = patch1->_patch->operator ()(i, 2);

            // join patch (new patch)
            auto cp0i = join_patch->_patch->operator ()(i, 0);
            auto cp1i = join_patch->_patch->operator ()(i, 1);

            cp0i = cpi3;
            cp1i = 2 * cpi3 - cpi2;

            ok = ok && join_patch->_patch->SetData(i, 0, cp0i.x(), cp0i.y(), cp0i.z());
            ok = ok && join_patch->_patch->SetData(i, 1, cp1i.x(), cp1i.y(), cp1i.z());
        }

        for (GLuint i = 0; i < 4; ++i)
        {
            // join the first patch's north side
            auto cp0i = patch2->_patch->operator ()(i, 0);
            auto cp1i = patch2->_patch->operator ()(i, 1);

            // join patch (new patch)
            auto cpi3 = join_patch->_patch->operator ()(i, 3);
            auto cpi2 = join_patch->_patch->operator ()(i, 2);

            cpi3 = cp0i;
            cpi2 = 2 * cp0i - cp1i;

            ok = ok && join_patch->_patch->SetData(i, 3, cpi3.x(), cpi3.y(), cpi3.z());
            ok = ok && join_patch->_patch->SetData(i, 2, cpi2.x(), cpi2.y(), cpi2.z());
        }
    }

    return ok;
}

GLboolean SOQAHCompositeSurface3::ContinuePatch(GLuint ind, SOQAHCompositeSurface3::Direction dir)
{
    GLboolean ok = GL_TRUE;

    auto* patch = _patches[ind];
    auto* continue_patch = AppendPatch();

    switch (dir) {
    case Direction::NORTH:
        for (GLuint i = 0; i < 4; ++i)
        {
            auto cpi3 = patch->_patch->operator ()(i, 3);
            auto cpi2 = patch->_patch->operator ()(i, 2);

            auto Ccp0i = cpi3;
            auto Ccp1i = 2 * cpi3 - cpi2;
            auto Ccp2i = 3 * cpi3 - cpi2;
            auto Ccp3i = 4 * cpi3 - cpi2;

            ok = ok && continue_patch->_patch->SetData(i, 0, Ccp0i.x(), Ccp0i.y(), Ccp0i.z());
            ok = ok && continue_patch->_patch->SetData(i, 1, Ccp1i.x(), Ccp1i.y(), Ccp1i.z());
            ok = ok && continue_patch->_patch->SetData(i, 2, Ccp2i.x(), Ccp2i.y(), Ccp2i.z());
            ok = ok && continue_patch->_patch->SetData(i, 3, Ccp3i.x(), Ccp3i.y(), Ccp3i.z());
        }
        break;
    case Direction::EAST:
        for (GLuint i = 0; i < 4; ++i)
        {
            auto cpi3 = patch->_patch->operator ()(3, i);
            auto cpi2 = patch->_patch->operator ()(2, i);

            auto Ccp0i = cpi3;
            auto Ccp1i = 2 * cpi3 - cpi2;
            auto Ccp2i = 3 * cpi3 - cpi2;
            auto Ccp3i = 4 * cpi3 - cpi2;

            ok = ok && continue_patch->_patch->SetData(0, i, Ccp0i.x(), Ccp0i.y(), Ccp0i.z());
            ok = ok && continue_patch->_patch->SetData(1, i, Ccp1i.x(), Ccp1i.y(), Ccp1i.z());
            ok = ok && continue_patch->_patch->SetData(2, i, Ccp2i.x(), Ccp2i.y(), Ccp2i.z());
            ok = ok && continue_patch->_patch->SetData(3, i, Ccp3i.x(), Ccp3i.y(), Ccp3i.z());
        }
        break;
    case Direction::SOUTH:
        for (GLuint i = 0; i < 4; ++i)
        {
            auto cpi3 = patch->_patch->operator ()(i, 0);
            auto cpi2 = patch->_patch->operator ()(i, 1);

            auto Ccp0i = cpi3;
            auto Ccp1i = 2 * cpi3 - cpi2;
            auto Ccp2i = 3 * cpi3 - cpi2;
            auto Ccp3i = 4 * cpi3 - cpi2;

            ok = ok && continue_patch->_patch->SetData(i, 0, Ccp0i.x(), Ccp0i.y(), Ccp0i.z());
            ok = ok && continue_patch->_patch->SetData(i, 1, Ccp1i.x(), Ccp1i.y(), Ccp1i.z());
            ok = ok && continue_patch->_patch->SetData(i, 2, Ccp2i.x(), Ccp2i.y(), Ccp2i.z());
            ok = ok && continue_patch->_patch->SetData(i, 3, Ccp3i.x(), Ccp3i.y(), Ccp3i.z());
        }
        break;
    case Direction::WEST:
        for (GLuint i = 0; i < 4; ++i)
        {
            auto cpi3 = patch->_patch->operator ()(i, 3);
            auto cpi2 = patch->_patch->operator ()(i, 2);

            auto Ccp0i = cpi3;
            auto Ccp1i = 2 * cpi3 - cpi2;
            auto Ccp2i = 3 * cpi3 - cpi2;
            auto Ccp3i = 4 * cpi3 - cpi2;

            ok = ok && continue_patch->_patch->SetData(i, 0, Ccp0i.x(), Ccp0i.y(), Ccp0i.z());
            ok = ok && continue_patch->_patch->SetData(i, 1, Ccp1i.x(), Ccp1i.y(), Ccp1i.z());
            ok = ok && continue_patch->_patch->SetData(i, 2, Ccp2i.x(), Ccp2i.y(), Ccp2i.z());
            ok = ok && continue_patch->_patch->SetData(i, 3, Ccp3i.x(), Ccp3i.y(), Ccp3i.z());
        }
        break;
    default:
        break;
    }


    return ok;
}

GLboolean SOQAHCompositeSurface3::MergePatches(GLuint ind1, SOQAHCompositeSurface3::Direction dir1, GLuint ind2, SOQAHCompositeSurface3::Direction dir2)
{
    auto* patch1 = _patches[ind1];
    auto* patch2 = _patches[ind2];

    if (dir1 == Direction::NORTH && dir2 == Direction::NORTH)
    {
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(i, 2);
            auto q = patch2->_patch->operator ()(i, 2);

            auto res = 0.5 * (p + q);

            patch1->_patch->SetData(i, 3, res.x(), res.y(), res.z());
            patch2->_patch->SetData(i, 3, res.x(), res.y(), res.z());
        }
    }

    if (dir1 == Direction::SOUTH && dir2 == Direction::SOUTH)
    {
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(i, 1);
            auto q = patch2->_patch->operator ()(i, 1);

            auto res = 0.5 * (p + q);

            patch1->_patch->SetData(i, 0, res.x(), res.y(), res.z());
            patch2->_patch->SetData(i, 0, res.x(), res.y(), res.z());
        }
    }

    if (dir1 == Direction::WEST && dir2 == Direction::WEST)
    {
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(2, i);
            auto q = patch2->_patch->operator ()(2, i);

            auto res = 0.5 * (p + q);

            patch1->_patch->SetData(3, i, res.x(), res.y(), res.z());
            patch2->_patch->SetData(3, i, res.x(), res.y(), res.z());
        }
    }

    if (dir1 == Direction::EAST && dir2 == Direction::EAST)
    {
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(1, i);
            auto q = patch2->_patch->operator ()(1, i);

            auto res = 0.5 * (p + q);

            patch1->_patch->SetData(0, i, res.x(), res.y(), res.z());
            patch2->_patch->SetData(0, i, res.x(), res.y(), res.z());
        }
    }

    if ((dir1 == Direction::NORTH && dir2 == Direction::SOUTH) ||
        (dir1 == Direction::SOUTH && dir2 == Direction::NORTH))
    {
        if (dir1 == Direction::SOUTH && dir2 == Direction::NORTH)
        {
            patch1 = _patches[ind2];
            patch2 = _patches[ind1];
        }
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(i, 2);
            auto q = patch2->_patch->operator ()(i, 1);

            auto res = 0.5 * (p + q);

            patch1->_patch->SetData(i, 3, res.x(), res.y(), res.z());
            patch2->_patch->SetData(i, 0, res.x(), res.y(), res.z());
        }
    }

    if ((dir1 == Direction::EAST && dir2 == Direction::WEST) ||
        (dir1 == Direction::WEST && dir2 == Direction::EAST))
    {
        if (dir1 == Direction::WEST && dir2 == Direction::EAST)
        {
            patch1 = _patches[ind2];
            patch2 = _patches[ind1];
        }
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(2, i);
            auto q = patch2->_patch->operator ()(1, i);

            auto res = 0.5 * (p + q);

            patch1->_patch->SetData(3, i, res.x(), res.y(), res.z());
            patch2->_patch->SetData(0, i, res.x(), res.y(), res.z());
        }
    }

    return GL_TRUE;
}

