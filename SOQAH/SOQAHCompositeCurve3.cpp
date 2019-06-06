#include "SOQAHCompositeCurve3.h"

using namespace cagd;

GLboolean SOQAHCompositeCurve3::ArcAttributes::GenerateImage(GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag)
{
    _img = _arc->GenerateImage(max_order_of_derivatives, div_point_count, usage_flag);
    if (_img)
    {
        return GL_TRUE;
    }

    return GL_FALSE;
}

SOQAHCompositeCurve3::SOQAHCompositeCurve3(GLuint arcCount)
{
    _arcs.reserve(arcCount);
}

SOQAHCompositeCurve3::ArcAttributes* SOQAHCompositeCurve3::AppendArc(GLboolean is_join_arc)
{
    _arcs.push_back(new SOQAHCompositeCurve3::ArcAttributes());
    _arcs[_arcs.size() - 1]->_is_join_arc = is_join_arc;
    return _arcs[_arcs.size() - 1];
}

DCoordinate3& SOQAHCompositeCurve3::GetArcPoint(GLuint arc_index, GLuint point_ind)
{
    return _arcs[arc_index]->_arc->operator [](point_ind);
}

DCoordinate3 SOQAHCompositeCurve3::GetArcPoint(GLuint arc_index, GLuint point_ind) const
{
    return _arcs[arc_index]->_arc->operator [](point_ind);
}

size_t SOQAHCompositeCurve3::GetArcCount() const
{
    return _arcs.size();
}

GLboolean SOQAHCompositeCurve3::UpdateVBODatas(GLenum usage_flag)
{
    GLboolean ok = GL_TRUE;

    for (auto arc : _arcs)
    {
        ok = ok && arc->_arc->UpdateVertexBufferObjectsOfData(usage_flag);
    }


    return ok;
}

GLboolean SOQAHCompositeCurve3::GenerateImages(GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag)
{
    GLboolean ok = GL_TRUE;
    for (auto arc : _arcs)
    {
        ok = ok && arc->GenerateImage(max_order_of_derivatives, div_point_count, usage_flag);
    }
    return ok;
}

GLboolean SOQAHCompositeCurve3::UpdateVBOs(GLenum usage_flag)
{
    GLboolean ok = GL_TRUE;
    for (auto arc : _arcs)
    {
        ok = ok && arc->_img->UpdateVertexBufferObjects(usage_flag);
    }
    return ok;
}

GLboolean SOQAHCompositeCurve3::Render(GLuint order, GLboolean renderControlPoints) const
{
    GLboolean ok = GL_TRUE;

    glDisable(GL_LIGHTING);

    // If rendering of control points is required
    if (renderControlPoints)
    {
        for (auto arc : _arcs)
        {
            glColor3f(arc->_arc_color.r(), arc->_arc_color.g(), arc->_arc_color.b());
            ok = ok && arc->_arc->RenderData();
        }
        if (!ok) throw std::runtime_error("Failed to render the control net of arcs!");
    }

    // Render the image of arc
    for (auto arc : _arcs)
    {
        glColor3f(arc->_img_color_0.r(), arc->_img_color_0.g(), arc->_img_color_0.b());
        ok = ok && arc->_img->RenderDerivatives(0, GL_LINE_STRIP);
    }
    if (!ok) throw std::runtime_error("Failed to render the image of arcs!");

//    order = 2;
    if (order >= 1)
    {
        for (auto arc : _arcs)
        {
            glColor3f(arc->_img_color_1.r(), arc->_img_color_1.g(), arc->_img_color_1.b());
            ok = ok && arc->_img->RenderDerivatives(1, GL_LINES);
        }
        if (!ok) throw std::runtime_error("Failed to render the 1st oreder derivatives of the arcs!");
    }

    if (order >= 2)
    {
        for (auto arc : _arcs)
        {
            glColor3f(arc->_img_color_2.r(), arc->_img_color_2.g(), arc->_img_color_2.b());
            ok = ok && arc->_img->RenderDerivatives(2, GL_LINES);
        }
        if (!ok) throw std::runtime_error("Failed to render the 2nd oreder derivatives of the arcs!");
    }

    return ok;
}

