#include <core/event.h>

#ifndef LAYER_H
#define LAYER_H

class Layer {
public:
    Layer() = default;
    ~Layer() = default;

    std::vector<Layer*> LayerStack;

private:

    virtual bool on_attach();
    virtual bool on_detach();
    virtual bool on_event(Event& e);
    virtual bool on_update(float dt);
    virtual bool on_render();

}

#endif //LAYER_H
