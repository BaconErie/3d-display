#include <iostream>
#include <opencv2/videoio.hpp>
#include "opencv2/highgui.hpp"



int main() {
    std::cout << "Hello, helper!" << std::endl;

    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    cv::Mat frame;
    cap >> frame; // Capture a frame
    if (frame.empty()) {
        std::cerr << "Error: Could not capture frame." << std::endl;
        return -1;
    }

    cv::imshow("Webcam", frame);
    cv::waitKey(0); // Wait for a key press

    return 0;
}