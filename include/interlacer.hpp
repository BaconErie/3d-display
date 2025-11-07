#pragma once

#include "shared.hpp"
#include <vector>
#include <iostream>

// Handles interlacer and 3D renderer

namespace interlacer {
    extern std::vector<float> pixel_exit_angles;

    void calculate_pixel_exit_angles();
    void calculate_eye_angles(float& left_eye, float& right_eye);
    void get_renderer_message_string();

    void listen_for_renderer_socket_and_call_dispatcher(); // Run this in a new thread, because socket accept blocks.
}