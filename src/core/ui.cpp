#include <core/ui.h>


UserInterface::UserInterface(QWidget* parent) :
    QWidget(parent) {

    resize(800,600);
    top_layout = new QVBoxLayout(this);
    drawing_window = new QWidget(this);
    debug_window = new QWidget(this);

    top_layout->addWidget(drawing_window);
    top_layout->addWidget(debug_window);
    top_layout->setStretch(0,5);
    top_layout->setStretch(1,1);
}
