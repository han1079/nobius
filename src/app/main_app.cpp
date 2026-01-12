#include <core/orchestrator.h>
#include <core/common.h>
#include <pch.h>
int main() {
    LoadSpec lspec = LoadSpec();
    Orchestrator app(lspec);
    app.init();
    app.run();
    return 0;
}