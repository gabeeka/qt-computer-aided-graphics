#include "GLWidget.h"

#if !defined(__APPLE__)
#include <GL/glu.h>
#endif

#include "../GUI/MainWindow.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
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
        _main_widget = parent;
        srand(time(nullptr));
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
            initOffModel();
            initParametricCurves(5);
            initParametricSurfaces(5);
            initCyclicCurves();

            initSOQAHArc();
            initSOQAHArcComposite();

            initSOQAHPatch();
            initSOQAHPatchComposite();


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

            switch (_render_function) {
                case 0:
                    renderParametricCurve();
                break;
                case 1:
                    renderOffModel();
                break;
                case 2:
                    renderParametricSurface();
                break;
                case 3:
                    renderCyclicCurves();
                break;
                case 4:
                    if (_render_index == 0)
//                        renderSOQAHArc();
                        renderSOQAHArcComposite();
                    else
//                        renderSOQAHPatch();
                        renderSOQAHPatchComposite();
                    break;
            }

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

    void GLWidget::renderParametricCurve()
    {
        glPointSize(5.0);
        glColor3f(1.0, 0.0, 0.0);
        _image_of_pc_vec[_render_index]->RenderDerivatives(0, GL_LINE_STRIP);

        glPointSize(5.0);

        glColor3f(0.0, 0.5, 0.0);
        _image_of_pc_vec[_render_index]->RenderDerivatives(1, GL_LINES);
        _image_of_pc_vec[_render_index]->RenderDerivatives(1, GL_POINTS);

        glColor3f(0.1, 0.5, 0.9);
        _image_of_pc_vec[_render_index]->RenderDerivatives(2, GL_LINES);
        _image_of_pc_vec[_render_index]->RenderDerivatives(2, GL_POINTS);

        glPointSize(1.0);
    }

    // OFF models
    void GLWidget::initOffModel()
    {
        _off_models.resize(3);
        if (_off_models[0].LoadFromOFF("Models/elephant.off", true))
        {
            if (!_off_models[0].UpdateVertexBufferObjects(GL_DYNAMIC_DRAW))
            {
                throw std::runtime_error("Error while loading off model");
            }
        }
        if (_off_models[1].LoadFromOFF("Models/mouse.off", true))
        {
            if (!_off_models[1].UpdateVertexBufferObjects(GL_DYNAMIC_DRAW))
            {
                throw std::runtime_error("Error while loading off model");
            }
        }
        if (_off_models[2].LoadFromOFF("Models/sphere.off", true))
        {
            if (!_off_models[2].UpdateVertexBufferObjects(GL_DYNAMIC_DRAW))
            {
                throw std::runtime_error("Error while loading off model");
            }
        }
        _angle = 0.0;
        _timer->start();
    }

    void GLWidget::renderOffModel()
    {
        MatFBRuby.Apply();
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        _off_models[_render_index].Render();
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

    void GLWidget::renderParametricSurface()
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);

        if(_directionalLight)
        {
            _directionalLight->Enable();
            MatFBTurquoise.Apply();
            if(_image_of_ps_vec[_render_index])
            {
                _image_of_ps_vec[_render_index]->Render();
            }
            _directionalLight->Disable();
        }
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);
    }


    // Cyclic curve
    void GLWidget::initCyclicCurves()
    {
        _cyclic_curve = nullptr;
        _cyclic_curve_interpolation = nullptr;
        _order = 4;
        _cyclic_curve= new CyclicCurve3(_order);
        _cyclic_curve_interpolation = new CyclicCurve3(_order);
        _show_fod_cc = true;
        _show_sod_cc = true;
        _show_control_polygon_cc = true;
        _show_interpolating_cc = true;

        _knot_vector.ResizeRows(2 * _order + 1);
        _data_points_to_interpolate.ResizeRows(2 * _order + 1);


        if(!_cyclic_curve)
        {
            throw Exception("Could not create the cyclic curve!");
        }
        if(!_cyclic_curve_interpolation)
        {
            throw Exception("Could not create the interpolation for the cyclic curve!");
        }

        GLdouble step = TWO_PI/(2 * _order + 1);
        for(GLuint i = 0; i <= 2 * _order; i++)
        {
            GLdouble u = i * step;
            _knot_vector[i] = u;
            DCoordinate3 &cp = (*_cyclic_curve)[i];
            cp[0] = cos(u);
            cp[1] = sin(u);
            cp[2] = -2.0 + 4.0 * (GLdouble)rand() / (GLdouble)RAND_MAX;
            _data_points_to_interpolate[i] = cp;
        }

        _div = 100;
        _max_order = 5;
        _image_of_cc = _cyclic_curve->GenerateImage(_max_order, _div);

        if (!_image_of_cc)
        {
            if (_cyclic_curve)
            {
                delete _cyclic_curve, _cyclic_curve = nullptr;
            }
            throw Exception("Could not generate the image of the cyclic curve!");
        }

        if (!_cyclic_curve->UpdateVertexBufferObjectsOfData())
        {
            if (_cyclic_curve)
            {
                delete _cyclic_curve, _cyclic_curve = nullptr;
            }
            throw Exception("Could not update the VBOs of the cyclic curve's control polygon!");
        }

        if (!_image_of_cc->UpdateVertexBufferObjects())
        {
            if (_cyclic_curve)
            {
                delete _cyclic_curve, _cyclic_curve = nullptr;
            }
            throw Exception("Could not update the VBOs of the cyclic curve's image!");
        }

        // init interpolations
        if (!_cyclic_curve_interpolation->UpdateDataForInterpolation(_knot_vector, _data_points_to_interpolate))
        {
            if (_cyclic_curve_interpolation)
            {
                delete _cyclic_curve_interpolation, _cyclic_curve_interpolation = nullptr;
            }
            throw Exception("Could not update the VBOs of the cyclic curve's interpolation!");
        }

        if (!_cyclic_curve_interpolation->UpdateVertexBufferObjectsOfData())
        {
            if (_cyclic_curve_interpolation)
            {
                delete _cyclic_curve_interpolation, _cyclic_curve_interpolation = nullptr;
            }
            throw Exception("Could not update the VBOs of the cyclic curve's control polygon!-interpolation");
        }

        _div = 100;
        _max_order = 3;
        _image_of_cc_interpolation = _cyclic_curve_interpolation->GenerateImage(_max_order, _div);

        if (!_image_of_cc_interpolation)
        {
            if (_image_of_cc_interpolation)
            {
                delete _image_of_cc_interpolation, _image_of_cc_interpolation = nullptr;
            }
            throw Exception("Could not generate the image of the cyclic curve!-interpolation");
        }

        if (!_image_of_cc_interpolation ->UpdateVertexBufferObjects())
        {
            if(_cyclic_curve_interpolation)
            {
                delete _cyclic_curve_interpolation, _cyclic_curve_interpolation = nullptr;
            }
            throw Exception("Could not update the VBOs of the cyclic curve's image!-inetpolataion");
        }
    }


    void GLWidget::renderCyclicCurves()
    {
        if (_cyclic_curve)
        {
            if (_show_control_polygon_cc)
            {
                glColor3f(1.0f, 0.7f, 0.4f);
                _cyclic_curve->RenderData(GL_LINE_LOOP);
                _cyclic_curve->RenderData(GL_POINTS);
            }
        }

        if (_image_of_cc_interpolation)
        {
            if(_show_interpolating_cc)
            {
                glColor3f(0.5f, 1.0f, 1.0f);
                _image_of_cc_interpolation->RenderDerivatives(0, GL_LINE_LOOP);
            }
        }

        if(_image_of_cc)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            _image_of_cc->RenderDerivatives(0, GL_LINE_LOOP);

            glPointSize(5.0);

            if(_show_fod_cc)
            {
                glColor3f(0.0f, 0.5f, 0.0f);
                _image_of_cc->RenderDerivatives(1, GL_LINES);
                _image_of_cc->RenderDerivatives(1, GL_POINTS);
            }

            if(_show_sod_cc)
            {
                glColor3f(0.1f, 0.5f, 0.9f);
                _image_of_cc->RenderDerivatives(2, GL_LINES);
                _image_of_cc->RenderDerivatives(2, GL_POINTS);
            }

        glPointSize(1.0);
        }
    }

    void GLWidget::initSOQAHPatch()
    {
        _patch = new SOQAHPatch3();

        _patch->SetData(0, 0, -2.0, -2.0, 0.0);
        _patch->SetData(0, 1, -2.0, -1.0, 0.0);
        _patch->SetData(0, 2, -2.0,  1.0, 0.0);
        _patch->SetData(0, 3, -2.0,  2.0, 0.0);

        _patch->SetData(1, 0, -1.0, -2.0, 0.0);
        _patch->SetData(1, 1, -1.0, -1.0, 2.0);
        _patch->SetData(1, 2, -1.0,  1.0, 2.0);
        _patch->SetData(1, 3, -1.0,  2.0, 0.0);

        _patch->SetData(2, 0, 1.0, -2.0, 0.0);
        _patch->SetData(2, 1, 1.0, -1.0, 2.0);
        _patch->SetData(2, 2, 1.0,  1.0, 2.0);
        _patch->SetData(2, 3, 1.0,  2.0, 0.0);

        _patch->SetData(3, 0, 2.0, -2.0, 0.0);
        _patch->SetData(3, 1, 2.0, -1.0, 0.0);
        _patch->SetData(3, 2, 2.0,  1.0, 0.0);
        _patch->SetData(3, 3, 2.0,  2.0, 0.0);

        _u_lines = _patch->GenerateUIsoparametricLines(3, 1, 30);
        _v_lines = _patch->GenerateVIsoparametricLines(3, 1, 30);

        for(GLuint i = 0; i < _u_lines->GetColumnCount(); i++)
        {

            if((*_u_lines)[i])
            {
                (*_u_lines)[i]->UpdateVertexBufferObjects();
            }
        }

        for(GLuint i = 0; i < _v_lines->GetColumnCount(); i++)
        {
            if((*_v_lines)[i])
            {
                (*_v_lines)[i]->UpdateVertexBufferObjects();
            }
        }

        // generate the mesh of the surface patch
        _before_interpolation = _patch->GenerateImage(30, 30, GL_STATIC_DRAW);

        if(_before_interpolation)
        {
            _before_interpolation->UpdateVertexBufferObjects();
        }

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
        if(_patch->UpdateDataForInterpolation(u_knot_vector, v_knot_vector, data_points_to_interpolate))
        {
            _after_interpolation = _patch->GenerateImage(30, 30, GL_STATIC_DRAW);

            if(_after_interpolation)
            {
                _after_interpolation->UpdateVertexBufferObjects();
            }
        }
    }

    void GLWidget::renderSOQAHPatch()
    {
        if(_before_interpolation)
        {
            MatFBRuby.Apply();
            _before_interpolation->Render();
        }

        if(_after_interpolation)
        {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_NORMALIZE);

            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            MatFBTurquoise.Apply();
            _after_interpolation->Render();
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }

        for(GLuint i = 0; i < _u_lines->GetColumnCount(); i++)
        {
            if((*_u_lines)[i])
            {
                (*_u_lines)[i]->RenderDerivatives(0, GL_LINE_STRIP);
            }
        }

        for(GLuint i = 0; i < _u_lines->GetColumnCount(); i++)
        {
            if((*_u_lines)[i])
            {
                (*_u_lines)[i]->RenderDerivatives(1, GL_LINES);
            }
        }

        for(GLuint i = 0; i < _v_lines->GetColumnCount(); i++)
        {
            if((*_v_lines)[i])
            {
                (*_v_lines)[i]->RenderDerivatives(0, GL_LINE_STRIP);
            }
        }
    }

    void GLWidget::initSOQAHPatchComposite()
    {
        _soqah_patch_composite = new SOQAHCompositeSurface3();
        addNewSOQAHPatch();
        updatePatchIndex(0);
    }

    void GLWidget::renderSOQAHPatchComposite()
    {
        _soqah_patch_composite->RenderPatches(_render_control_net);
    }

    void GLWidget::addNewSOQAHPatch()
    {
        auto* patch = _soqah_patch_composite->AppendPatch();
        auto index = static_cast<GLuint>(_soqah_patch_composite->GetPatchCount()- 1);

        patch->_patch->SetData(0, 0, -2.0, -2.0 + index * 8.0, 0.0);
        patch->_patch->SetData(0, 1, -2.0, -1.0 + index * 8.0, 0.0);
        patch->_patch->SetData(0, 2, -2.0,  1.0 + index * 8.0, 0.0);
        patch->_patch->SetData(0, 3, -2.0,  2.0 + index * 8.0, 0.0);

        patch->_patch->SetData(1, 0, -1.0, -2.0 + index * 8.0, 0.0);
        patch->_patch->SetData(1, 1, -1.0, -1.0 + index * 8.0, 2.0);
        patch->_patch->SetData(1, 2, -1.0,  1.0 + index * 8.0, 2.0);
        patch->_patch->SetData(1, 3, -1.0,  2.0 + index * 8.0, 0.0);

        patch->_patch->SetData(2, 0, 1.0, -2.0 + index * 8.0, 0.0);
        patch->_patch->SetData(2, 1, 1.0, -1.0 + index * 8.0, 2.0);
        patch->_patch->SetData(2, 2, 1.0,  1.0 + index * 8.0, 2.0);
        patch->_patch->SetData(2, 3, 1.0,  2.0 + index * 8.0, 0.0);

        patch->_patch->SetData(3, 0, 2.0, -2.0 + index * 8.0, 0.0);
        patch->_patch->SetData(3, 1, 2.0, -1.0 + index * 8.0, 0.0);
        patch->_patch->SetData(3, 2, 2.0,  1.0 + index * 8.0, 0.0);
        patch->_patch->SetData(3, 3, 2.0,  2.0 + index * 8.0, 0.0);


        // update patches
        _soqah_patch_composite->UpdatePatches();
    }

    void GLWidget::addNewPatch()
    {
        addNewSOQAHPatch();
    }

    void GLWidget::resetPatches()
    {
        initSOQAHPatchComposite();
    }

    void GLWidget::initSOQAHArc()
    {
        _soqah_arc = new SOQAHArcs3();

        // add control points
        (*_soqah_arc )[0] = DCoordinate3( 1.0,  0.0, 0.0);
        (*_soqah_arc )[1] = DCoordinate3( 1.0,  1.0, 0.0);
        (*_soqah_arc )[2] = DCoordinate3(-1.0,  1.0, 0.0);
        (*_soqah_arc )[3] = DCoordinate3(-1.0, -1.0, 0.0);

        _soqah_arc->UpdateVertexBufferObjectsOfData();

        _image_of_soqah_arc = _soqah_arc->GenerateImage(3, 40, GL_STATIC_DRAW);
        if (!_image_of_soqah_arc) throw std::runtime_error("Failed to generate image for Arc!");
        // cout << *_image_of_soqah_arc << endl;
        _image_of_soqah_arc->UpdateVertexBufferObjects(GL_STATIC_DRAW);
    }

    void GLWidget::renderSOQAHArc()
    {
        glDisable(GL_LIGHTING);
        glColor3f(0.8f, 0.0f, 0.0f);
        _soqah_arc->RenderData();
        if (_image_of_soqah_arc)
        {
            glColor3f(0.3f, 0.5f, 0.0f);
            _image_of_soqah_arc->RenderDerivatives(0, GL_LINE_STRIP);

            glColor3f(0.2f, 0.5f, 0.7f);
            _image_of_soqah_arc->RenderDerivatives(1, GL_LINES);


            glColor3f(0.7f, 0.5f, 0.9f);
            _image_of_soqah_arc->RenderDerivatives(2, GL_LINES);

        }
    }

    void GLWidget::initSOQAHArcComposite()
    {
        _soqah_arc_composite = new SOQAHCompositeCurve3(500);
        addNewSOQAHArc();
        updateSOQAHArcComposite();
        arcIndexChanged(0);
    }

    void GLWidget::renderSOQAHArcComposite()
    {
        (void)_soqah_arc_composite->Render(_render_first_order, _render_second_order, _render_control_points);
    }

    void GLWidget::updateSOQAHArcComposite()
    {
        _soqah_arc_composite->UpdateVBODatas();
        _soqah_arc_composite->GenerateImages(2, 40);
        _soqah_arc_composite->UpdateVBOs();
    }

    void GLWidget::addNewSOQAHArc()
    {
        auto* arc1 = _soqah_arc_composite->AppendArc();
        auto index = static_cast<GLuint>(_soqah_arc_composite->GetArcCount() - 1);

        _soqah_arc_composite->GetArcPoint(index, 0) = DCoordinate3(  1.0,   0.0,  index * 5.0);
        _soqah_arc_composite->GetArcPoint(index, 1) = DCoordinate3(  1.0,   1.0,  index * 5.0);
        _soqah_arc_composite->GetArcPoint(index, 2) = DCoordinate3( -1.0,   1.0,  index * 5.0);
        _soqah_arc_composite->GetArcPoint(index, 3) = DCoordinate3( -1.0,  -1.0,  index * 5.0);

        arc1->_arc_color    = Color4((static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)));
        arc1->_img_color_0  = Color4((static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)));
        arc1->_img_color_1  = Color4((static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)));
        arc1->_img_color_2  = Color4((static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)), (static_cast<float>(rand()) / (RAND_MAX)));


        // Update stuff
        updateSOQAHArcComposite();
    }

    void GLWidget::addNewArc()
    {
        addNewSOQAHArc();
    }

    void GLWidget::resetArcs()
    {
        initSOQAHArcComposite();
    }

    void GLWidget::updateArcJoinIndex1(int value)
    {
        _joinIndex1 = value;
    }


    void GLWidget::updateArcJoinIndex2(int value)
    {
        _joinIndex2 = value;
    }

    void GLWidget::updateArcJoinDir1(int value)
    {
        if (value == 0)
            _joinDirection1 = SOQAHCompositeCurve3::Direction::LEFT;
        else
            _joinDirection1 = SOQAHCompositeCurve3::Direction::RIGHT;
    }

    void GLWidget::updateArcJoinDir2(int value)
    {
        if (value == 0)
            _joinDirection2 = SOQAHCompositeCurve3::Direction::LEFT;
        else
            _joinDirection2 = SOQAHCompositeCurve3::Direction::RIGHT;
    }

    void GLWidget::joinArcs()
    {
        _soqah_arc_composite->JoinArcs(_joinIndex1, _joinDirection1, _joinIndex2, _joinDirection2);
        updateSOQAHArcComposite();
    }

    void GLWidget::mergeArcs()
    {
        _soqah_arc_composite->MergeArcs(_joinIndex1, _joinDirection1, _joinIndex2, _joinDirection2);
        updateSOQAHArcComposite();
    }

    void GLWidget::continueArc()
    {
        _soqah_arc_composite->Continue(_joinIndex1, _joinDirection1);
        updateSOQAHArcComposite();
    }

    void GLWidget::updatePatchIndex(int value)
    {
        if (value >= _soqah_patch_composite->GetPatchCount())
        {
            return;
        }
        _patch_index = value;
        auto* widget = reinterpret_cast<MainWindow*>(_main_widget);
        DCoordinate3 point;
        _soqah_patch_composite->GetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);

        widget->_side_widget->p_cp_x_coord->setValue(point.x());
        widget->_side_widget->p_cp_y_coord->setValue(point.y());
        widget->_side_widget->p_cp_z_coord->setValue(point.z());
    }

    void GLWidget::updatePatchCpIndex1(int value)
    {
        _p_cp_index_1 = value;
        auto* widget = reinterpret_cast<MainWindow*>(_main_widget);
        DCoordinate3 point;
        _soqah_patch_composite->GetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);

        widget->_side_widget->p_cp_x_coord->setValue(point.x());
        widget->_side_widget->p_cp_y_coord->setValue(point.y());
        widget->_side_widget->p_cp_z_coord->setValue(point.z());
    }

    void GLWidget::updatePatchCpIndex2(int value)
    {
        _p_cp_index_2 = value;
        auto* widget = reinterpret_cast<MainWindow*>(_main_widget);
        DCoordinate3 point;
        _soqah_patch_composite->GetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);

        widget->_side_widget->p_cp_x_coord->setValue(point.x());
        widget->_side_widget->p_cp_y_coord->setValue(point.y());
        widget->_side_widget->p_cp_z_coord->setValue(point.z());
    }

    void GLWidget::updatePatchCpXCoord(double value)
    {
        DCoordinate3 point;
        _soqah_patch_composite->GetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);
        point.x()=value;
        _soqah_patch_composite->SetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);
        _soqah_patch_composite->RefreshNeighbours(_patch_index);
        _soqah_patch_composite->UpdatePatches();
    }

    void GLWidget::updatePatchCpYCoord(double value)
    {
        DCoordinate3 point;
        _soqah_patch_composite->GetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);
        point.y()=value;
        _soqah_patch_composite->SetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);
        _soqah_patch_composite->RefreshNeighbours(_patch_index);
        _soqah_patch_composite->UpdatePatches();
    }

    void GLWidget::updatePatchCpZCoord(double value)
    {
        DCoordinate3 point;
        _soqah_patch_composite->GetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);
        point.z()=value;
        _soqah_patch_composite->SetPatchPoint(_patch_index, _p_cp_index_1, _p_cp_index_2, point);
        _soqah_patch_composite->RefreshNeighbours(_patch_index);
        _soqah_patch_composite->UpdatePatches();
    }

    void GLWidget::updateRenderControlNet(int value)
    {
        _render_control_net = static_cast<GLboolean>(value);
    }

    void GLWidget::updateMaterial(int index)
    {
        _soqah_patch_composite->SetMaterialIndex(_patch_index, index);
    }

    void GLWidget::updatePatchIndex1(int value)
    {
        _patchIndex1 = value;
    }


    void GLWidget::updatePatchIndex2(int value)
    {
        _patchIndex2 = value;
    }

    void GLWidget::updatePatchDir1(int value)
    {
        switch (value) {
        case 0:
            _patchDirection1 = SOQAHCompositeSurface3::Direction::NORTH;
            break;
        case 1:
            _patchDirection1 = SOQAHCompositeSurface3::Direction::EAST;
            break;
        case 2:
            _patchDirection1 = SOQAHCompositeSurface3::Direction::SOUTH;
            break;
        case 3:
            _patchDirection1 = SOQAHCompositeSurface3::Direction::WEST;
            break;
        }
    }

    void GLWidget::updatePatchDir2(int value)
    {
        switch (value) {
        case 0:
            _patchDirection2 = SOQAHCompositeSurface3::Direction::NORTH;
            break;
        case 1:
            _patchDirection2 = SOQAHCompositeSurface3::Direction::EAST;
            break;
        case 2:
            _patchDirection2 = SOQAHCompositeSurface3::Direction::SOUTH;
            break;
        case 3:
            _patchDirection2 = SOQAHCompositeSurface3::Direction::WEST;
            break;
        }
    }

    void GLWidget::joinPatches()
    {
        _soqah_patch_composite->JoinPatches(_patchIndex1, _patchDirection1, _patchIndex2, _patchDirection2);
        _soqah_patch_composite->UpdatePatches();
    }

    void GLWidget::mergePatches()
    {
        _soqah_patch_composite->MergePatches(_patchIndex1, _patchDirection1, _patchIndex2, _patchDirection2);
        _soqah_patch_composite->UpdatePatches();
    }

    void GLWidget::continuePatch()
    {
        _soqah_patch_composite->ContinuePatch(_patchIndex1, _patchDirection1);
        _soqah_patch_composite->UpdatePatches();
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
    void GLWidget::render_cyclic_curve()
    {
        render_setup(3, 0);
    }

    // project
    void GLWidget::renderArc()
    {
        render_setup(4, 0);
    }
    void GLWidget::renderPatch()
    {
        render_setup(4, 1);
    }

    // render setup
    void GLWidget::render_setup(GLuint render_function, GLuint render_index)
    {
        _render_function = render_function;
        _render_index    = render_index;
        updateGL();
    }

    // shader setup
    void GLWidget::installShader()
    {

        std::string shader_vert;
        std::string shader_frag;

        switch(_shader_index)
        {
        case 0:
            {
                shader_vert = "./Shaders/directional_light.vert";
                shader_frag = "./Shaders/directional_light.frag";
                break;
            }
        case 1:
            {
                shader_vert = "./Shaders/reflection_lines.vert";
                shader_frag = "./Shaders/reflection_lines.frag";
                break;
            }
        case 2:
            {
                shader_vert = "./Shaders/toon.vert";
                shader_frag = "./Shaders/toon.frag";
                break;
            }
        case 3:
            {
                shader_vert = "./Shaders/two_sided_lighting.vert";
                shader_frag = "./Shaders/two_sided_lighting.frag";
                break;
            }
        case 4:
            {
                _shader.Disable();
                return;
            }
        }

        try {
            if(!_shader.InstallShaders(shader_vert, shader_frag, GL_FALSE))
            {
                // error
            }
            _shader.Enable();
            // if shader is reflection lines
            if (shader_vert == "./Shaders/reflection_lines.vert")
            {
                _shader.SetUniformVariable1f("scale_factor", 8.0);
                _shader.SetUniformVariable1f("shading", 0.5);
                _shader.SetUniformVariable1f("smoothing", 1.0);
            }
        } catch (Exception &e) {
            cerr << e << endl;
        }
        updateGL();
    }

    void GLWidget::initDirectionalLight()
    {
        _shader_index = 0;
        installShader();
    }

    void GLWidget::initReflectionLines()
    {
        _shader_index = 1;
        installShader();
    }

    void GLWidget::initToon()
    {
        _shader_index = 2;
        installShader();
    }

    void GLWidget::initTwoSidedLighting()
    {
        _shader_index = 3;
        installShader();
    }

    void GLWidget::disableShader()
    {
        _shader_index = 4;
        installShader();
    }

    void GLWidget::set_scale_factor(double value)
    {
        if (_scale_factor != value)
        {
            _scale_factor = value;
            _shader.SetUniformVariable1f("scale_factor", _scale_factor);
            updateGL();
        }
    }

    void GLWidget::set_smoothing(double value)
    {
        if (_smoothing != value)
        {
            _smoothing = value;
            _shader.SetUniformVariable1f("smoothing", _smoothing);
            updateGL();
        }
    }

    void GLWidget::set_shading(double value)
    {
        if (_shading != value)
        {
            _shading = value;
            _shader.SetUniformVariable1f("shading", _shading);
            updateGL();
        }
    }

    void GLWidget::set_red_default_outline_color(double value)
    {
        if (_red_default_outline_color != value)
        {
            _red_default_outline_color = value;
            _shader.SetUniformVariable4f("default_outline_color", _red_default_outline_color, _green_default_outline_color, _blue_default_outline_color, 1.0f);
            updateGL();
        }
    }

    void GLWidget::set_green_default_outline_color(double value)
    {
        if (_green_default_outline_color != value)
        {
            _green_default_outline_color = value;
            _shader.SetUniformVariable4f("default_outline_color", _red_default_outline_color, _green_default_outline_color, _blue_default_outline_color, 1.0f);
            updateGL();
        }
    }

    void GLWidget::set_blue_default_outline_color(double value)
    {
        if (_blue_default_outline_color != value)
        {
            _blue_default_outline_color = value;
            _shader.SetUniformVariable4f("default_outline_color", _red_default_outline_color, _green_default_outline_color, _blue_default_outline_color, 1.0f);
            updateGL();
        }
    }

    void GLWidget::arcIndexChanged(int value)
    {
        if (value >= _soqah_arc_composite->GetArcCount())
        {
            return;
        }
        _arc_index = value;
        cpIndexChanged(0);
    }

    void GLWidget::cpIndexChanged(int value)
    {
        if (_arc_index >= _soqah_arc_composite->GetArcCount())
        {
            return;
        }
        _cp_index = value;
        auto* widget = reinterpret_cast<MainWindow*>(_main_widget);
        widget->_side_widget->cp_x_coord->setValue(_soqah_arc_composite->GetArcPoint(_arc_index, _cp_index).x());
        widget->_side_widget->cp_y_coord->setValue(_soqah_arc_composite->GetArcPoint(_arc_index, _cp_index).y());
        widget->_side_widget->cp_z_coord->setValue(_soqah_arc_composite->GetArcPoint(_arc_index, _cp_index).z());
    }

    void GLWidget::updateCpXCoord(double value)
    {
        _soqah_arc_composite->GetArcPoint(_arc_index, _cp_index).x()=value;
        _soqah_arc_composite->RefreshNeighbours(_arc_index);
        updateSOQAHArcComposite();
    }

    void GLWidget::updateCpYCoord(double value)
    {
        _soqah_arc_composite->GetArcPoint(_arc_index, _cp_index).y()=value;
        _soqah_arc_composite->RefreshNeighbours(_arc_index);
        updateSOQAHArcComposite();
    }

    void GLWidget::updateCpZCoord(double value)
    {
        _soqah_arc_composite->GetArcPoint(_arc_index, _cp_index).z()=value;
        _soqah_arc_composite->RefreshNeighbours(_arc_index);
        updateSOQAHArcComposite();
    }

    void GLWidget::updateRenderFirstOrder(int value)
    {
        _render_first_order = static_cast<GLboolean>(value);
    }

    void GLWidget::updateRenderSecondOrder(int value)
    {
        _render_second_order = static_cast<GLboolean>(value);
    }

    void GLWidget::updateRenderControlPoints(int value)
    {
        _render_control_points = static_cast<GLboolean>(value);
    }

    void GLWidget::_animate()
    {

        if (_render_function != 1)
        {
            updateGL();
            return;
        }
        // For model animation
        GLfloat* vertex = _off_models[_render_index].MapVertexBuffer(GL_READ_WRITE);
        GLfloat* normal = _off_models[_render_index].MapNormalBuffer(GL_READ_ONLY);

        _angle += DEG_TO_RADIAN;
        if (_angle >= TWO_PI) _angle -= TWO_PI;

        GLfloat scale = sin(_angle) / 3000.0;
        for (GLuint i = 0; i < _off_models[_render_index].VertexCount(); ++i)
        {
            for (GLuint coordinate = 0; coordinate < 3; ++coordinate, ++vertex, ++normal)
                *vertex += scale * (*normal);
        }

        _off_models[_render_index].UnmapVertexBuffer();
        _off_models[_render_index].UnmapNormalBuffer();

        updateGL();
    }
}
