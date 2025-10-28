
#ifndef UI_H
#define UI_H

#include <QWidget>
#include <QVBoxLayout>

class UserInterface : public QWidget {
public:
    UserInterface(QWidget* parent = nullptr);
    ~UserInterface() = default;

    QVBoxLayout* top_layout;
    QWidget* drawing_window;
    QWidget* debug_window;
};

#endif //UI_H
