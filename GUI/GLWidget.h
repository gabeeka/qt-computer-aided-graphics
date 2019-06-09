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
#include "../SOQAH/SOQAHPatch3.h"

#include "../SOQAH/SOQAHCompositeCurve3.h"
#include "../SOQAH/SOQAHCompositeSurface3.h"

#include <QTimer>

namespace cagd
{
    class GLWidget: public QGLWidget
    {
        Q_OBJECT

    private:
        QWidget*     _main_widget;

        GLuint      _render_function{1};
        GLuint      _render_index{0};

        // arc join operation flags
        GLuint _joinIndex1{0};
        GLuint _joinIndex2{1};

        SOQAHCompositeCurve3::Direction _joinDirection1{SOQAHCompositeCurve3::Direction::LEFT};
        SOQAHCompositeCurve3::Direction _joinDirection2{SOQAHCompositeCurve3::Direction::LEFT};

        // arc render options
        GLboolean _render_first_order{GL_FALSE};
        GLboolean _render_second_order{GL_FALSE};
        GLboolean _render_control_points{GL_FALSE};

        // arc control point manipulation
        int      _arc_index{0};
        int      _cp_index{0};

        // patch operation falgs
        GLuint _patchIndex1{0};
        GLuint _patchIndex2{1};

        // patch render options
        GLboolean _render_control_net{GL_FALSE};

        SOQAHCompositeSurface3::Direction _patchDirection1{SOQAHCompositeSurface3::Direction::NORTH};
        SOQAHCompositeSurface3::Direction _patchDirection2{SOQAHCompositeSurface3::Direction::SOUTH};

        // patch control point manipulation
        int     _patch_index{0};
        int     _p_cp_index_1{0};
        int     _p_cp_index_2{0};

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

        // SOQAH arc composite
        SOQAHCompositeCurve3*   _soqah_arc_composite;


        // SOQAH patch declarations
        SOQAHPatch3*                    _patch;
        TriangulatedMesh3*              _before_interpolation;
        TriangulatedMesh3*              _after_interpolation;
        RowMatrix<GenericCurve3*>*      _u_lines;
        RowMatrix<GenericCurve3*>*      _v_lines;

        // SOQAH patch composite
        SOQAHCompositeSurface3*         _soqah_patch_composite;

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
        void initSOQAHArc();
        void renderSOQAHArc();

        // SOQAH Arc Composite
        void initSOQAHArcComposite();
        void renderSOQAHArcComposite();

        void updateSOQAHArcComposite();
        void addNewSOQAHArc();

        // SOQAH Patch
        void initSOQAHPatch();
        void renderSOQAHPatch();

        // SOQAH Patch Composite
        void initSOQAHPatchComposite();
        void renderSOQAHPatchComposite();

        void addNewSOQAHPatch();


        // Shaders
        GLuint          _shader_index{0};
        ShaderProgram   _shader;

        GLdouble _scale_factor;
        GLdouble _smoothing;
        GLdouble _shading;

        GLdouble _red_default_outline_color;
        GLdouble _green_default_outline_color;
        GLdouble _blue_default_outline_color;
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

        void set_scale_factor(double value);
        void set_smoothing(double value);
        void set_shading(double value);
        void set_red_default_outline_color(double value);
        void set_green_default_outline_color(double value);
        void set_blue_default_outline_color(double value);

        // project
        void renderArc();
        void renderPatch();

        // control point manipulation
        void arcIndexChanged(int  value);
        void cpIndexChanged(int value);

        void updateCpXCoord(double value);
        void updateCpYCoord(double value);
        void updateCpZCoord(double value);

        void updateRenderFirstOrder(int value);
        void updateRenderSecondOrder(int value);
        void updateRenderControlPoints(int value);

        void addNewArc();
        void resetArcs();

        // arc join operation
        void updateArcJoinIndex1(int value);
        void updateArcJoinIndex2(int value);

        void updateArcJoinDir1(int value);
        void updateArcJoinDir2(int value);

        void joinArcs();
        void mergeArcs();
        void continueArc();

        // patch control point manipulation
        void updatePatchIndex(int value);

        void updatePatchCpIndex1(int value);
        void updatePatchCpIndex2(int value);

        void updatePatchCpXCoord(double value);
        void updatePatchCpYCoord(double value);
        void updatePatchCpZCoord(double value);

        void updateRenderControlNet(int value);
        void updateMaterial(int index);

        // patch operations
        void updatePatchIndex1(int value);
        void updatePatchIndex2(int value);
        void updatePatchDir1(int value);
        void updatePatchDir2(int value);

        void joinPatches();
        void mergePatches();
        void continuePatch();

        void addNewPatch();
        void resetPatches();

        void render_setup(GLuint render_function, GLuint render_index);

    private slots:
        void _animate();
    };
}
