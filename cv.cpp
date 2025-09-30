#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/highgui.hpp"
#include <opencv2/objdetect.hpp>
#include <chrono>
#include <vector>

void detect_first_face_in_bounds(cv::Rect& output_rect, cv::Mat& grayscale_mat, cv::Rect& search_bounds, cv::CascadeClassifier& face_detector_model) {
    /* Detects and gives the FIRST face detected, as a Rect. Mat must be already grayscale */

    if (grayscale_mat.type() != CV_8UC1) {
        std::cerr << "Error: detect_first_face_in_bounds's Mat must be grayscale." << std::endl;
        return;
    }

    cv::Mat sub_mat = cv::Mat(grayscale_mat, search_bounds);

    std::vector<cv::Rect> faces;
    face_detector_model.detectMultiScale(sub_mat, faces);

    if (faces.size() == 0) {
        output_rect = cv::Rect(-1, -1, -1, -1);
    } else {
        output_rect = faces[0] + search_bounds.tl(); // Adjust to full image coordinates
    }
    
}

int main() {
    std::cout << "Hello, helper!" << std::endl;

    // Load face model
    cv::CascadeClassifier face_detector_model;
    if (!face_detector_model.load("models/face_detector_model.xml")) {
        std::cerr << "Error: Could not load face model." << std::endl;
        return -1;
    }

    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end;
    cv::Rect face_rect;
    cv::Rect search_bounds; // Initial search bounds

    cv::Mat frame;
    cv::Mat grayscale_frame;
    
    while (true) {
        end = std::chrono::steady_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - begin;
        int fps = (int)(1.0 / elapsed_seconds.count());

        begin = std::chrono::steady_clock::now();
        
        cap >> frame; // Capture a frame
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame." << std::endl;
            return -1;
        }

        search_bounds = cv::Rect(frame.cols/2, 0, frame.cols/2, frame.rows);
        cv::rectangle(frame, search_bounds, cv::Scalar(255, 0, 0), 2);

        cv::cvtColor(frame, grayscale_frame, cv::COLOR_BGR2GRAY);

        cv::putText(frame, "FPS: " + std::to_string(fps), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

        detect_first_face_in_bounds(face_rect, grayscale_frame, search_bounds, face_detector_model);

        if (face_rect != cv::Rect(-1, -1, -1, -1)) {
            cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("Webcam", frame);
        if (cv::waitKey(1000/120) == 'q') { // Wait for a key press
            break;
        }

        
    }

    return 0;
}