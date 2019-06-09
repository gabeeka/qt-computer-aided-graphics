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

    ok = ok && _patch->UpdateVertexBufferObjectsOfData();

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

GLboolean SOQAHCompositeSurface3::PatchAttributes::RenderPatch(GLboolean renderControlNet)
{
    GLboolean ok = GL_TRUE;

    if (renderControlNet)
    {
        glDisable(GL_LIGHTING);
        glColor3f(0.0, 0.0, 1.0);
        ok = ok && _patch->RenderData();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    ApplyMaterial(_materialIndex);
    ok = ok && _image_of_patch->Render();
    if (!ok) throw std::runtime_error("Failed to render the image of patch!");

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

void SOQAHCompositeSurface3::PatchAttributes::ApplyMaterial(GLuint materialIndex)
{
    switch (materialIndex) {
    case 0:
        MatFBBrass.Apply();
        break;
    case 1:
        MatFBGold.Apply();
        break;
    case 2:
        MatFBSilver.Apply();
        break;
    case 3:
        MatFBEmerald.Apply();
        break;
    case 4:
        MatFBPearl.Apply();
        break;
    case 5:
        MatFBRuby.Apply();
        break;
    case 6:
        MatFBTurquoise.Apply();
        break;
    default:
        break;
    }
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

void SOQAHCompositeSurface3::SetMaterialIndex(GLuint patchIndex, GLuint materialIndex)
{
    _patches[patchIndex]->_materialIndex = materialIndex;
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
        (dir2 == Direction::EAST && patch2->_east)   ||
        (dir2 == Direction::NORTH && patch2->_north) ||
        (dir2 == Direction::WEST && patch2->_west)   ||
        (dir2 == Direction::SOUTH && patch2->_south))
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
        join_patch->_west = patch1;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(0, i);
            auto q = patch1->_patch->operator ()(1, i);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(0, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(1, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir1 == Direction::SOUTH)
    {
        patch1->_south = join_patch;
        join_patch->_west = patch1;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(i, 3);
            auto q = patch1->_patch->operator ()(i, 2);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(0, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(1, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir1 == Direction::NORTH)
    {
        patch1->_north = join_patch;
        join_patch->_west = patch1;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch1->_patch->operator ()(i, 0);
            auto q = patch1->_patch->operator ()(i, 1);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(0, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(1, i, qq.x(), qq.y(), qq.z());
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
        join_patch->_east = patch2;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch2->_patch->operator ()(0, i);
            auto q = patch2->_patch->operator ()(1, i);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(3, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(2, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir2 == Direction::SOUTH)
    {
        patch2->_south = join_patch;
        join_patch->_east = patch2;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch2->_patch->operator ()(i, 3);
            auto q = patch2->_patch->operator ()(i, 2);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(3, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(2, i, qq.x(), qq.y(), qq.z());
        }
    }
    if (dir2 == Direction::NORTH)
    {
        patch2->_north = join_patch;
        join_patch->_east = patch2;

        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch2->_patch->operator ()(i, 0);
            auto q = patch2->_patch->operator ()(i, 1);

            auto pp = p;
            auto qq = 2 * p - q;

            ok = ok && join_patch->_patch->SetData(3, i, pp.x(), pp.y(), pp.z());
            ok = ok && join_patch->_patch->SetData(2, i, qq.x(), qq.y(), qq.z());
        }
    }

    return ok;
}

GLboolean SOQAHCompositeSurface3::ContinuePatch(GLuint ind, SOQAHCompositeSurface3::Direction dir)
{
    GLboolean ok = GL_TRUE;

    auto* patch = _patches[ind];

    // Check if continue is possible
    if ((dir == Direction::EAST  && patch->_east)   ||
        (dir == Direction::NORTH && patch->_north)  ||
        (dir == Direction::WEST  && patch->_west)   ||
        (dir == Direction::SOUTH && patch->_south))
    {
        std::cout << "Continue failed, invalid direction." << std::endl;
        return GL_FALSE;
    }

    auto* continue_patch = AppendPatch();

    switch (dir) {
    case Direction::NORTH:
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

            patch->_south = continue_patch;
            continue_patch->_north = patch;
        }
        break;
    case Direction::WEST:
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
    GLboolean ok = GL_TRUE;

    auto* patch1 = _patches[ind1];
    auto* patch2 = _patches[ind2];

    // Check if merge is possible
    if ((dir1 == Direction::EAST  && patch1->_east)   ||
        (dir1 == Direction::NORTH && patch1->_north)  ||
        (dir1 == Direction::WEST  && patch1->_west)   ||
        (dir1 == Direction::SOUTH && patch1->_south)  ||
        (dir2 == Direction::EAST  && patch2->_east)   ||
        (dir2 == Direction::NORTH && patch2->_north)  ||
        (dir2 == Direction::WEST  && patch2->_west)   ||
        (dir2 == Direction::SOUTH && patch2->_south))
    {
        std::cout << "Merge failed, invalid direction." << std::endl;
        return GL_FALSE;
    }

    for (GLuint i = 0; i < 4; ++i)
    {
        DCoordinate3 p;
        DCoordinate3 q;

        // Get the appropriate points based on directions
        switch (dir1)
        {
        case Direction::NORTH:
            p = patch1->_patch->operator ()(i, 1);
            break;
        case Direction::SOUTH:
            p = patch1->_patch->operator ()(i, 2);
            break;
        case Direction::WEST:
            p = patch1->_patch->operator ()(1, i);
            break;
        case Direction::EAST:
            p = patch1->_patch->operator ()(2, i);
            break;
        default:
            std::cout << "Merge failed, invalid direction." << std::endl;
            return GL_FALSE;
        }
        switch (dir2)
        {
        case Direction::NORTH:
            q = patch2->_patch->operator ()(i, 1);
            break;
        case Direction::SOUTH:
            q = patch2->_patch->operator ()(i, 2);
            break;
        case Direction::WEST:
            q = patch2->_patch->operator ()(1, i);
            break;
        case Direction::EAST:
            q = patch2->_patch->operator ()(2, i);
            break;
        default:
            std::cout << "Merge failed, invalid direction." << std::endl;
            return GL_FALSE;
        }

        // Calculate the merge point
        auto res = 0.5 * (p + q);

        // Update the existing points based in directions
        switch (dir1)
        {
        case Direction::NORTH:
            ok = ok && patch1->_patch->SetData(i, 0, res.x(), res.y(), res.z());
            patch1->_north = patch2;
            break;
        case Direction::SOUTH:
            ok = ok && patch1->_patch->SetData(i, 3, res.x(), res.y(), res.z());
            patch1->_south = patch2;
            break;
        case Direction::WEST:
            ok = ok && patch1->_patch->SetData(0, i, res.x(), res.y(), res.z());
            patch1->_west = patch2;
            break;
        case Direction::EAST:
            ok = ok && patch1->_patch->SetData(3, i, res.x(), res.y(), res.z());
            patch1->_east = patch2;
            break;
        default:
            std::cout << "Merge failed, invalid direction." << std::endl;
            return GL_FALSE;
        }
        switch (dir2)
        {
        case Direction::NORTH:
            ok = ok && patch2->_patch->SetData(i, 0, res.x(), res.y(), res.z());
            patch2->_north = patch1;
            break;
        case Direction::SOUTH:
            ok = ok && patch2->_patch->SetData(i, 3, res.x(), res.y(), res.z());
            patch2->_south = patch1;
            break;
        case Direction::WEST:
            ok = ok && patch2->_patch->SetData(0, i, res.x(), res.y(), res.z());
            patch2->_west = patch1;
            break;
        case Direction::EAST:
            ok = ok && patch2->_patch->SetData(3, i, res.x(), res.y(), res.z());
            patch2->_east = patch1;
            break;
        default:
            std::cout << "Merge failed, invalid direction." << std::endl;
            return GL_FALSE;
        }
    }

    return ok;
}

GLboolean SOQAHCompositeSurface3::RefreshNeighbours(GLuint ind)
{
    GLboolean ok = GL_TRUE;
    auto* patch = _patches[ind];

    if (patch->_north)
    {
        auto* patch_north = patch->_north;
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch->_patch->operator ()(i, 0);
            auto q = patch->_patch->operator ()(i, 1);
            auto res0 = p;
            auto res1 = 2 * p - q;

            if (patch_north->_north == patch)
            {
                ok = ok && patch_north->_patch->SetData(i, 0, res0);
                ok = ok && patch_north->_patch->SetData(i, 1, res1);
            }
            else if (patch_north->_south == patch)
            {
                ok = ok && patch_north->_patch->SetData(i, 3, res0);
                ok = ok && patch_north->_patch->SetData(i, 2, res1);
            }
            else if (patch_north->_west == patch)
            {
                ok = ok && patch_north->_patch->SetData(0, i, res0);
                ok = ok && patch_north->_patch->SetData(1, i, res1);
            }
            else if (patch_north->_east == patch)
            {
                ok = ok && patch_north->_patch->SetData(3, i, res0);
                ok = ok && patch_north->_patch->SetData(2, i, res1);
            }
        }
    }

    if (patch->_south)
    {
        auto* patch_south = patch->_south;
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch->_patch->operator ()(i, 3);
            auto q = patch->_patch->operator ()(i, 2);
            auto res0 = p;
            auto res1 = 2 * p - q;

            if (patch_south->_north == patch)
            {
                ok = ok && patch_south->_patch->SetData(i, 0, res0);
                ok = ok && patch_south->_patch->SetData(i, 1, res1);
            }
            else if (patch_south->_south == patch)
            {
                ok = ok && patch_south->_patch->SetData(i, 3, res0);
                ok = ok && patch_south->_patch->SetData(i, 2, res1);
            }
            else if (patch_south->_west == patch)
            {
                ok = ok && patch_south->_patch->SetData(0, i, res0);
                ok = ok && patch_south->_patch->SetData(1, i, res1);
            }
            else if (patch_south->_east == patch)
            {
                ok = ok && patch_south->_patch->SetData(3, i, res0);
                ok = ok && patch_south->_patch->SetData(2, i, res1);
            }
        }
    }

    if (patch->_west)
    {
        auto* patch_west = patch->_west;
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch->_patch->operator ()(0, i);
            auto q = patch->_patch->operator ()(1, i);
            auto res0 = p;
            auto res1 = 2 * p - q;

            if (patch_west->_north == patch)
            {
                ok = ok && patch_west->_patch->SetData(i, 0, res0);
                ok = ok && patch_west->_patch->SetData(i, 1, res1);
            }
            else if (patch_west->_south == patch)
            {
                ok = ok && patch_west->_patch->SetData(i, 3, res0);
                ok = ok && patch_west->_patch->SetData(i, 2, res1);
            }
            else if (patch_west->_west == patch)
            {
                ok = ok && patch_west->_patch->SetData(0, i, res0);
                ok = ok && patch_west->_patch->SetData(1, i, res1);
            }
            else if (patch_west->_east == patch)
            {
                ok = ok && patch_west->_patch->SetData(3, i, res0);
                ok = ok && patch_west->_patch->SetData(2, i, res1);
            }
        }
    }

    if (patch->_east)
    {
        auto* patch_east = patch->_east;
        for (GLuint i = 0; i < 4; ++i)
        {
            auto p = patch->_patch->operator ()(3, i);
            auto q = patch->_patch->operator ()(2, i);
            auto res0 = p;
            auto res1 = 2 * p - q;

            if (patch_east->_north == patch)
            {
                ok = ok && patch_east->_patch->SetData(i, 0, res0);
                ok = ok && patch_east->_patch->SetData(i, 1, res1);
            }
            else if (patch_east->_south == patch)
            {
                ok = ok && patch_east->_patch->SetData(i, 3, res0);
                ok = ok && patch_east->_patch->SetData(i, 2, res1);
            }
            else if (patch_east->_west == patch)
            {
                ok = ok && patch_east->_patch->SetData(0, i, res0);
                ok = ok && patch_east->_patch->SetData(1, i, res1);
            }
            else if (patch_east->_east == patch)
            {
                ok = ok && patch_east->_patch->SetData(3, i, res0);
                ok = ok && patch_east->_patch->SetData(2, i, res1);
            }
        }
    }

    return ok;
}

