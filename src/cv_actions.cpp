#include "cv_actions.hpp"

bool cv_actions::detect_face(cv::FaceDetectorYN& face_model, cv::Rect search_bounds, cv::Mat& out_frame, cv::Point& left_eye_position, cv::Point& right_eye_position) {
    if (search_bounds.width < 63 || search_bounds.height < 63) {
        // Minimum size for the face detector is 63x63, otherwise it crashes(?)
        return false;
    }
    
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        return false; // Failed to open webcam
    }

    cv::Mat full_frame;
    cap >> full_frame; // Capture a full_frame
    if (full_frame.empty()) {
        return false; // Failed to capture full_frame
    }

    face_model.setInputSize(search_bounds.size());

    cv::Mat sub_mat = cv::Mat(full_frame, search_bounds);
    cv::Mat output_array;
    face_model.detect(full_frame, output_array);

    if (output_array.rows == 0) {
        return false; // No face detected
    }


    left_eye_position = cv::Point(
        (int)output_array.at<float>(0, 6) + search_bounds.x,
        (int)output_array.at<float>(0, 7) + search_bounds.y
    );
    right_eye_position = cv::Point(
        (int)output_array.at<float>(0, 4) + search_bounds.x,
        (int)output_array.at<float>(0, 5) + search_bounds.y
    );


    // Draw search bounds
    cv::rectangle(out_frame, search_bounds, cv::Scalar(255, 0, 0), 2);

    // Draw face rectangle
    cv::rectangle(out_frame, cv::Rect(
        (int)output_array.at<float>(0, 0) + search_bounds.x,
        (int)output_array.at<float>(0, 1) + search_bounds.y,
        (int)output_array.at<float>(0, 2),
        (int)output_array.at<float>(0, 3)
    ), cv::Scalar(0, 255, 0), 2);

    // Draw eye positions
    cv::circle(out_frame, left_eye_position, 5, cv::Scalar(255, 0, 0), -1);
    cv::circle(out_frame, right_eye_position, 5, cv::Scalar(255, 0, 0), -1);
    
    return true;
}