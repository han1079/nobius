#include "control_panel.h"

DebugDisplay::DebugDisplay(QWidget* parent) : 
    QWidget(parent),
    font_obj("Arial", 10),
    font_info_obj(font_obj),
    padding_px(5),
    tracker_positions({{0,0,0,0}}) {};

int DebugDisplay::register_debug_line(std::string&& prefix) {
    debug_lines.push_back(prefix + ": (NaN)");
    debug_prefixes.push_back(prefix + ": ");
    return (debug_lines.size() - 1);
}

void DebugDisplay::update_boundaries() {
    tracker_positions.clear();
    for (int i = 0; i < debug_lines.size(); ++i) {
        int x = padding_px;
        int y = padding_px + (i + 1) * font_info_obj.height();
        int max_chars = (width() - 2 * padding_px) / font_info_obj.averageCharWidth();
        tracker_positions.push_back({x, y, max_chars});
    }
}

void DebugDisplay::update_tracker_int(int value, int pos_idx) {
    std::string formatted_line = debug_prefixes.at(pos_idx) + std::to_string(value);
    update_debug_display(formatted_line, pos_idx);
}

void DebugDisplay::update_tracker_float(float value, int pos_idx) {
    std::string formatted_line = debug_prefixes.at(pos_idx) + std::to_string(value);
    update_debug_display(formatted_line, pos_idx);
}

void DebugDisplay::update_tracker_string(const std::string& value, int pos_idx) {
    std::string formatted_line = debug_prefixes.at(pos_idx) + std::string(value);
    update_debug_display(formatted_line, pos_idx);
}

void DebugDisplay::update_debug_display(const std::string& formatted_line, int pos_idx) {
    if (pos_idx < 0 || pos_idx >= debug_lines.size()) {
        return;
    }
    debug_lines[pos_idx] = formatted_line;
    update();
}

void DebugDisplay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setFont(font_obj);
    painter.fillRect(event->rect(), Qt::white);
    painter.setPen(Qt::black);

    update_boundaries();

    for (int i = 0; i < debug_lines.size(); ++i) {
        if (i >= tracker_positions.size()) {
            break; // Prevent out-of-bounds access
        }
        int x = tracker_positions[i][0];
        int y = tracker_positions[i][1];
        painter.drawText(x, y, QString::fromStdString(debug_lines[i]));
    }
}


DrawingWidget::DrawingWidget(QWidget* parent) : 
    QWidget(parent) {}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event) {
    int x_pos = event->position().x();
    int y_pos = event->position().y();
    
    emit publish_mouse_x_pos(x_pos, mouse_x_idx);
    emit publish_mouse_y_pos(y_pos, mouse_y_idx);
}



ControlPanel::ControlPanel(QWidget* parent) : 
      QWidget(parent) {
    
    // Main layout - Drawing Window on top, and the user interface area below
    resize(800,600);
    setStyleSheet("background-color: gray;");  
    setWindowTitle("Dynamic Testing of Primitives");

    main_layout = new QVBoxLayout(this);

    drawing_window = new QWidget(this);
    user_interface_area = new QWidget(this);

    main_layout->addWidget(drawing_window);
    main_layout->addWidget(user_interface_area);
    main_layout->setStretch(0, 4);
    main_layout->setStretch(1, 1);

    //TODO: Add basic stretching to make drawing window take up most of the room

    // User Interface Area Layout
    user_interface_layout = new QHBoxLayout(user_interface_area);
    user_interface_layout->setStretch(0, 3);
    user_interface_layout->setStretch(1, 1);

    debug_display = new DebugDisplay(this);
    user_controls_area = new QWidget(this);

    user_interface_layout->addWidget(user_controls_area);
    user_interface_layout->addWidget(debug_display);

    debug_display->setMinimumWidth(200);
    debug_display->setMinimumHeight(100);

    //TODO: Same stretching idea here. Make the debug display a corner box.

    user_control_layout = new QVBoxLayout(user_controls_area);
    user_control_layout->setStretch(0,1);
    user_control_layout->setStretch(1,1);

    timeline_slider = new QSlider(Qt::Horizontal, this);
    mode_selector = new QToolBar(this);

    user_control_layout->addWidget(timeline_slider);
    user_control_layout->addWidget(mode_selector);


    //Make two actions that act as toggle buttons for select/draw modes
    toggle_select_mode = new QAction(this);
    toggle_draw_mode = new QAction(this);

    mode_selector->addAction(toggle_select_mode);
    mode_selector->addAction(toggle_draw_mode);

    mouse_x_idx = debug_display->register_debug_line(std::string("Mouse X"));
    mouse_y_idx = debug_display->register_debug_line(std::string("Mouse Y"));

    connect(this, &ControlPanel::publish_mouse_x_pos, debug_display, &DebugDisplay::update_tracker_int);
    connect(this, &ControlPanel::publish_mouse_y_pos, debug_display, &DebugDisplay::update_tracker_int);

    drawing_window->setMouseTracking(true);
}

void ControlPanel::mouseMoveEvent(QMouseEvent* event) {
    int x_pos = event->position().x();
    int y_pos = event->position().y();
    emit publish_mouse_x_pos(x_pos, mouse_x_idx);
    emit publish_mouse_y_pos(y_pos, mouse_y_idx);
}
