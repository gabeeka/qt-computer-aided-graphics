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
    }

    //--------------------------------
    // implementation of private slots
    //--------------------------------
    void MainWindow::on_action_Quit_triggered()
    {
        qApp->exit(0);
    }
}
