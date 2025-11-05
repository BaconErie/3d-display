#pragma once

#include <gtk/gtk.h>
#include <glibmm.h>

#include <opencv2/videoio.hpp>
#include <opencv2/objdetect/face.hpp>
#include <opencv2/core/types.hpp>

namespace shared {
    extern GdkPaintable* webcam_paintable;

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
}