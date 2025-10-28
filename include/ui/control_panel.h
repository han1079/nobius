#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSlider>
#include <QToolBar>
#include <QPaintEvent>
#include <QAction>
#include <QPainter>
#include <QFont>
#include <string>
#include <vector>


class DebugDisplay : public QWidget {
    Q_OBJECT

public:
    DebugDisplay(QWidget* parent = nullptr);
    int register_debug_line(std::string&& prefix);

public slots:
    void update_tracker_int(int value, int pos_idx);
    void update_tracker_float(float value, int pos_idx);
    void update_tracker_string(const std::string& value, int pos_idx);


private:
    std::vector<std::string> debug_lines;
    std::vector<std::string> debug_prefixes;
    QFont font_obj;
    QFontMetrics font_info_obj;
    int padding_px;

    std::vector<std::vector<int>> tracker_positions;

    /* Helper to generate positions of text strings.
     Calculates based on font metrics and padding.
     Returns <x_lower_left, y_lower_left, max_characters> */
    void update_boundaries();
    void update_debug_display(const std::string& formatted_line, int pos_idx);

protected:
    void paintEvent(QPaintEvent* event) override;
};

class DrawingWidget : public QWidget {
    Q_OBJECT

public:
    DrawingWidget(QWidget* parent = nullptr) = default;

signals:
    void publish_mouse_x_pos(int x_pos, int pos_idx);
    void publish_mouse_y_pos(int y_pos, int pos_idx);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
};

class ControlPanel : public QWidget {
    Q_OBJECT
public:
    ControlPanel(QWidget* parent = nullptr);
    ~ControlPanel() = default;
    
signals:
    void publish_mouse_x_pos(int x_pos, int pos_idx);
    void publish_mouse_y_pos(int y_pos, int pos_idx);

private:

    // Top of the Window - Drawing Area
    QVBoxLayout* main_layout;
    QWidget* drawing_window;
    QWidget* user_interface_area;

    // Mid-Bottom of the Window - Timeline Controls 
    QHBoxLayout* user_interface_layout;
    DebugDisplay* debug_display;
    QWidget* user_controls_area;

    // Bottom: Debug / Input / Ribbon
    QVBoxLayout* user_control_layout;
    QSlider* timeline_slider;
    QToolBar* mode_selector;

    // Buttons on the mode selector toolbar (QAction Objects)
    QAction* toggle_select_mode;
    QAction* toggle_draw_mode;

    int mouse_x_idx;
    int mouse_y_idx;


protected:
    //void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    //void enterEvent(QEnterEvent* event) override;
    //void leaveEvent(QEvent* event) override;
};

#endif // CONTROL_PANEL_H