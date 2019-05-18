#include "GLWidget.h"
#include "../Core/Materials.h"
#include "../Core/Constants.h"

#if !defined(__APPLE__)
#include <GL/glu.h>
#endif

#include <iostream>
using namespace std;

#include <Core/Exceptions.h>
#include <QTime>

#include "../Test/TestFunctions.h"

namespace cagd
{
    //--------------------------------
    // special and default constructor
    //--------------------------------
    GLWidget::GLWidget(QWidget *parent, const QGLFormat &format): QGLWidget(format, parent)
    {
        _timer = new QTimer(this);
        _timer->setInterval(0);
        connect(_timer, SIGNAL(timeout()), this, SLOT(_animate()));
    }

    //-----------
    // destructor
    //-----------
    GLWidget::~GLWidget()
    {
        if (_dl)
        {
            glDeleteLists(_dl, 1);
        }

        if (_pc)
        {
            delete _pc, _pc = nullptr;
        }
        if (_image_of_pc)
        {
            delete _image_of_pc, _image_of_pc = nullptr;
        }
    }

    //--------------------------------------------------------------------------------------
    // this virtual function is called once before the first call to paintGL() or resizeGL()
    //--------------------------------------------------------------------------------------
    void GLWidget::initializeGL()
    {
        // creating a perspective projection matrix
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        _aspect = (double)width() / (double)height();
        _z_near = 1.0;
        _z_far  = 1000.0;
        _fovy   = 45.0;

        gluPerspective(_fovy, _aspect, _z_near, _z_far);

        // setting the model view matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        _eye[0] = _eye[1] = 0.0; _eye[2] = 6.0;
        _center[0] = _center[1] = _center[2] = 0.0;
        _up[0] = _up[2] = 0.0; _up[1] = 1.0;

        gluLookAt(_eye[0], _eye[1], _eye[2], _center[0], _center[1], _center[2], _up[0], _up[1], _up[2]);

        // enabling the depth test
        glEnable(GL_DEPTH_TEST);

        // setting the background color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // initial values of transformation parameters
        _angle_x = _angle_y = _angle_z = 0.0;
        _trans_x = _trans_y = _trans_z = 0.0;
        _zoom = 1.0;

        try
        {
            glEnable(GL_POINT_SMOOTH);
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_POLYGON_SMOOTH, GL_NICEST);
            glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
            glEnable(GL_DEPTH_TEST);

            // initializing the OpenGL Extension Wrangler library
            GLenum error = glewInit();

            if (error != GLEW_OK)
            {
                throw Exception("Could not initialize the OpenGL Extension Wrangler Library!");
            }

            if (!glewIsSupported("GL_VERSION_2_0"))
            {
                throw Exception("Your graphics card is not compatible with OpenGL 2.0+! "
                                "Try to update your driver or buy a new graphics adapter!");
            }

            // create and store your geometry in display lists or vertex buffer objects
            // ...

            if (_mouse.LoadFromOFF("Models/mouse.off", true))
            {
                if (_mouse.UpdateVertexBufferObjects(GL_DYNAMIC_DRAW))
                {
                    _angle = 0.0;
                    _timer->start();
                }
            }

            //initTorusKnot(3);
            //initLissajous(1);
            //initRose(1);
            //initBoxOfTriangles();
        }

