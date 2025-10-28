#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <core/ui.h>
#include <core/state_tracker.h>
#include <core/layer.h>

class AppConfig {

};

class Orchestrator {
private:

    static Orchestrator* orch_inst_ptr;

public:

    //Constructor and Destructor for the high level orchestrator
    Orchestrator(AppConfig c) : config(c) {}
    ~Orchestrator() = default;

    //Getter Function for the Orchestrator
    static Orchestrator* get();

private:

    AppConfig config;

    /*The Orchestrator has access to the following items:
     *
     * UserInterface Object
     *
     * */

    bool running = true;
    bool screen_minimized = false;

public:

}

#endif //ORCHESTRATOR_H
