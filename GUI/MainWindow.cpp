#include "MainWindow.h"

namespace cagd
{
    MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
    {
        setupUi(this);

    /*

      the structure of the main window's central widget

     *---------------------------------------------------*
     |                 central widget                    |
     |                                                   |
     |  *---------------------------*-----------------*  |
     |  |     rendering context     |   scroll area   |  |
     |  |       OpenGL widget       | *-------------* |  |
     |  |                           | | side widget | |  |
     |  |                           | |             | |  |
     |  |                           | |             | |  |
     |  |                           | *-------------* |  |
     |  *---------------------------*-----------------*  |
     |                                                   |
     *---------------------------------------------------*

    */
        _side_widget = new SideWidget(this);

        _scroll_area = new QScrollArea(this);
        _scroll_area->setWidget(_side_widget);
        _scroll_area->setSizePolicy(_side_widget->sizePolicy());
        _scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        _gl_widget = new GLWidget(this);

        centralWidget()->setLayout(new QHBoxLayout());
        centralWidget()->layout()->addWidget(_gl_widget);
        centralWidget()->layout()->addWidget(_scroll_area);

        // creating a signal slot mechanism between the rendering context and the side widget
        connect(_side_widget->rotate_x_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_x(int)));
        connect(_side_widget->rotate_y_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_y(int)));
        connect(_side_widget->rotate_z_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_z(int)));

        connect(_side_widget->zoom_factor_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_zoom_factor(double)));

        connect(_side_widget->trans_x_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_x(double)));
        connect(_side_widget->trans_y_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_y(double)));
        connect(_side_widget->trans_z_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_z(double)));

        // models
        connect(_side_widget->elephant_radio_button, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_elephant()));
        connect(_side_widget->mouse_radio_button, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_mouse()));
        connect(_side_widget->sphere_radio_button, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_model_sphere()));

        //curves
        connect(_side_widget->spiral_on_cone, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_spiral_on_cone()));
        connect(_side_widget->torus, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_torus()));
        connect(_side_widget->ellipse, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_ellipse()));
        connect(_side_widget->hypo, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_hypo()));
        connect(_side_widget->lissajou, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_lissajou()));

        //surfaces
        connect(_side_widget->sphere, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_sphere()));
        connect(_side_widget->chelicoid, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_chelicoid()));
        connect(_side_widget->hyperboloid, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_hyperboloid()));
        connect(_side_widget->kleinbottle, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_kleinbottle()));
        connect(_side_widget->dupincyclide, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_dupincycle()));

        // cyclice curves
        connect(_side_widget->cyclic_curve, SIGNAL(clicked(bool)), _gl_widget, SLOT(render_cyclic_curve()));

        // shaders
        connect(_side_widget->dir_light, SIGNAL(clicked(bool)), _gl_widget, SLOT(initDirectionalLight()));
        connect(_side_widget->ref_lines, SIGNAL(clicked(bool)), _gl_widget, SLOT(initReflectionLines()));
        connect(_side_widget->toon, SIGNAL(clicked(bool)), _gl_widget, SLOT(initToon()));
        connect(_side_widget->two_s_light, SIGNAL(clicked(bool)), _gl_widget, SLOT(initTwoSidedLighting()));
        connect(_side_widget->none, SIGNAL(clicked(bool)), _gl_widget, SLOT(disableShader()));

        // project
        connect(_side_widget->arc_radio_button, SIGNAL(clicked(bool)), _gl_widget, SLOT(renderArc()));
        connect(_side_widget->patch_radio_button, SIGNAL(clicked(bool)), _gl_widget, SLOT(renderPatch()));

        // control point manipulation
        connect(_side_widget->arc_index, SIGNAL(valueChanged(int)), _gl_widget, SLOT(arcIndexChanged(int)));
        connect(_side_widget->cp_index, SIGNAL(valueChanged(int)), _gl_widget, SLOT(cpIndexChanged(int)));

        connect(_side_widget->cp_x_coord, SIGNAL(valueChanged(double)), _gl_widget, SLOT(updateCpXCoord(double)));
        connect(_side_widget->cp_y_coord, SIGNAL(valueChanged(double)), _gl_widget, SLOT(updateCpYCoord(double)));
        connect(_side_widget->cp_z_coord, SIGNAL(valueChanged(double)), _gl_widget, SLOT(updateCpZCoord(double)));

        connect(_side_widget->first_order, SIGNAL(stateChanged(int)), _gl_widget, SLOT(updateRenderFirstOrder(int)));
        connect(_side_widget->second_order, SIGNAL(stateChanged(int)), _gl_widget, SLOT(updateRenderSecondOrder(int)));
        connect(_side_widget->control_points, SIGNAL(stateChanged(int)), _gl_widget, SLOT(updateRenderControlPoints(int)));

        connect(_side_widget->appendArc, SIGNAL(clicked(bool)), _gl_widget, SLOT(addNewArc()));
        connect(_side_widget->reset_arcs, SIGNAL(clicked(bool)), _gl_widget, SLOT(resetArcs()));

        // arc operations
        connect(_side_widget->arc_join_ind_1, SIGNAL(valueChanged(int)), _gl_widget, SLOT(updateArcJoinIndex1(int)));
        connect(_side_widget->arc_join_ind_2, SIGNAL(valueChanged(int)), _gl_widget, SLOT(updateArcJoinIndex2(int)));

        connect(_side_widget->arc_join_dir_1, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(updateArcJoinDir1(int)));
        connect(_side_widget->arc_join_dir_2, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(updateArcJoinDir2(int)));

        connect(_side_widget->join_arcs, SIGNAL(clicked(bool)), _gl_widget, SLOT(joinArcs()));
        connect(_side_widget->merge_arcs, SIGNAL(clicked(bool)), _gl_widget, SLOT(mergeArcs()));
        connect(_side_widget->continue_arc, SIGNAL(clicked(bool)), _gl_widget, SLOT(continueArc()));

        // patch control point manipulation
        connect(_side_widget->patch_index, SIGNAL(valueChanged(int)), _gl_widget, SLOT(updatePatchIndex(int)));

        connect(_side_widget->p_cp_index_1, SIGNAL(valueChanged(int)), _gl_widget, SLOT(updatePatchCpIndex1(int)));
        connect(_side_widget->p_cp_index_2, SIGNAL(valueChanged(int)), _gl_widget, SLOT(updatePatchCpIndex2(int)));

        connect(_side_widget->p_cp_x_coord, SIGNAL(valueChanged(double)), _gl_widget, SLOT(updatePatchCpXCoord(double)));
        connect(_side_widget->p_cp_y_coord, SIGNAL(valueChanged(double)), _gl_widget, SLOT(updatePatchCpYCoord(double)));
        connect(_side_widget->p_cp_z_coord, SIGNAL(valueChanged(double)), _gl_widget, SLOT(updatePatchCpZCoord(double)));

        connect(_side_widget->add_new_patch, SIGNAL(clicked(bool)), _gl_widget, SLOT(addNewPatch()));
        connect(_side_widget->reset_patches, SIGNAL(clicked(bool)), _gl_widget, SLOT(resetPatches()));

        // patch operations
        connect(_side_widget->patch_index_1, SIGNAL(valueChanged(int)), _gl_widget, SLOT(updatePatchIndex1(int)));
        connect(_side_widget->patch_index_2, SIGNAL(valueChanged(int)), _gl_widget, SLOT(updatePatchIndex2(int)));

        connect(_side_widget->patch_dir_1, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(updatePatchDir1(int)));
        connect(_side_widget->patch_dir_2, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(updatePatchDir2(int)));

        connect(_side_widget->join_patches, SIGNAL(clicked(bool)), _gl_widget, SLOT(joinPatches()));
        connect(_side_widget->merge_patches, SIGNAL(clicked(bool)), _gl_widget, SLOT(mergePatches()));
        connect(_side_widget->continue_patch, SIGNAL(clicked(bool)), _gl_widget, SLOT(continuePatch()));

    }

    //--------------------------------
    // implementation of private slots
    //--------------------------------
    void MainWindow::on_action_Quit_triggered()
    {
        qApp->exit(0);
    }
}
