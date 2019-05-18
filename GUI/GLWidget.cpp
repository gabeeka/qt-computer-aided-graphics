#include "GLWidget.h"

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

        for(GLuint i = 0; i < 5; ++i)
        {
            if (_pc_vec[i])
                delete _pc_vec[i], _pc_vec[i] = 0;

            if (_image_of_pc_vec[i])
                delete _image_of_pc_vec[i], _image_of_pc_vec[i] = 0;
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
            initOffModel("Models/mouse.off");
            initParametricCurves(5);
            initParametricSurfaces(5);





            HCoordinate3 direction(0.0, 0.0, 1.0, 0.0);
            Color4 ambient(0.4, 0.4, 0.4, 1.0);
            Color4 diffuse(0.8, 0.8, 0.8, 1.0);
            Color4 specular(1.0, 1.0, 1.0, 1.0);
            _directionalLight = new DirectionalLight(GL_LIGHT0,direction,ambient,diffuse,specular);
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

            // renderOffModel();
            // renderParametricCurve(4);
            renderParametricSurface(4);

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
    void GLWidget::initParametricCurves(GLuint curve_count)
    {
        _pc_vec.resize(curve_count);
        _image_of_pc_vec.resize(curve_count);

        initSpiralOnCone(200, GL_STATIC_DRAW);
        initTorus(200, GL_STATIC_DRAW);
        initElliplse(200, GL_STATIC_DRAW);
        initHypo(200, GL_STATIC_DRAW);
        initLissajou(200, GL_STATIC_DRAW);

        for(GLuint i = 0; i < _image_of_pc_vec.size(); ++i)
        {
            if(_image_of_pc_vec[i])
            {
                _image_of_pc_vec[i]->UpdateVertexBufferObjects();
            }
        }
    }

    void GLWidget::initSpiralOnCone(GLuint div_point_count, GLenum usage_flag)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(3);
        derivative(0) = spiral_on_cone::d0;
        derivative(1) = spiral_on_cone::d1;
        derivative(2) = spiral_on_cone::d2;
        _pc_vec[0] = 0;
        _pc_vec[0] = new ParametricCurve3(derivative, spiral_on_cone::u_min, spiral_on_cone::u_max);
        _image_of_pc_vec[0] = 0;
        _image_of_pc_vec[0] = _pc_vec[0]->GenerateImage(div_point_count,usage_flag);
    }

    void GLWidget::initTorus(GLuint div_point_count, GLenum usage_flag)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(2);
        derivative(0) = torus::d0;
        derivative(1) = torus::d1;
        _pc_vec[1] = 0;
        _pc_vec[1] = new ParametricCurve3(derivative, torus::u_min, torus::u_max);
        _image_of_pc_vec[1] = 0;
        _image_of_pc_vec[1] = _pc_vec[1]->GenerateImage(div_point_count,usage_flag);
    }

    void GLWidget::initElliplse(GLuint div_point_count, GLenum usage_flag)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(1);
        derivative(0) = ellipse::d0;
        _pc_vec[2] = 0;
        _pc_vec[2] = new ParametricCurve3(derivative, ellipse::u_min, ellipse::u_max);
        _image_of_pc_vec[2] = 0;
        _image_of_pc_vec[2] = _pc_vec[2]->GenerateImage(div_point_count,usage_flag);
    }

    void GLWidget::initHypo(GLuint div_point_count, GLenum usage_flag)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(3);
        derivative(0) = hypo::d0;
        derivative(1) = hypo::d1;
        derivative(2) = hypo::d2;
        _pc_vec[3] = 0;
        _pc_vec[3] = new ParametricCurve3(derivative, hypo::u_min, hypo::u_max);
        _image_of_pc_vec[3] = 0;
        _image_of_pc_vec[3] = _pc_vec[3]->GenerateImage(div_point_count,usage_flag);
    }

    void GLWidget::initLissajou(GLuint div_point_count, GLenum usage_flag)
    {
        RowMatrix<ParametricCurve3::Derivative> derivative(3);
        derivative(0) = lissajou::d0;
        derivative(1) = lissajou::d1;
        derivative(2) = lissajou::d2;
        _pc_vec[4] = 0;
        _pc_vec[4] = new ParametricCurve3(derivative, lissajou::u_min, lissajou::u_max);
        _image_of_pc_vec[4] = 0;
        _image_of_pc_vec[4] = _pc_vec[4]->GenerateImage(div_point_count,usage_flag);
    }

    void GLWidget::renderParametricCurve(GLuint curve_index)
    {
        glPointSize(5.0);
        glColor3f(1.0, 0.0, 0.0);
        _image_of_pc_vec[curve_index]->RenderDerivatives(0, GL_LINE_STRIP);

        glPointSize(5.0);

        glColor3f(0.0, 0.5, 0.0);
        _image_of_pc_vec[curve_index]->RenderDerivatives(1, GL_LINES);
        _image_of_pc_vec[curve_index]->RenderDerivatives(1, GL_POINTS);

        glColor3f(0.1, 0.5, 0.9);
        _image_of_pc_vec[curve_index]->RenderDerivatives(2, GL_LINES);
        _image_of_pc_vec[curve_index]->RenderDerivatives(2, GL_POINTS);

        glPointSize(1.0);
    }

    // OFF models
    void GLWidget::initOffModel(const char* file_name)
    {
        if (_mouse.LoadFromOFF(file_name, true))
        {
            if (_mouse.UpdateVertexBufferObjects(GL_DYNAMIC_DRAW))
            {
                _angle = 0.0;
                _timer->start();
            }
        }
    }

    void GLWidget::renderOffModel()
    {
        MatFBRuby.Apply();
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        _mouse.Render();
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE);
    }


    // Parametric surfaces
    void GLWidget::initParametricSurfaces(GLuint surface_count)
    {
        _ps_vec.resize(surface_count);
        _image_of_ps_vec.resize(surface_count);

        initSphere(200, 200, GL_STATIC_DRAW);
        initChelicoid(200, 200, GL_STATIC_DRAW);
        initHyperboloid(200, 200, GL_STATIC_DRAW);
        initKleinbottle(200, 200, GL_STATIC_DRAW);
        initDupincyclide(200, 200, GL_STATIC_DRAW);

        for(GLuint i=0; i<_image_of_ps_vec.size();i++)
        {
            if(_image_of_ps_vec[i])
            {
                _image_of_ps_vec[i]->UpdateVertexBufferObjects();
            }
        }
    }

    void GLWidget::initSphere(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag)
    {
        TriangularMatrix<ParametricSurface3::PartialDerivative> derivative(3);
        derivative(0,0)=sphere::d00;
        derivative(1,0)=sphere::d10;
        derivative(1,1)=sphere::d01;
        _ps_vec[0] = new ParametricSurface3(derivative,sphere::u_min,sphere::u_max,sphere::v_min,sphere::v_max);
        _image_of_ps_vec[0] = nullptr;
        _image_of_ps_vec[0] = _ps_vec[0]->GenerateImage(u_div_point_count,v_div_point_count,usage_flag);
    }

    void GLWidget::initChelicoid(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag)
    {
        TriangularMatrix<ParametricSurface3::PartialDerivative> derivative(3);
        derivative(0,0)=chelicoid::d00;
        derivative(1,0)=chelicoid::d10;
        derivative(1,1)=chelicoid::d01;
        _ps_vec[1] = new ParametricSurface3(derivative,chelicoid::u_min,chelicoid::u_max,chelicoid::v_min,chelicoid::v_max);
        _image_of_ps_vec[1] = nullptr;
        _image_of_ps_vec[1] = _ps_vec[1]->GenerateImage(u_div_point_count,v_div_point_count,usage_flag);
    }

    void GLWidget::initHyperboloid(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag)
    {
        TriangularMatrix<ParametricSurface3::PartialDerivative> derivative(3);
        derivative(0,0)=hyperboloid::d00;
        derivative(1,0)=hyperboloid::d10;
        derivative(1,1)=hyperboloid::d01;
        _ps_vec[2] = new ParametricSurface3(derivative,hyperboloid::u_min,hyperboloid::u_max,hyperboloid::v_min,hyperboloid::v_max);
        _image_of_ps_vec[2] = nullptr;
        _image_of_ps_vec[2] = _ps_vec[2]->GenerateImage(u_div_point_count,v_div_point_count,usage_flag);
    }

    void GLWidget::initKleinbottle(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag)
    {
        TriangularMatrix<ParametricSurface3::PartialDerivative> derivative(3);
        derivative(0,0)=kleinbottle::d00;
        derivative(1,0)=kleinbottle::d10;
        derivative(1,1)=kleinbottle::d01;
        _ps_vec[3] = new ParametricSurface3(derivative,kleinbottle::u_min,kleinbottle::u_max,kleinbottle::v_min,kleinbottle::v_max);
        _image_of_ps_vec[3] = nullptr;
        _image_of_ps_vec[3] = _ps_vec[3]->GenerateImage(u_div_point_count,v_div_point_count,usage_flag);
    }

    void GLWidget::initDupincyclide(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag)
    {
        TriangularMatrix<ParametricSurface3::PartialDerivative> derivative(3);
        derivative(0,0)=dupincyclide::d00;
        derivative(1,0)=dupincyclide::d10;
        derivative(1,1)=dupincyclide::d01;
        _ps_vec[4] = new ParametricSurface3(derivative,dupincyclide::u_min,dupincyclide::u_max,dupincyclide::v_min,dupincyclide::v_max);
        _image_of_ps_vec[4] = nullptr;
        _image_of_ps_vec[4] = _ps_vec[4]->GenerateImage(u_div_point_count,v_div_point_count,usage_flag);
    }

    void GLWidget::renderParametricSurface(GLuint surface_index)
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);

        if(_directionalLight)
        {
            _directionalLight->Enable();
            MatFBTurquoise.Apply();
            if(_image_of_ps_vec[surface_index])
            {
                _image_of_ps_vec[surface_index]->Render();
            }
            _directionalLight->Disable();
        }
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);
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

    // models
    void GLWidget::render_elephant()
    {
        render_setup(1, 0);
    }
    void GLWidget::render_mouse()
    {
        render_setup(1, 1);
    }
    void GLWidget::render_model_sphere()
    {
        render_setup(1, 2);
    }

    // curves
    void GLWidget::render_spiral_on_cone()
    {
        render_setup(0, 0);
    }
    void GLWidget::render_torus()
    {
        render_setup(0, 1);
    }
    void GLWidget::render_ellipse()
    {
        render_setup(0, 2);
    }
    void GLWidget::render_hypo()
    {
        render_setup(0, 3);
    }
    void GLWidget::render_lissajou()
    {
        render_setup(0, 4);
    }

    // surfaces
    void GLWidget::render_sphere()
    {
        render_setup(2, 0);
    }
    void GLWidget::render_chelicoid()
    {
        render_setup(2, 1);
    }
    void GLWidget::render_hyperboloid()
    {
        render_setup(2, 2);
    }
    void GLWidget::render_kleinbottle()
    {
        render_setup(2, 3);
    }
    void GLWidget::render_dupincycle()
    {
        render_setup(2, 4);
    }

    // render setup
    void GLWidget::render_setup(GLuint render_function, GLuint render_index)
    {
        _render_function = render_function;
        _render_index    = render_index;
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
