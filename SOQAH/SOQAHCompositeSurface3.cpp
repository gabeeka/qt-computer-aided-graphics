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
    ok = ok && (_image_of_patch = _patch->GenerateImage(30, 30, GL_STATIC_DRAW));
    if (!ok) throw std::runtime_error("Failed to generate image of patch!");
    // Update the VBOs of the image of patch
    ok = ok && (_image_of_patch->UpdateVertexBufferObjects(usage_flag));
    if (!ok) throw std::runtime_error("Failed to updathe the VBOs of the image of patch!");

    return ok;
}

GLboolean SOQAHCompositeSurface3::PatchAttributes::RenderPatch(GLuint order)
{
    GLboolean ok = GL_TRUE;

    MatFBRuby.Apply();
    ok = ok && _image_of_patch->Render();
    if (!ok) throw std::runtime_error("Failed to render the image of patch!");

//    if (order < 1) return ok;

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

    // Check if join is possible
    if ((dir1 == Direction::EAST && patch1->_east)   ||
        (dir1 == Direction::NORTH && patch1->_north) ||
        (dir1 == Direction::WEST && patch1->_west)   ||
        (dir1 == Direction::SOUTH && patch1->_south) ||
        (dir2 == Direction::EAST && patch1->_east)   ||
        (dir2 == Direction::NORTH && patch1->_north) ||
        (dir2 == Direction::WEST && patch1->_west)   ||
        (dir2 == Direction::SOUTH && patch1->_south))
    {
        std::cout << "Join failed, invalid direction." << std::endl;
        return GL_FALSE;
    }

    auto* join_patch = AppendPatch();

    // Connect the first patch
    if (dir1 == Direction::EAST)
    {
        patch1->_east = join_patch;
        join_patch->_west = patch1;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(3, i);
            auto q = patch1->_patch->operator ()(2, i);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(0, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(1, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir1 == Direction::WEST)
    {
        patch1->_west = join_patch;
        join_patch->_east = patch1;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(0, i);
            auto q = patch1->_patch->operator ()(1, i);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(3, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(2, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir1 == Direction::SOUTH)
    {
        patch1->_south = join_patch;
        join_patch->_north = patch1;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(i, 0);
            auto q = patch1->_patch->operator ()(i, 1);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(i, 3, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(i, 2, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir1 == Direction::NORTH)
    {
        patch1->_north = join_patch;
        join_patch->_south = patch1;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(i, 3);
            auto q = patch1->_patch->operator ()(i, 2);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(i, 0, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(i, 1, qq.x(), qq.y(), qq.z());
        }
    }

    // Connect the second patch
    if (dir2 == Direction::EAST)
    {
        patch2->_east = join_patch;
        join_patch->_east = patch2;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch2->_patch->operator ()(3, i);
            auto q = patch2->_patch->operator ()(2, i);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(3, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(2, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir2 == Direction::WEST)
    {
        patch2->_west = join_patch;
        join_patch->_west = patch2;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch2->_patch->operator ()(0, i);
            auto q = patch2->_patch->operator ()(1, i);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(0, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(1, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir2 == Direction::SOUTH)
    {
        patch2->_south = join_patch;
        join_patch->_south = patch2;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch2->_patch->operator ()(i, 0);
            auto q = patch2->_patch->operator ()(i, 1);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(i, 0, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(i, 1, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir2 == Direction::NORTH)
    {
        patch2->_north = join_patch;
        join_patch->_south = patch2;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch2->_patch->operator ()(i, 3);
            auto q = patch2->_patch->operator ()(i, 2);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(i, 3, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(i, 2, qq.x(), qq.y(), qq.z());
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

            patch->_north = continue_patch;
            continue_patch->_south = patch;
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

            patch->_east = continue_patch;
            continue_patch->_west = patch;
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

            patch->_south = continue_patch;
            continue_patch->_north = patch;
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

            patch->_west = continue_patch;
            continue_patch->_east = patch;
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

