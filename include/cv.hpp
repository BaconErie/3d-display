/*
CV Actions. Captures from webcam and processes them.
*/

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


namespace cv_actions {
    // Capture a frame from the webcam
    // Returns false if either unsuccessful or no face detected
    bool detect_face(cv::FaceDetectorYN& face_model, cv::Rect bounding_box, cv::Mat& out_frame, cv::Point& left_eye_position, cv::Point& right_eye_position);
}