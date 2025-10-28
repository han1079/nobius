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

static int X_WINDOW_SIZE = 800;
static int Y_WINDOW_SIZE = 600;

static bool DEBUG_MODE = true;

#define DEBUG_PRINT(x) if (DEBUG_MODE) std::cout << x << std::endl; 

// Custom Syntax:
// lstf = list of
// rfto = references to
// ptto = pointer to 
// *_obj = variable is a big object
// Assume that anything that is a verb is a function
typedef std::variant<int*, float*, double*, bool*, std::string*, std::monostate> dbg_wc_ptr_t;
typedef std::vector<dbg_wc_ptr_t> lstf_dbg_wc_ptr_t;
class debug_register {
    private:
        
    std::vector<std::string> lstf_preface;
    lstf_dbg_wc_ptr_t lstf_ptto_debug_value;
    std::vector<std::function<std::string()>> lstf_formatting_function;

    int latest_message_idx = -1;

    int font_size = 10;
   
    int box_pixel_length = -1;
    int box_pixel_height = -1;

    QFont font_obj;
    QFontMetrics font_info_obj;

    int px_padding = 5;

    void confirmed_debug_data_is_consistent(){
        // Guard statement to make sure we can just use len() of any of these
        // lists to append the next one.
        size_t num_of_msgs = lstf_preface.size();
        if (lstf_ptto_debug_value.size() != num_of_msgs || lstf_formatting_function.size() != num_of_msgs) {
            throw std::runtime_error("Inconsistent Data Fields.");
        }
    }

    public:

    debug_register() : 
        font_obj("Arial", 10),
        font_info_obj(font_obj) {}
             
    bool set_font_size(int proposed_size) {
        if (proposed_size > 16 || proposed_size < 6) {
            std::cout << "Font size " << proposed_size << " inappropriate" << std::endl;
            return false;
        } else {
            font_size = proposed_size; 
            return true;
        }
    }

    int get_font_size() { return font_size; }

    float get_max_debug_lines() {
        return box_pixel_height / font_info_obj.height();
    }

    void updateBoxSize(const QRect& debug_box) {
        int _bpl = debug_box.width();
        int _bph = debug_box.height();
        
        // Keep the box a minimum size.
        if (_bpl < 50 || _bph < 50) {
            box_pixel_length = 50;
            box_pixel_height = 50;
        } else {
            box_pixel_length = debug_box.width();
            box_pixel_height = debug_box.height();
        }
    }

    auto get_debug_value_at_idx(int idx){
        dbg_wc_ptr_t debug_pointer_at_idx; 

        if (idx == -1){ // Pythonic -1 as "last element" access
            debug_pointer_at_idx = lstf_ptto_debug_value.back();
        } else {
            debug_pointer_at_idx = lstf_ptto_debug_value.at(idx);
        }

        auto run_me_to_dereference = [](auto wildcard) -> std::variant<int, float, double, std::string, std::monostate> {
            if constexpr(std::is_same_v<std::decay_t<decltype(wildcard)>, std::monostate>) {
                return std::monostate();
            } else {
                return *wildcard;
            }
        };

        auto to_rtn = std::visit(run_me_to_dereference, debug_pointer_at_idx);      
        return to_rtn; 
    }


    bool register_new_message(std::string preface,  dbg_wc_ptr_t ptto_debug_value) {
        // Guard line to make sure that everything matches before adding to the list of debugs.

        confirmed_debug_data_is_consistent(); 

        // Don't mess with anything if we don't have room.
        if (latest_message_idx > (int) (box_pixel_height / font_info_obj.height())) {
            std::cout << "Too many messages" << std::endl;
            return false;
        }
        
        lstf_preface.push_back(preface);
        lstf_ptto_debug_value.push_back(ptto_debug_value);

        auto formatter_function = [preface, ptto_debug_value]() -> std::string
        {
            auto run_me_to_dereference_and_format = [](auto&& wildcard) {
                // This function is called by std::visit - which means the wildcard
                // is one of the types IN the variant - not the variant itself.
                using wildcard_type = std::decay_t<decltype(wildcard)>;

                constexpr bool COMPILER_SEES_INT = std::is_same_v<wildcard_type, int*>;
                constexpr bool COMPILER_SEES_FLOAT = std::is_same_v<wildcard_type, float*>;
                constexpr bool COMPILER_SEES_DOUBLE = std::is_same_v<wildcard_type, double*>;
                constexpr bool COMPILER_SEES_STRING = std::is_same_v<wildcard_type, std::string*>;
                constexpr bool COMPILER_SEES_BOOL = std::is_same_v<wildcard_type, bool*>;
                constexpr bool COMPILER_SEES_NULL = std::is_same_v<wildcard_type, std::monostate>;

                if constexpr (COMPILER_SEES_INT || COMPILER_SEES_FLOAT || COMPILER_SEES_DOUBLE) {
                    return std::to_string(*wildcard);
                
                } else if constexpr (COMPILER_SEES_STRING) {
                    return std::string(*wildcard);
                } else if constexpr (COMPILER_SEES_BOOL) {
                    if (*wildcard) {
                        return std::string("True");
                    } else {
                        return std::string("False");
                    }
                } else if constexpr (COMPILER_SEES_NULL) {
                    return std::string("");
                } else {
                    throw(std::invalid_argument("Type not one of the wildcard somehow."));
                }
            };
            auto debug_value_str = std::visit(run_me_to_dereference_and_format, ptto_debug_value);
            return preface + debug_value_str;
        };
        
        lstf_formatting_function.push_back(formatter_function);
        return true;
    }


