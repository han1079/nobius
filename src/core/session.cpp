#include <core/session.h>
Session::~Session() = default;
void Session::on_enter() {}
void Session::on_exit() {}
void Session::process_input_deltas(DispatchHistory& h) {}
void Session::process_time_deltas(float dT) {} 
void Session::submit_render_request() {}