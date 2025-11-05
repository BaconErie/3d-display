#include "shared.hpp"

namespace shared {
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
    GtkEditable* green_red_line_distance_editable = nullptr;
    GtkEditable* horizontal_displacement_editable = nullptr;
    GtkEditable* vertical_displacement_editable = nullptr;
}