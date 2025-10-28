#ifndef DEBUG_WINDOW_H
#define DEBUG_WINDOW_H

#include <string>
#include <variant>
#include <vector>

#include <QFont>
#include <QRect>
#include <QPainter>

#include <glm/glm.hpp>

typedef std::variant<int*, float*, double*, bool*, std::string*, std::monostate> dbg_wc_ptr_t;
typedef std::vector<dbg_wc_ptr_t> lstf_dbg_wc_ptr_t;

class DebugInterface : public QWidget {
public:
    DebugInterface(debug_register& dbg, QWidget* parent = nullptr);

    void initialize_debug_register();

private:
    glm::vec2 debug_box_size;
    QRect debug_box;

    int mouse_x;
    int mouse_y;

    debug_register rfto_debug_register;

protected:
    void paintEvent(QPaintEvent*);
    void drawDebugBox(QPainter& painter);

}

class debug_register {
private:

    std::vector<std::string> lstf_preface;
    lstf_dbg_wc_ptr_t lstf_ptto_debug_value;
    std::vector<std::function<std::string()>> lstf_formatting_function;

    void confirmed_debug_data_is_consistent();

public:

    debug_register();
    bool register_new_message(std::string preface, dbg_wc_ptr_t ptto_debug_value);
};

#endif // DEBUG_WINDOW_H
