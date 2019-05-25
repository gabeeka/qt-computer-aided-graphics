#pragma once

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLFormat>

#include "../Core/Materials.h"
#include "../Core/Constants.h"

#include "../Core/DCoordinates3.h"
#include "../Core/GenericCurves3.h"
#include "../Core/TriangulatedMeshes3.h"
#include "../Parametric/ParametricCurves3.h"
#include "../Parametric/ParametricSurfaces3.h"
#include "../Core/Lights.h"
#include "../Cyclic/CyclicCurves3.h"
#include "../Core/ShaderPrograms.h"

#include "../SOQAH/SOQAHArcs3.h"

#include <QTimer>

namespace cagd
{
    class GLWidget: public QGLWidget
    {
        Q_OBJECT

    private:
        GLuint      _render_function{1};
        GLuint      _render_index{0};

        // variables defining the projection matrix
        double       _aspect;            // aspect ratio of the rendering window
        double       _fovy;              // field of view in direction y
        double       _z_near, _z_far;    // distance of near and far clipping planes

        // variables defining the model-view matrix
        double       _eye[3], _center[3], _up[3];

        // variables needed by transformations
        int         _angle_x, _angle_y, _angle_z;
        double      _zoom;
        double      _trans_x, _trans_y, _trans_z;

        // your other declarations
        GLuint                      _triangle_count;
        std::vector<DCoordinate3>   _cloud;
        GLuint                      _dl;


        // Prametric curves
        std::vector<ParametricCurve3*>          _pc_vec;
        std::vector<GenericCurve3*>             _image_of_pc_vec;


        // Models - off files
        QTimer*                                 _timer;
        GLdouble                                _angle;
        std::vector<TriangulatedMesh3>          _off_models;


        // Parametric surfaces
        std::vector<cagd::ParametricSurface3*>  _ps_vec;
        std::vector<cagd::TriangulatedMesh3*>   _image_of_ps_vec;


        // Cyclic curves
        CyclicCurve3*               _cyclic_curve;
        GenericCurve3*              _image_of_cc;
        CyclicCurve3*               _cyclic_curve_interpolation;
        GenericCurve3*              _image_of_cc_interpolation;
        GLuint                      _order;
        bool                        _show_fod, _show_sod, _show_fod_cc, _show_sod_cc;
        bool                        _show_control_polygon_cc, _show_interpolating_cc;
        ColumnMatrix<GLdouble>      _knot_vector;
        ColumnMatrix<DCoordinate3>  _data_points_to_interpolate;
        GLuint                      _div;
        GLuint                      _max_order;


        // SOQAH arc declarations
        SOQAHArcs3*       _soqah_arc;
        GenericCurve3*    _image_of_soqah_arc;


        // Lighting
        DirectionalLight*                       _directionalLight;


        // Private member functions

        // ParametricCurves
        void initParametricCurves(GLuint curve_count);
        void initSpiralOnCone(GLuint div_point_count, GLenum usage_flag);
        void initTorus(GLuint div_point_count, GLenum usage_flag);
        void initElliplse(GLuint div_point_count, GLenum usage_flag);
        void initHypo(GLuint div_point_count, GLenum usage_flag);
        void initLissajou(GLuint div_point_count, GLenum usage_flag);
        void renderParametricCurve();


        // OFF models
        void initOffModel();
        void renderOffModel();


        // Parametric surfaces
        void initParametricSurfaces(GLuint surface_count);
        void initSphere(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initChelicoid(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initHyperboloid(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initKleinbottle(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initDupincyclide(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void renderParametricSurface();


        // Cyclic curves
        void initCyclicCurves();
        void renderCyclicCurves();


        // SOQAH Arc
        void initSOQAH();
        void renderSOQAH();


        // Shaders
        GLuint          _shader_index{0};
        ShaderProgram   _shader;

    public:
        // special and default constructor
        // the format specifies the properties of the rendering window
        GLWidget(QWidget* parent = 0, const QGLFormat& format = QGL::Rgba | QGL::DepthBuffer | QGL::DoubleBuffer);

        // redeclared virtual functions
        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);

        // destructor
        ~GLWidget();
    public slots:
        // public event handling methods/slots
        void set_angle_x(int value);
        void set_angle_y(int value);
        void set_angle_z(int value);

        void set_zoom_factor(double value);

        void set_trans_x(double value);
        void set_trans_y(double value);
        void set_trans_z(double value);

        // models
        void render_elephant();
        void render_mouse();
        void render_model_sphere();

        // curves
        void render_spiral_on_cone();
        void render_torus();
        void render_ellipse();
        void render_hypo();
        void render_lissajou();

        // surfaces
        void render_sphere();
        void render_chelicoid();
        void render_hyperboloid();
        void render_kleinbottle();
        void render_dupincycle();

        // cyclic curve
        void render_cyclic_curve();

        // shaders
        void initDirectionalLight();
        void initReflectionLines();
        void initToon();
        void initTwoSidedLighting();
        void installShader();
        void disableShader();

        void render_setup(GLuint render_function, GLuint render_index);

    private slots:
        void _animate();
    };
}
