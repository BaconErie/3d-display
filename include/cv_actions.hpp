/*
CV Actions. Captures from webcam and processes them.
*/

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <cmath>


const float SEARCH_AREA_SIZE = 1.5f;

namespace cv_actions {
    // Capture a frame from the webcam
    // Returns false if either unsuccessful or no face detected
    bool detect_face(
        cv::Ptr<cv::FaceDetectorYN>& face_model_pointer,
        cv::Rect& bounding_box,
        cv::VideoCapture& cap,
        cv::Mat& out_frame,
        cv::Point& left_eye_position,
        cv::Point& right_eye_position
    );

    bool detect_qr(
        cv::VideoCapture& cap,
        cv::Mat& out_frame,
        float& qr_code_inverse_proportion
    );
}