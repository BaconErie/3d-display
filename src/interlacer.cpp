#include "interlacer.hpp"

namespace interlacer {
    bool renderer_ready = false;
    std::vector<double> pixel_exit_angles;
    double N = 1.5;             // Index of refraction 
    int PIXELS_PER_INCH;    // Number of pixels per inch on the display
    double LENS_PER_INCH;   // Amount of lenses per inch on the display
    double DISPLAY_WIDTH;
    bool was_calibrated = false;
}

void interlacer::setup() {
    interlacer::PIXELS_PER_INCH = parameters::window_width / parameters::green_to_red_line_distance;
    interlacer::LENS_PER_INCH = parameters::lenticule_density;
    interlacer::DISPLAY_WIDTH = parameters::window_width;

    calculate_pixel_exit_angles();
    std::cout << "done with setup !!" << std::endl;

    std::cout << "PIXELS_PER_INCH " << PIXELS_PER_INCH << std::endl;
    std::cout << "LENS_PER_INCH " << LENS_PER_INCH << std::endl;
    std::cout << "DISPLAY_WIDTH " << DISPLAY_WIDTH << std::endl;

    for (int i=0; i < pixel_exit_angles.size(); i++) {
        std::cout << "pixel_exit_angles[" << i << "] = " << pixel_exit_angles[i] << std::endl;
    }

    was_calibrated = true;
}

double interlacer::f(double x) {
    return std::pow(1-std::pow(x, 2), 0.5);
}

double interlacer::f_prime(double x) {
    return (interlacer::f(x+1.0/1000000) - interlacer::f(x))*1000000;
}

double interlacer::t(double x) {
    double f_prime = interlacer::f_prime(x);

    std::cout << "x: " << x << std::endl;
    std::cout << "f_prime: " << f_prime << std::endl;
    std::cout << "N: " << interlacer::N << std::endl;
    std::cout << "std::atan2(f_prime, 1): " << std::atan2(f_prime, 1) << std::endl;
    std::cout << " std::sin(std::atan2(f_prime, 1)): " << std::sin(
            std::atan2(f_prime, 1)
        ) << std::endl;

    std::cout << "interlacer::N * std::sin(std::atan2(f_prime, 1)): " << interlacer::N * std::sin(
            std::atan2(f_prime, 1)
        ) << std::endl;

    std::cout << "std::atan2(f_prime, 1): " << std::atan2(f_prime, 1) << std::endl;
    std::cout << "std::asin( interlacer::N * std::sin( std::atan2(f_prime, 1) ) ): " << std::asin(
        interlacer::N * std::sin(
            std::atan2(f_prime, 1)
        )
    ) << std::endl;
    std::cout << "\n\n\n\n" << std::endl;

    return (std::asin(
        interlacer::N * std::sin(
            std::atan2(f_prime, 1)
        )
    ) - std::atan2(f_prime, 1)) * 57.2957795;
}

void interlacer::calculate_pixel_exit_angles() {
    double PIXELS_PER_LENS = interlacer::PIXELS_PER_INCH / interlacer::LENS_PER_INCH;
    std::cout << "pixels_Per_lens " <<  PIXELS_PER_INCH << std::endl;
    for (int pixel=0; pixel < DISPLAY_WIDTH; pixel++) {
        double x = 2 * std::fmod(pixel, PIXELS_PER_LENS) / PIXELS_PER_LENS - 1; // How far this is from the center of the lens
        std::cout << "for pixel " << pixel << " the x valeu is gonna be " << x << std::endl;
        pixel_exit_angles.push_back(interlacer::t(x));
    }
}

void interlacer::listen_for_renderer_socket_and_call_dispatcher() {
    std::cout << "Beginning listen." << std::endl;
    shared_vars::acceptor.accept(shared_vars::socket);
    std::cout << "We accepted something!!!!." << std::endl;
    shared_vars::renderer_ready_dispatcher.emit();
}