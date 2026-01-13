#include <core/orchestrator.h>
#include <core/common.h>
#include <pch.h>
int main() {
    Orchestrator app;
    app.init();
    app.run();
    return 0;
}