    void draw_debug_message(QPainter& painter, const QRect& debug_box) {
        // First - get all of the debug message objects and create two lists
        // The first list is a list of QStrings that format the preface and the data into one thing
        // The second list is a list of positions to put the debug statements
        
        std::vector<QString> formatted_debugs;
        std::vector<float> base_height_positions;
        int number_of_lines = (int) get_max_debug_lines();
       
        // Create padding with some pixels off from top left.
        int left_edge = debug_box.left() + px_padding;
        int starting_y = debug_box.top() + px_padding;
        int inter_line_padding = (int)(font_info_obj.height() / 10) + 1;
        int font_height = font_info_obj.height();

        // Set the painter stuff
        painter.setFont(font_obj);
        painter.setPen(QPen(Qt::black));

        // Holders for the variables that change with each line        
        int y_base_pixel = starting_y;
        QString _qstr;
        for (auto i : lstf_formatting_function) {
            _qstr = font_info_obj.elidedText(QString::fromStdString(i()), Qt::ElideRight, box_pixel_length - 2*px_padding);
            y_base_pixel = y_base_pixel + font_height + inter_line_padding;
            painter.drawText(left_edge, y_base_pixel, _qstr);
        }
    }
};


class TestScreen : public QWidget {
public:
    TestScreen(debug_register& dbg, QWidget* parent = nullptr) : 
        QWidget(parent), 
        access_to_debug_register(dbg),
        mouse_x(0), 
        mouse_y(0),
        in_window_or_not("Not in window")
    {
        resize(X_WINDOW_SIZE, Y_WINDOW_SIZE);
        setStyleSheet("background-color: gray;");  
        setWindowTitle("Dynamic Testing of Primitives");
        setMouseTracking(true);

    }

    void initialize_debug_register() {
        access_to_debug_register.updateBoxSize(debug_box);
        access_to_debug_register.register_new_message("Mouse X: ", &mouse_x); 
        access_to_debug_register.register_new_message("Mouse Y: ", &mouse_y); 
        access_to_debug_register.register_new_message("", &in_window_or_not);
    }
    
private:
    glm::vec2 debug_box_size = glm::vec2(100.0f, 100.0f);
    QRect debug_box = QRect(X_WINDOW_SIZE - debug_box_size.x, Y_WINDOW_SIZE - debug_box_size.y, X_WINDOW_SIZE-1, Y_WINDOW_SIZE-1);

    int mouse_x;
    int mouse_y;
    std::string in_window_or_not;

    debug_register access_to_debug_register;
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        if (DEBUG_MODE) {
            drawDebugBox(painter);
            access_to_debug_register.draw_debug_message(painter, debug_box);
        }
    }

    void drawDebugBox(QPainter& painter) {
        QPen pen(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);
        
        debug_box = QRect(width() - debug_box_size.x, height() - debug_box_size.y, 
                         debug_box_size.x, debug_box_size.y);

        painter.drawRect(debug_box);
        painter.fillRect(debug_box, QBrush(QColor(255, 255, 255, 30)));
         
    }



    void mouseMoveEvent(QMouseEvent* event) override {
        QPointF mouse_xy = event->position();
        mouse_x = mouse_xy.x();
        mouse_y = mouse_xy.y();
        update();
    }

    void enterEvent(QEnterEvent* event) override {
        Q_UNUSED(event);
        in_window_or_not = "Mouse entered widget area.";
        DEBUG_PRINT(in_window_or_not);
        update();
    }

    void leaveEvent(QEvent* event) override {
        Q_UNUSED(event);
        in_window_or_not = "Mouse left widget area.";
        DEBUG_PRINT(in_window_or_not);
        update();
    }

};   


int main(int argc, char *argv[]) {
    debug_register global_debug_register;
    QApplication app(argc, argv);
    
    TestScreen screen(global_debug_register);
    screen.initialize_debug_register();
    screen.show();
    
    return app.exec();
}
