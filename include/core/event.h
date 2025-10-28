#ifndef EVENT_H
#define EVENT_H

enum class EventType {
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    AppTick, AppUpdate, AppRender,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory {
    None = 0,
    EventCategoryApplication = (1<<0),
    EventCategoryInput = (1<<1),
    EventCategoryKeyboard = (1<<2),
    EventCategoryMouse = (1<<3),
    EventCategoryMouseButton = (1<<4)
};

#define EVENT_TYPE_GETTERS(event_type_name) static EventType get_class_event_type() { return EventType::event_type_name; }\
                                        virtual EventType get_instance_event_type() const override { return get_class_event_type(); }\
                                        virtual char* get_instance_name() const override { return #event_type_name; }

#define EVENT_CATEGORY_GETTER(event_category_name) virtual EventCategory get_category_bit_flag() const override { return event_category_name; }

#define MAKE_CALLBACK_FCN_USING(fcn) [this](auto&& arg) -> bool { return this->fcn(arg); }

class Event {
public:
    virtual ~Event() = default;
    
    bool event_is_handled = false;

    virtual EventType get_instance_event_type() const;
    virtual char* get_instance_name() const;
    virtual EventCategory get_category_bit_flag() const;

    bool is_in_category(EventCategory category_to_check){
        return get_category_bit_flag() & category_to_check;
    }
};

class EventRouter {
private:
    Event& m_event;
public:
    EventRouter(Event& e) : m_event(e) {}
    ~EventRouter() = default;

    template <typename fcn_t, typename event_class_name>
    bool match_event_to_fcn(const fcn_t& callback){
        if (m_event.get_instance_event_type() == event_class_name::get_class_event_type()){
            m_event.event_is_handled |= callback(static_cast<event_class_name&>(m_event));
            return true;
        }
        return false;
    }

};

#endif //EVENT_H
