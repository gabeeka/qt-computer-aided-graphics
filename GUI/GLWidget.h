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

#include <QTimer>

namespace cagd
{
    class GLWidget: public QGLWidget
    {
        Q_OBJECT

    private:

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

        // Cyclic curves
        CyclicCurve3*               _cyclic_curve;
        GenericCurve3*              _image_of_cc;

        // Prametric curves
        std::vector<ParametricCurve3*>          _pc_vec;
        std::vector<GenericCurve3*>             _image_of_pc_vec;

        // Models - off files
        QTimer*                                 _timer;
        GLdouble                                _angle;
        TriangulatedMesh3                       _mouse;

        // Parametric surfaces
        std::vector<cagd::ParametricSurface3*>  _ps_vec;
        std::vector<cagd::TriangulatedMesh3*>   _image_of_ps_vec;

        //Lighting
        DirectionalLight*                       _directionalLight;


        // Private member functions

        // ParametricCurves
        void initParametricCurves(GLuint curve_count);
        void initSpiralOnCone(GLuint div_point_count, GLenum usage_flag);
        void initTorus(GLuint div_point_count, GLenum usage_flag);
        void initElliplse(GLuint div_point_count, GLenum usage_flag);
        void initHypo(GLuint div_point_count, GLenum usage_flag);
        void initLissajou(GLuint div_point_count, GLenum usage_flag);
        void renderParametricCurve(GLuint curve_index);


        // OFF models
        void initOffModel(const char* file_name);
        void renderOffModel();


        // Parametric surfaces
        void initParametricSurfaces(GLuint surface_count);
        void initSphere(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initChelicoid(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initHyperboloid(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initKleinbottle(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void initDupincyclide(GLuint u_div_point_count, GLuint v_div_point_count, GLenum usage_flag);
        void renderParametricSurface(GLuint surface_index);


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

    private slots:
        void _animate();
    };
}
