#include "cv_actions.hpp"

bool cv_actions::detect_face(
    cv::Ptr<cv::FaceDetectorYN>& face_model, 
    cv::Rect& search_bounds,
    cv::VideoCapture &cap,
    cv::Mat& out_frame, 
    cv::Point& left_eye_position,
    cv::Point& right_eye_position
) {
    if (search_bounds.width < 63 || search_bounds.height < 63) {
        // Minimum size for the face detector is 63x63, otherwise it crashes(?)
        return false;
    }
    
    if (!cap.isOpened()) {
        return false; // Failed to open webcam
    }

    cap >> out_frame; // Capture a full_frame
    if (out_frame.empty()) {
        return false; // Failed to capture full_frame
    }

    face_model->setInputSize(search_bounds.size());

    cv::Mat sub_mat = cv::Mat(out_frame, search_bounds);
    cv::Mat output_array;
    face_model->detect(sub_mat, output_array);

    if (output_array.rows == 0) {
        search_bounds = cv::Rect(
            0,
            0,
            out_frame.cols,
            out_frame.rows
        ); // Reset search bounds
        return false; // No face detected
    }

    int face_x = (int)output_array.at<float>(0, 0) + search_bounds.x;
    int face_y = (int)output_array.at<float>(0, 1) + search_bounds.y;
    int face_width = (int)output_array.at<float>(0, 2);
    int face_height = (int)output_array.at<float>(0, 3);
    cv::Rect face_rect(face_x, face_y, face_width, face_height);


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
        face_x,
        face_y,
        face_width,
        face_height
    ), cv::Scalar(0, 255, 0), 2);

    // Draw eye positions
    cv::circle(out_frame, left_eye_position, 5, cv::Scalar(255, 0, 0), -1);
    cv::circle(out_frame, right_eye_position, 5, cv::Scalar(255, 0, 0), -1);

    int x_new = std::clamp((int)(face_x + face_width/2 - face_width*SEARCH_AREA_SIZE/2), 0, out_frame.cols);
    int y_new = std::clamp((int)(face_y + face_height/2 - face_height*SEARCH_AREA_SIZE/2), 0, out_frame.rows);
    int width_new = std::clamp((int)(face_width*SEARCH_AREA_SIZE), 0, out_frame.cols - x_new);
    int height_new = std::clamp((int)(face_height*SEARCH_AREA_SIZE), 0, out_frame.rows - y_new);

    search_bounds = cv::Rect(
        x_new,
        y_new,
        width_new,
        height_new
    );

    return true;
}