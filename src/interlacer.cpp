#include "interlacer.hpp"

namespace interlacer {
    bool renderer_ready = false;
    std::vector<double> pixel_exit_angles;
    double N = 1.5;             // Index of refraction 
    int PIXELS_PER_INCH;    // Number of pixels per inch on the display
    double LENS_PER_INCH;   // Amount of lenses per inch on the display
    double DISPLAY_WIDTH;
    bool was_calibrated = false;
    std::mutex pixel_exit_angles_mutex;
}

void interlacer::setup() {
    interlacer::PIXELS_PER_INCH = parameters::window_width / parameters::green_to_red_line_distance;
    interlacer::LENS_PER_INCH = parameters::lenticule_density;
    interlacer::DISPLAY_WIDTH = parameters::window_width;

    calculate_pixel_exit_angles();
    //std::cout << "done with setup !!" << std::endl;

    //std::cout << "PIXELS_PER_INCH " << PIXELS_PER_INCH << std::endl;
    //std::cout << "LENS_PER_INCH " << LENS_PER_INCH << std::endl;
    //std::cout << "DISPLAY_WIDTH " << DISPLAY_WIDTH << std::endl;

    for (int i=0; i < pixel_exit_angles.size(); i++) {
        //std::cout << "pixel_exit_angles[" << i << "] = " << pixel_exit_angles[i] << std::endl;
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

    //std::cout << "x: " << x << std::endl;
    //std::cout << "f_prime: " << f_prime << std::endl;
    //std::cout << "N: " << interlacer::N << std::endl;
    //std::cout << "std::atan2(f_prime, 1): " << std::atan2(f_prime, 1) << std::endl;
    //std::cout << " std::sin(std::atan2(f_prime, 1)): " << std::sin(
    //        std::atan2(f_prime, 1)
    //    ) << std::endl;

    //std::cout << "interlacer::N * std::sin(std::atan2(f_prime, 1)): " << interlacer::N * std::sin(
    //        std::atan2(f_prime, 1)
    //    ) << std::endl;

    //std::cout << "std::atan2(f_prime, 1): " << std::atan2(f_prime, 1) << std::endl;
    //std::cout << "std::asin( interlacer::N * std::sin( std::atan2(f_prime, 1) ) ): " << std::asin(
    //    interlacer::N * std::sin(
    //        std::atan2(f_prime, 1)
    //    )
    //) << std::endl;
    //std::cout << "\n\n\n\n" << std::endl;

    return (std::asin(
        interlacer::N * std::sin(
            std::atan2(f_prime, 1)
        )
    ) - std::atan2(f_prime, 1)) * 57.2957795; // 57.2957795 converts it to degrees. It's 180/pi
}

void interlacer::calculate_pixel_exit_angles() {
    interlacer::pixel_exit_angles_mutex.lock();

    interlacer::pixel_exit_angles = std::vector<double>();

    double PIXELS_PER_LENS = interlacer::PIXELS_PER_INCH / interlacer::LENS_PER_INCH;

    //std::cout << "interlacer.cpp. line 73. pixels per inch is " << interlacer::PIXELS_PER_INCH << std::endl;

    //std::cout << "pixels_Per_lens " <<  PIXELS_PER_INCH << std::endl;
    for (int pixel=0; pixel < DISPLAY_WIDTH; pixel++) {
        double x = 2 * std::fmod(pixel, PIXELS_PER_LENS) / PIXELS_PER_LENS - 1; // How far this is from the center of the lens
        double t_val = interlacer::t(x);
        //std::cout << "for pixel " << pixel << " the x valeu is gonna be " << x << " and the value t_val is " << t_val << std::endl;
        interlacer::pixel_exit_angles.push_back(t_val);
    }

    //std::cout << "interlacer.cpp. line 83. the size of pixel_exit_angles is " << interlacer::pixel_exit_angles.size() << std::endl;

    interlacer::pixel_exit_angles_mutex.unlock();
}

void interlacer::calculate_segments(
    int renderer_width,
    unsigned char& is_first_segment_left_eye,
    std::vector<int64_t>& segments_vector,
    float left_eye_horizontal_angle,
    float right_eye_horizontal_angle
) {
    if (renderer_width != interlacer::DISPLAY_WIDTH) {
        interlacer::DISPLAY_WIDTH = renderer_width;
        calculate_pixel_exit_angles();
    }

    interlacer::pixel_exit_angles_mutex.lock();

    //std::cout << "interlacer.cpp. line 92. this is display width: " << interlacer::DISPLAY_WIDTH << std::endl; 


    // Do the first segment
    int first_segment_unknown_pixels = 0;
    float pixel_exit_angle = interlacer::pixel_exit_angles.at(0);

    //std::cout << "interlacer.cpp. line 116. before the while loop." << std::endl;

    while (pixel_exit_angle == std::nanf("")) { // i.e. unknown
        pixel_exit_angle = interlacer::pixel_exit_angles.at(++first_segment_unknown_pixels);
    }

    //std::cout << "interlacer.cpp. line 122. after the while loop." << std::endl;

    bool is_known_pixels_for_left_eye = false;
    int known_pixels_count = first_segment_unknown_pixels+1;

    if (
        std::abs(pixel_exit_angle - left_eye_horizontal_angle) <
        std::abs(pixel_exit_angle - right_eye_horizontal_angle)
    ) {
        // Closer to left eye
        is_first_segment_left_eye = 0;
        is_known_pixels_for_left_eye = true;
        segments_vector.push_back(0);
    } else {
        // Closer to right eye
        is_first_segment_left_eye = 1;
        is_known_pixels_for_left_eye = false;
        segments_vector.push_back(1);
    }

    int unknown_pixels_count = 0;

    // We've done the first blank segment, so now we can go into the loop. Every
    // segment afterwards acts the same.
    for (int i=first_segment_unknown_pixels+1; i<renderer_width; i++) {
        ////std::cout << "interlacer.cpp. line 124. this is i. " << i;

        bool did_eye_flip = false;
        pixel_exit_angle = interlacer::pixel_exit_angles.at(i);

        //std::cout << ". the exit angle is " << pixel_exit_angle << ". ";

        if (pixel_exit_angle == std::nanf("")) {
            // Unknown exit angle
            std::cout << "For the " << i << "th pixel, the pixel_exit_angle was unknown. It is exactly " << pixel_exit_angle << std::endl;
            unknown_pixels_count++;
        }
        else {
            known_pixels_count++;
            if (
                std::abs(pixel_exit_angle - left_eye_horizontal_angle) <
                std::abs(pixel_exit_angle - right_eye_horizontal_angle)
            ) {
                // Closer to left eye
                //std::cout << "it is closer to the left eye ";
                did_eye_flip = is_known_pixels_for_left_eye != true;
            } else {
                // Closer to right eye
                //std::cout << "it is closer to the right eye ";
                did_eye_flip = is_known_pixels_for_left_eye != false;
            }
        }

        //std::cout << std::endl;

        if (did_eye_flip) {
            segments_vector.push_back(known_pixels_count + unknown_pixels_count/2);
            known_pixels_count = unknown_pixels_count/2 + unknown_pixels_count%2;
            unknown_pixels_count = 0;
            is_known_pixels_for_left_eye = !is_known_pixels_for_left_eye;
        }
    }

    //std::cout << "interlacer.cpp this is line 155. we will now print out segments vector. ";
    for (int64_t a : segments_vector) {
        //std::cout << a << " ";
    }
    //std::cout << std::endl;
    //std::cout << "And the length is " << segments_vector.size() << std::endl;

    segments_vector.push_back(-1);

    interlacer::pixel_exit_angles_mutex.unlock();
}

void interlacer::listen_for_renderer_socket_and_call_dispatcher() {
    //std::cout << "Beginning listen." << std::endl;
    shared_vars::acceptor.accept(shared_vars::socket);
    //std::cout << "We accepted something!!!!." << std::endl;
    shared_vars::renderer_ready_dispatcher.emit();
}