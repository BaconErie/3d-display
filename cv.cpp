#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/highgui.hpp"
#include <opencv2/objdetect.hpp>
#include <chrono>
#include <vector>
#include <algorithm>
#include <queue>

const float SEARCH_AREA_SIZE = 1.5f;
const int SMOOTHING_QUEUE_SIZE = 5;

void detect_face_in_bounds(cv::Rect& face_rect, cv::Mat& grayscale_mat, cv::Rect& search_bounds, cv::CascadeClassifier& face_model) {
    /* Detects and gives the first face in the specified bounds.
       Mat must be already grayscale 
    */

    cv::Mat sub_mat = cv::Mat(grayscale_mat, search_bounds);

    std::vector<cv::Rect> faces;
    face_model.detectMultiScale(sub_mat, faces);

    if (faces.size() > 0) {
        face_rect = faces[0];
        face_rect.x += search_bounds.x;
        face_rect.y += search_bounds.y;
    } else {
        face_rect = cv::Rect(-1, -1, -1, -1);
    }
}

void detect_first_second_object_in_bounds(cv::Rect& first_rect, cv::Rect& second_rect, cv::Mat& grayscale_mat, cv::Rect& search_bounds, cv::CascadeClassifier& detector_model) {
    /* Detects and gives the two objects in the specified bounds.
       Gets the objects with the lowest y coordinates first (i.e. higher up)
       Mat must be already grayscale 
    */

    cv::Mat sub_mat = cv::Mat(grayscale_mat, search_bounds);

    std::vector<cv::Rect> objects;
    detector_model.detectMultiScale(sub_mat, objects);

    first_rect = cv::Rect(-1, -1, -1, -1);
    second_rect = cv::Rect(-1, -1, -1, -1);
    
    for (int i=0; i<objects.size(); i++) {
        if (objects[i].y < first_rect.y || first_rect == cv::Rect(-1, -1, -1, -1)) {
            second_rect = first_rect;
            first_rect = objects[i];
        } else if (objects[i].y < second_rect.y || second_rect == cv::Rect(-1, -1, -1, -1)) {
            second_rect = objects[i];
        }
    }


    if (first_rect != cv::Rect(-1, -1, -1, -1)) {
        first_rect.x += search_bounds.x;
        first_rect.y += search_bounds.y;
    }

    if (second_rect != cv::Rect(-1, -1, -1, -1)) {
        second_rect.x += search_bounds.x;
        second_rect.y += search_bounds.y;
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

    cv::CascadeClassifier eye_detector_model;
    if (!eye_detector_model.load("models/eye_detector_model.xml")) {
        std::cerr << "Error: Could not load eye model." << std::endl;
        return -1;
    }

    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point cv_begin;
    std::chrono::steady_clock::time_point cv_end;
    cv::Rect face_rect;
    cv::Rect search_bounds; // Initial search bounds

    cv::Rect eye1;
    cv::Rect eye2;

    cv::Mat frame;
    cv::Mat grayscale_frame;

    int no_face_counter = 0;

    int last_fps = 0;
    int percent_time_in_cv = 0;

    std::queue<int> last_x_vals = std::queue<int>();
    std::queue<int> last_y_vals = std::queue<int>();
    int moving_window_x_sum = 0;
    int moving_window_y_sum = 0;

    while (true) {
        begin = std::chrono::steady_clock::now();
       
        
        cap >> frame; // Capture a frame
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame." << std::endl;
            return -1;
        }

        if (search_bounds == cv::Rect(0,0,0,0)) {
            search_bounds = cv::Rect(0, 0, frame.cols, frame.rows);
        }


        cv::rectangle(frame, search_bounds, cv::Scalar(255, 0, 0), 2);

        cv::cvtColor(frame, grayscale_frame, cv::COLOR_BGR2GRAY);

        cv::putText(frame, "FPS: " + std::to_string(last_fps) + " Percent time in CV: " + std::to_string(percent_time_in_cv) + "%", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

        
        cv_begin = std::chrono::steady_clock::now();
        detect_face_in_bounds(face_rect, grayscale_frame, search_bounds, face_detector_model);

        if (face_rect.width != -1) {
            cv::rectangle(frame, face_rect, cv::Scalar(0, 255, 0), 2);

            int x_new = std::clamp((int)(face_rect.x + face_rect.width/2 - face_rect.width*SEARCH_AREA_SIZE/2), 0, frame.cols);
            int y_new = std::clamp((int)(face_rect.y + face_rect.height/2 - face_rect.height*SEARCH_AREA_SIZE/2), 0, frame.rows);
            int width_new = std::clamp((int)(face_rect.width*SEARCH_AREA_SIZE), 0, frame.cols - x_new);
            int height_new = std::clamp((int)(face_rect.height*SEARCH_AREA_SIZE), 0, frame.rows - y_new);

            search_bounds = cv::Rect(
                x_new,
                y_new,
                width_new,
                height_new
            );

            last_x_vals.push(face_rect.x + face_rect.width/2);
            last_y_vals.push(face_rect.y + face_rect.height/2);
            moving_window_x_sum += face_rect.x + face_rect.width/2;
            moving_window_y_sum += face_rect.y + face_rect.height/2;

            if (last_x_vals.size() > SMOOTHING_QUEUE_SIZE) {
                moving_window_x_sum -= last_x_vals.front();
                last_x_vals.pop();
            }
            if (last_y_vals.size() > SMOOTHING_QUEUE_SIZE) {
                moving_window_y_sum -= last_y_vals.front();
                last_y_vals.pop();
            }

            int x_avg = moving_window_x_sum / last_x_vals.size();
            int y_avg = moving_window_y_sum / last_y_vals.size();

            // Look for eye1 and eye2

            cv::Rect eye_search_bounds = cv::Rect(face_rect.x, face_rect.y, face_rect.width, face_rect.height/2);
            detect_first_second_object_in_bounds(eye1, eye2, grayscale_frame, eye_search_bounds, eye_detector_model);

            if (eye1.width != -1) {
                cv::circle(frame, cv::Point(eye1.x + eye1.width/2, eye1.y + eye1.height/2), 5, cv::Scalar(0, 0, 255), -1);  
                cv::rectangle(frame, eye1, cv::Scalar(0, 0, 255), 2);
            }
            if (eye2.width != -1) {
                cv::circle(frame, cv::Point(eye2.x + eye2.width/2, eye2.y + eye2.height/2), 5, cv::Scalar(0, 0, 255), -1);
                cv::rectangle(frame, eye2, cv::Scalar(0, 0, 255), 2);
            }

            cv::circle(frame, cv::Point(x_avg, y_avg), 5, cv::Scalar(0, 255, 0), -1);

            no_face_counter = 0; 
        } else {
            no_face_counter++;
            if (no_face_counter >= 3) { // If no face detected for 3 consecutive frames, reset search bounds
                search_bounds = cv::Rect(0, 0, frame.cols, frame.rows);
                no_face_counter = 0;
            }
        }
        cv_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> cv_elapsed_seconds = cv_end - cv_begin;

        cv::imshow("Webcam", frame);
        if (cv::waitKey(1000/120) == 'q') { // Wait for a key press
            break;
        }

        end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - begin;
        percent_time_in_cv = int(cv_elapsed_seconds.count() / elapsed_seconds.count() * 100.0);
        last_fps = (int)(1.0 / elapsed_seconds.count());

        
    }

    return 0;
}