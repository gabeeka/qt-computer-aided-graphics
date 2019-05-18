#pragma once

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLFormat>

#include "../Core/DCoordinates3.h"
#include "../Core/GenericCurves3.h"
#include "../Parametric/ParametricCurves3.h"
#include "../Core/TriangulatedMeshes3.h"

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
        QTimer*                     _timer;
        GLdouble                    _angle;
        GLuint                      _triangle_count;
        std::vector<DCoordinate3>   _cloud;
        GLuint                      _dl;

        // Prametric curves
        ParametricCurve3*           _pc;
        GenericCurve3*              _image_of_pc;

        // Mouse
        TriangulatedMesh3           _mouse;


        void initBoxOfTriangles();
        void paintBoxOfTriangles();

        void initSpiralOnCone(GLuint order);
        void paintSpiralOnCone(GLuint order);

        void initTorusKnot(GLuint order);
        void paintTorusKnot(GLuint order);

        void initLissajous(GLuint order);
        void paintLissajous(GLuint order);

        void initRose(GLuint order);
        void paintRose(GLuint order);


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
