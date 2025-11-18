#pragma once

#include "shared.hpp"
#include "interlacer.hpp"
#include <vector>
#include <iostream>
#include <cmath>

// Handles interlacer and 3D renderer

namespace interlacer {
    extern bool renderer_ready;
    extern std::vector<double> pixel_exit_angles;
    extern double N;             // Index of refraction 
    extern int PIXELS_PER_INCH; // Number of pixels per inch on the display
    extern double LENS_PER_INCH; // Amount of lenses per inch on the display
    extern double DISPLAY_SIZE;

    void setup();

    double f(double x);       // Function f(x) that defines the lens surface
    double f_prime(double x); // Derivative of f(x)
    double t(double x); // Returns the exit angle of the pixel at x distance from the center of the lens

    void calculate_pixel_exit_angles();
    void calculate_eye_angles(double& left_eye, double& right_eye);
    void get_renderer_message_string();

    void listen_for_renderer_socket_and_call_dispatcher(); // Run this in a new thread, because socket accept blocks.
}