GLboolean SOQAHCompositeCurve3::JoinArcs(GLuint ind1, Direction dir1, GLuint ind2, Direction dir2)
{
    GLuint upper_bound = static_cast<GLuint>(_arcs.size() - 1);
    if (ind1 > upper_bound || ind2 > upper_bound)
    {
        std::cout << "Join failed: invalid arc index!" << std::endl;
        return GL_FALSE;
    }

    auto* arc1 = _arcs[ind1];
    auto* arc2 = _arcs[ind2];

    // We add a new arc
    auto* new_arc = AppendArc(GL_TRUE);
    // Set color
    new_arc->_arc_color    = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));
    new_arc->_img_color_0  = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));
    new_arc->_img_color_1  = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));
    new_arc->_img_color_2  = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));

    // Set the new arc's 1st and 4th control point based on directions, calculate the rest
    if (dir1 == Direction::LEFT)
    {
        auto p0 = GetArcPoint(ind1, 0);
        auto p1 = GetArcPoint(ind1, 1);
        new_arc->_arc->operator [](0) = p0;
        new_arc->_arc->operator [](1) = (2 * p0 - p1);
        arc1->_left = new_arc;
    }
    else
    {
        auto p2 = GetArcPoint(ind1, 2);
        auto p3 = GetArcPoint(ind1, 3);
        new_arc->_arc->operator [](0) = p3;
        new_arc->_arc->operator [](1) = (2 * p3 - p2);
        arc1->_right = new_arc;
    }

    if (dir2 == Direction::LEFT)
    {
        auto p0 = GetArcPoint(ind2, 0);
        auto p1 = GetArcPoint(ind2, 1);
        new_arc->_arc->operator [](3) = p0;
        new_arc->_arc->operator [](2) = (2 * p0 - p1);
        arc2->_left = new_arc;
    }
    else
    {
        auto p2 = GetArcPoint(ind2, 2);
        auto p3 = GetArcPoint(ind2, 3);
        new_arc->_arc->operator [](3) = p3;
        new_arc->_arc->operator [](2) = (2 * p3 - p2);
        arc2->_right = new_arc;
    }

    new_arc->_left = arc1;
    new_arc->_right = arc2;

    return GL_TRUE;
}

GLboolean SOQAHCompositeCurve3::Continue(GLuint ind, SOQAHCompositeCurve3::Direction dir)
{
    GLuint upper_bound = static_cast<GLuint>(_arcs.size() - 1);
    if (ind > upper_bound)
    {
        std::cout << "Join failed: invalid arc index!" << std::endl;
        return GL_FALSE;
    }

    auto* arc = _arcs[ind];
    auto* new_arc = AppendArc();
    // Set color
    new_arc->_arc_color    = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));
    new_arc->_img_color_0  = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));
    new_arc->_img_color_1  = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));
    new_arc->_img_color_2  = Color4((static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)),
                                    (static_cast<float>(rand()) / (RAND_MAX)));

    if (dir == Direction::LEFT)
    {
        auto p0 = GetArcPoint(ind, 0);
        auto p1 = GetArcPoint(ind, 1);
        new_arc->_arc->operator [](0) = p0;
        new_arc->_arc->operator [](1) = (2 * p0 - p1);
        new_arc->_arc->operator [](2) = (3 * p0 - p1);
        new_arc->_arc->operator [](3) = (4 * p0 - p1);
        arc->_left = new_arc;
    }
    else
    {
        auto p2 = GetArcPoint(ind, 2);
        auto p3 = GetArcPoint(ind, 3);
        new_arc->_arc->operator [](0) = p3;
        new_arc->_arc->operator [](1) = (2 * p3 - p2);
        new_arc->_arc->operator [](2) = (3 * p3 - p2);
        new_arc->_arc->operator [](3) = (4 * p3 - p2);
        arc->_right = new_arc;
    }

    new_arc->_left = arc;

    return GL_TRUE;

}

