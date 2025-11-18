#pragma once

#include <cstdint>

#include <gtk/gtk.h>
#include <glibmm.h>

#include <opencv2/videoio.hpp>
#include <opencv2/objdetect/face.hpp>
#include <opencv2/core/types.hpp>

#include <boost/asio.hpp>

namespace shared_vars {
    extern GdkPaintable* webcam_paintable;

    extern GtkBuilder *builder;

    extern std::mutex webcam_paintable_mutex;
    extern cv::VideoCapture webcam_capture;
    extern Glib::Dispatcher webcam_dispatcher;
    extern cv::Ptr<cv::FaceDetectorYN> face_detector_pointer;
    extern cv::Rect bounding_box;

    extern GtkPicture* main_webcam_image;
    extern GtkPicture* fov_webcam_image;

    extern GtkStack* stack_widget;

    extern GtkEditable* qr_code_distance_editable;
    extern GtkEditable* lenticule_density_editable;
    extern GtkEditable* green_red_line_distance_editable;
    extern GtkEditable* horizontal_displacement_editable;
    extern GtkEditable* vertical_displacement_editable;

    extern bool is_current_cv_action_face;

    extern boost::asio::io_context io_context;
    extern boost::asio::ip::tcp::socket socket;
    extern boost::asio::ip::tcp::endpoint endpoint;
    extern boost::asio::ip::tcp::acceptor acceptor;

    extern Glib::Dispatcher renderer_ready_dispatcher; 
}

namespace parameters {
    extern float qr_code_distance;
    extern float qr_code_inverse_proportion;
    extern float webcam_fov_deg;
    extern float lenticule_density;
    extern float green_to_red_line_distance;
    extern float horizontal_displacement;
    extern float vertical_displacement;
    extern float window_width;
}