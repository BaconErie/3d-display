#include "shared.hpp"

namespace shared_vars {
    GdkPaintable* webcam_paintable = nullptr;

    std::mutex webcam_paintable_mutex;
    cv::VideoCapture webcam_capture;
    Glib::Dispatcher webcam_dispatcher;
    cv::Ptr<cv::FaceDetectorYN> face_detector_pointer;
    cv::Rect bounding_box(0, 0, 0, 0);

    GtkPicture* main_webcam_image = nullptr;
    GtkPicture* fov_webcam_image = nullptr;

    GtkStack* stack_widget = nullptr;

    GtkEditable* qr_code_distance_editable = nullptr;
    GtkEditable* lenticule_density_editable = nullptr;
    GtkEditable* index_of_refraction_editable = nullptr;
    GtkEditable* green_red_line_distance_editable = nullptr;
    GtkEditable* horizontal_displacement_editable = nullptr;
    GtkEditable* vertical_displacement_editable = nullptr;

    bool is_current_cv_action_face = true;

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address(boost::asio::ip::address_v4(2130706433)), 42842);    
    boost::asio::ip::tcp::acceptor acceptor(io_context);

    Glib::Dispatcher renderer_ready_dispatcher;
    bool is_renderer_active = false;

    GtkBuilder *builder = nullptr;

    double left_eye_horizontal_angle = 0;
    double right_eye_horizontal_angle = 0;
    double left_eye_vertical_angle = 0;
    double right_eye_vertical_angle = 0;
}

void shared_vars::listen_for_renderer_socket_and_call_dispatcher() {
    //std::cout << "Beginning listen." << std::endl;
    shared_vars::acceptor.accept(shared_vars::socket);
    //std::cout << "We accepted something!!!!." << std::endl;
    shared_vars::renderer_ready_dispatcher.emit();
}

namespace parameters {
    float qr_code_distance = 0;
    float qr_code_inverse_proportion = 0;
    float webcam_fov_deg = 0;
    float lenticule_density = 0;
    float index_of_refraction = 1.5;
    float green_to_red_line_distance = 0;
    float horizontal_displacement = 0;
    float vertical_displacement = 0;
    float window_width = 0;
}