GLboolean SOQAHCompositeCurve3::MergeArcs(GLuint ind1, SOQAHCompositeCurve3::Direction dir1, GLuint ind2, SOQAHCompositeCurve3::Direction dir2)
{
    GLuint upper_bound = static_cast<GLuint>(_arcs.size() - 1);
    if (ind1 > upper_bound || ind2 > upper_bound)
    {
        std::cout << "Join failed: invalid arc index!" << std::endl;
        return GL_FALSE;
    }

    auto* arc1 = _arcs[ind1];
    auto* arc2 = _arcs[ind2];

    // Set the new arc's 1st and 4th control point based on directions, calculate the rest
    if (dir1 == Direction::LEFT && dir2 == Direction::LEFT)
    {
        auto p = GetArcPoint(ind1, 1);
        auto q = GetArcPoint(ind2, 1);

        auto res = 0.5 * (p + q);

        arc1->_arc->operator [](0) = res;
        arc2->_arc->operator [](0) = res;
        arc1->_left = arc2;
        arc2->_left = arc1;
    }

    if (dir1 == Direction::RIGHT && dir2 == Direction::RIGHT)
    {
        auto p3 = GetArcPoint(ind1, 2);
        auto q3 = GetArcPoint(ind1, 2);

        auto res = 0.5 * (p3 + q3);

        arc1->_arc->operator [](3) = res;
        arc2->_arc->operator [](3) = res;
        arc1->_right = arc2;
        arc2->_right = arc1;
    }

    if (dir1 == Direction::LEFT && dir2 == Direction::RIGHT)
    {
        auto p0 = GetArcPoint(ind1, 1);
        auto q3 = GetArcPoint(ind1, 2);

        auto res = 0.5 * (p0 + q3);

        arc1->_arc->operator [](0) = res;
        arc2->_arc->operator [](3) = res;
        arc1->_left = arc2;
        arc2->_right = arc1;
    }

    if (dir1 == Direction::RIGHT && dir2 == Direction::LEFT)
    {
        auto q3 = GetArcPoint(ind1, 2);
        auto p0 = GetArcPoint(ind2, 1);

        auto res = 0.5 * (p0 + q3);

        arc1->_arc->operator [](3) = res;
        arc2->_arc->operator [](0) = res;
        arc1->_right = arc2;
        arc2->_left = arc1;
    }

    return GL_TRUE;
}

void SOQAHCompositeCurve3::RefreshNeighbours(GLuint ind)
{
    auto* arc = _arcs[ind];

    if (arc->_left)
    {
        auto* arc_left = arc->_left;
        auto p0 = arc->_arc->operator [](0);
        auto p1 = arc->_arc->operator [](1);

        if (arc_left->_left == arc)
        {
            arc_left->_arc->operator [](0) = p0;
            arc_left->_arc->operator [](1) = (2 * p0 - p1);
        }
        else if (arc_left->_right == arc)
        {
            arc_left->_arc->operator [](3) = p0;
            arc_left->_arc->operator [](2) = (2 * p0 - p1);
        }
    }

    if (arc->_right)
    {
        auto* arc_right = arc->_right;
        auto p2 = arc->_arc->operator [](2);
        auto p3 = arc->_arc->operator [](3);

        if (arc->_right == arc)
        {
            arc_right->_arc->operator [](3) = p3;
            arc_right->_arc->operator [](2) = (2 * p3 - p2);
        }
        else if (arc_right->_left == arc)
        {
            arc_right->_arc->operator [](0) = p3;
            arc_right->_arc->operator [](1) = (2 * p3 - p2);
        }
    }
}

