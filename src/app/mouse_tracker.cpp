#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QPaintEvent>
#include <QFont>
#include <QString>
#include <QMouseEvent>
#include <iostream>
#include <glm/glm.hpp>
#include <QLineEdit>
#include <qnamespace.h>
#include <stdexcept>
#include <variant>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "../ui/ui.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    UserInterface main_window;
    main_window.show();
    
    return app.exec();
}