        catch (Exception &e)
        {
            cout << e << endl;
        }
    }

    //-----------------------
    // the rendering function
    //-----------------------
    void GLWidget::paintGL()
    {
        // clears the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // stores/duplicates the original model view matrix
        glPushMatrix();

            // applying transformations
            glRotatef(_angle_x, 1.0, 0.0, 0.0);
            glRotatef(_angle_y, 0.0, 1.0, 0.0);
            glRotatef(_angle_z, 0.0, 0.0, 1.0);
            glTranslated(_trans_x, _trans_y, _trans_z);
            glScaled(_zoom, _zoom, _zoom);

            // render your geometry (this is oldest OpenGL rendering technique, later we will use some advanced methods)
            //glPushMatrix();

            //paintTorusKnot(3);
            //paintLissajous(1);
            //paintRose(1);
            //paintBoxOfTriangles();

            MatFBRuby.Apply();
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_NORMALIZE);
            _mouse.Render();
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_NORMALIZE);

        // pops the current matrix stack, replacing the current matrix with the one below it on the stack,
        // i.e., the original model view matrix is restored
        glPopMatrix();
    }

    //----------------------------------------------------------------------------
    // when the main window is resized one needs to redefine the projection matrix
    //----------------------------------------------------------------------------
    void GLWidget::resizeGL(int w, int h)
    {
        // setting the new size of the rendering context
        glViewport(0, 0, w, h);

        // redefining the projection matrix
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        _aspect = (double)w / (double)h;

        gluPerspective(_fovy, _aspect, _z_near, _z_far);

        // switching back to the model view matrix
        glMatrixMode(GL_MODELVIEW);

        updateGL();
    }

    //----------------------------------------------------
    // implementation of the private methods for homeworks
    //----------------------------------------------------
    void GLWidget::initBoxOfTriangles()
    {
        _triangle_count = 10000;
        _cloud.resize(3 * _triangle_count);

        for (GLuint i = 0; i < _cloud.size(); i++)
        {
            DCoordinate3 &ref = _cloud[i];

            ref[0] = -1.0 + 2.0 * (GLdouble)rand() / (GLdouble)RAND_MAX;
            ref[1] = -1.0 + 2.0 * (GLdouble)rand() / (GLdouble)RAND_MAX;
            ref[2] = -1.0 + 2.0 * (GLdouble)rand() / (GLdouble)RAND_MAX;
        }

        _dl = glGenLists(1);
        if (!_dl)
        {
            throw Exception("Could not create the display list!");
        }

        glNewList(_dl, GL_COMPILE);
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glBegin(GL_TRIANGLES);
                for (GLuint i = 0; i < _cloud.size(); i++)
                {
                    glColor3dv(&_cloud[i][0]);
                    glVertex3dv(&_cloud[i][0]);
                }
            glEnd();
            glPopAttrib();
        glEndList();

        _angle = 0;
        _timer->start();
    }

    void GLWidget::paintBoxOfTriangles()
    {
        QTime T;
        T.start();

        glRotated(_angle, 1.0, 1.0, 1.0);
        glTranslated(-1.0, 0.0, 0.0);
        if (_dl)
        {
            glCallList(_dl);
        }
        glPopMatrix();

        glPushMatrix();
        glRotated(_angle, 1.0, -1.0, 1.0);
        glTranslated(1.0, 0.0, 0.0);
        if (_dl)
        {
            glCallList(_dl);
        }
        glPopMatrix();
        cout << "Done in " << T.elapsed() / 1000.0 << " in seconds." << endl;
    }

    void GLWidget::initSpiralOnCone(GLuint order)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(order);
        derivative(0) = spiral_on_cone::d0;
        if (order > 1) derivative(1) = spiral_on_cone::d1;
        if (order > 2) derivative(2) = spiral_on_cone::d2;

        _pc = nullptr;
        _pc = new (nothrow) ParametricCurve3(derivative, spiral_on_cone::u_min, spiral_on_cone::u_max);

        if (!_pc)
        {
            // shieeet
            cout << "_pc is null" << endl;
        }

        GLuint div_point_cout = 200;
        GLenum usage_flag = GL_STATIC_DRAW;

        _image_of_pc = nullptr;
        _image_of_pc = _pc->GenerateImage(div_point_cout, usage_flag);

        if (!_image_of_pc)
        {
            // shieeet
            cout << "_image_of_pc is null" << endl;
        }

        if (!_image_of_pc->UpdateVertexBufferObjects(usage_flag))
        {
            cout << "Could not create the vertex buffer object for parametric curve!" << endl;
        }
    }

    void GLWidget::paintSpiralOnCone(GLuint order)
    {
        if (_image_of_pc)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            _image_of_pc->RenderDerivatives(0, GL_LINE_STRIP);

            glPointSize(5.0f);
                if (order > 1)
                {
                    glColor3f(0.4f, 0.6f, 0.1f);
                    _image_of_pc->RenderDerivatives(1, GL_LINES);
                    _image_of_pc->RenderDerivatives(1, GL_POINTS);
                }

                if (order > 2)
                {
                    glColor3f(1.0f, 0.5f, 0.9f);
                    _image_of_pc->RenderDerivatives(2, GL_LINES);
                    _image_of_pc->RenderDerivatives(2, GL_POINTS);
                }
            glPointSize(1.0f);

        }
    }

    void GLWidget::initTorusKnot(GLuint order)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(order);
        derivative(0) = torus_knot::d0;
        if (order > 1) derivative(1) = torus_knot::d1;
        if (order > 2) derivative(2) = torus_knot::d2;

        _pc = nullptr;
        _pc = new (nothrow) ParametricCurve3(derivative, torus_knot::u_min, torus_knot::u_max);

        if (!_pc)
        {
            // shieeet
            cout << "_pc is null" << endl;
        }

        GLuint div_point_cout = 200;
        GLenum usage_flag = GL_STATIC_DRAW;

        _image_of_pc = nullptr;
        _image_of_pc = _pc->GenerateImage(div_point_cout, usage_flag);

        if (!_image_of_pc)
        {
            // shieeet
            cout << "_image_of_pc is null" << endl;
        }

        if (!_image_of_pc->UpdateVertexBufferObjects(0.75, usage_flag))
        {
            cout << "Could not create the vertex buffer object for parametric curve!" << endl;
        }
    }

    void GLWidget::paintTorusKnot(GLuint order)
    {
        if (_image_of_pc)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            _image_of_pc->RenderDerivatives(0, GL_LINE_STRIP);

            glPointSize(5.0f);
                if (order > 1)
                {
                    glColor3f(0.4f, 0.6f, 0.1f);
                    _image_of_pc->RenderDerivatives(1, GL_LINES);
                    _image_of_pc->RenderDerivatives(1, GL_POINTS);
                }

                if (order > 2)
                {
                    glColor3f(1.0f, 0.5f, 0.9f);
                    _image_of_pc->RenderDerivatives(2, GL_LINES);
                    _image_of_pc->RenderDerivatives(2, GL_POINTS);
                }
            glPointSize(1.0f);

        }
    }

    void GLWidget::initLissajous(GLuint order)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(order);
        derivative(0) = lissajous::d0;
        if (order > 1) derivative(1) = lissajous::d1;
        if (order > 2) derivative(2) = lissajous::d2;

        _pc = nullptr;
        _pc = new (nothrow) ParametricCurve3(derivative, lissajous::u_min, lissajous::u_max);

        if (!_pc)
        {
            // shieeet
            cout << "_pc is null" << endl;
        }

        GLuint div_point_cout = 200;
        GLenum usage_flag = GL_STATIC_DRAW;

        _image_of_pc = nullptr;
        _image_of_pc = _pc->GenerateImage(div_point_cout, usage_flag);

        if (!_image_of_pc)
        {
            // shieeet
            cout << "_image_of_pc is null" << endl;
        }

        if (!_image_of_pc->UpdateVertexBufferObjects(usage_flag))
        {
            cout << "Could not create the vertex buffer object for parametric curve!" << endl;
        }
    }

    void GLWidget::paintLissajous(GLuint order)
    {
        if (_image_of_pc)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            _image_of_pc->RenderDerivatives(0, GL_LINE_STRIP);

            glPointSize(5.0f);
                if (order > 1)
                {
                    glColor3f(0.4f, 0.6f, 0.1f);
                    _image_of_pc->RenderDerivatives(1, GL_LINES);
                    _image_of_pc->RenderDerivatives(1, GL_POINTS);
                }

                if (order > 2)
                {
                    glColor3f(1.0f, 0.5f, 0.9f);
                    _image_of_pc->RenderDerivatives(2, GL_LINES);
                    _image_of_pc->RenderDerivatives(2, GL_POINTS);
                }
            glPointSize(1.0f);

        }
    }

    void GLWidget::initRose(GLuint order)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(order);
        derivative(0) = rose::d0;
        if (order > 1) derivative(1) = rose::d1;
        if (order > 2) derivative(2) = rose::d2;

        _pc = nullptr;
        _pc = new (nothrow) ParametricCurve3(derivative, rose::u_min, rose::u_max);

        if (!_pc)
        {
            // shieeet
            cout << "_pc is null" << endl;
        }

        GLuint div_point_cout = 200;
        GLenum usage_flag = GL_STATIC_DRAW;

        _image_of_pc = nullptr;
        _image_of_pc = _pc->GenerateImage(div_point_cout, usage_flag);

        if (!_image_of_pc)
        {
            // shieeet
            cout << "_image_of_pc is null" << endl;
        }

        if (!_image_of_pc->UpdateVertexBufferObjects(usage_flag))
        {
            cout << "Could not create the vertex buffer object for parametric curve!" << endl;
        }
    }

    void GLWidget::paintRose(GLuint order)
    {
        if (_image_of_pc)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            _image_of_pc->RenderDerivatives(0, GL_LINE_STRIP);

            glPointSize(5.0f);
                if (order > 1)
                {
                    glColor3f(0.4f, 0.6f, 0.1f);
                    _image_of_pc->RenderDerivatives(1, GL_LINES);
                    _image_of_pc->RenderDerivatives(1, GL_POINTS);
                }

                if (order > 2)
                {
                    glColor3f(1.0f, 0.5f, 0.9f);
                    _image_of_pc->RenderDerivatives(2, GL_LINES);
                    _image_of_pc->RenderDerivatives(2, GL_POINTS);
                }
            glPointSize(1.0f);

        }
    }



    //-----------------------------------
    // implementation of the public slots
    //-----------------------------------

    void GLWidget::set_angle_x(int value)
    {
        if (_angle_x != value)
        {
            _angle_x = value;
            updateGL();
        }
    }

    void GLWidget::set_angle_y(int value)
    {
        if (_angle_y != value)
        {
            _angle_y = value;
            updateGL();
        }
    }

    void GLWidget::set_angle_z(int value)
    {
        if (_angle_z != value)
        {
            _angle_z = value;
            updateGL();
        }
    }

    void GLWidget::set_zoom_factor(double value)
    {
        if (_zoom != value)
        {
            _zoom = value;
            updateGL();
        }
    }

    void GLWidget::set_trans_x(double value)
    {
        if (_trans_x != value)
        {
            _trans_x = value;
            updateGL();
        }
    }

    void GLWidget::set_trans_y(double value)
    {
        if (_trans_y != value)
        {
            _trans_y = value;
            updateGL();
        }
    }

    void GLWidget::set_trans_z(double value)
    {
        if (_trans_z != value)
        {
            _trans_z = value;
            updateGL();
        }
    }

    void GLWidget::_animate()
    {
        // For model animation
        GLfloat* vertex = _mouse.MapVertexBuffer(GL_READ_WRITE);
        GLfloat* normal = _mouse.MapNormalBuffer(GL_READ_ONLY);

        _angle += DEG_TO_RADIAN;
        if (_angle >= TWO_PI) _angle -= TWO_PI;

        GLfloat scale = sin(_angle) / 3000.0;
        for (GLuint i = 0; i < _mouse.VertexCount(); ++i)
        {
            for (GLuint coordinate = 0; coordinate < 3; ++coordinate, ++vertex, ++normal)
                *vertex += scale * (*normal);
        }

        _mouse.UnmapVertexBuffer();
        _mouse.UnmapNormalBuffer();

        updateGL();
    }
}
