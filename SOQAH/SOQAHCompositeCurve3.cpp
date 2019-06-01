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

SOQAHCompositeCurve3::ArcAttributes* SOQAHCompositeCurve3::AppendArc()
{
    _arcs.push_back(new SOQAHCompositeCurve3::ArcAttributes());
    return _arcs[_arcs.size() - 1];
}

DCoordinate3& SOQAHCompositeCurve3::GetArcPoint(GLuint arc_index, GLuint point_ind)
{
    return _arcs[arc_index]->_arc->operator [](point_ind);
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
        if (!ok) throw std::runtime_error("Failed to render the 1st oreder derivatives of the arcs!");
    }

    return ok;
}

