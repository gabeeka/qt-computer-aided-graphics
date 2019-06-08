#include "SideWidget.h"

namespace cagd
{
    SideWidget::SideWidget(QWidget *parent): QWidget(parent)
    {
        setupUi(this);

        QPalette p = rotate_x_slider->palette();

        p.setColor(QPalette::Highlight, QColor(255,50,10).lighter());

        rotate_x_slider->setPalette(p);

        p = rotate_y_slider->palette();

        p.setColor(QPalette::Highlight, QColor(50,255,10).lighter());

        rotate_y_slider->setPalette(p);

        // arc operations directions

        arc_join_dir_1->addItem("LEFT", 0);
        arc_join_dir_1->addItem("RIGHT", 1);

        arc_join_dir_2->addItem("LEFT", 0);
        arc_join_dir_2->addItem("RIGHT", 1);

        // patch operations directions

        patch_dir_1->addItem("NORTH",   0);
        patch_dir_1->addItem("EAST",    1);
        patch_dir_1->addItem("SOUTH",   2);
        patch_dir_1->addItem("WEST",    3);

        patch_dir_2->addItem("NORTH",   0);
        patch_dir_2->addItem("EAST",    1);
        patch_dir_2->addItem("SOUTH",   2);
        patch_dir_2->addItem("WEST",    3);

        patch_dir_2->setCurrentIndex(2);
    }
}
