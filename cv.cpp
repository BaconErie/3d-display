#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/highgui.hpp"
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/dnn.hpp>
#include <chrono>
#include <vector>
#include <algorithm>
#include <queue>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

const float SEARCH_AREA_SIZE = 1.5f;
const int SMOOTHING_QUEUE_SIZE = 5;


/** Variables that will be shared by ALL actions
 *  Do not include variables that are only relevant to main
 */
namespace shared_variables {
    cv::VideoCapture cap;
}


/** Data that is used by the face action
 *  Declare in global so that we can reuse the values
 */
namespace face_action_variables {
    // Timing variables
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::steady_clock::time_point cv_begin;
    std::chrono::steady_clock::time_point cv_end;
    int last_fps = 0;
    int percent_time_in_cv = 0;

    cv::Rect face_rect; // Output for face detection
    cv::Rect search_bounds; // Initial search bounds

    cv::Point left_eye; // Output for left eye detection
    cv::Point right_eye; // Output for right eye detection

    cv::Mat frame; // Current frame from webcam

    int no_face_counter = 0; // Counts how many frames in a row no face was detected
                             // Used to reset search bounds if too many frames in a row have no face

    // Smoothing variables
    std::queue<int> last_x_vals;
    std::queue<int> last_y_vals;
    int moving_window_x_sum = 0;
    int moving_window_y_sum = 0;

    std::shared_ptr<cv::FaceDetectorYN> face_detector;
}

/*******************************
*                              *
*   DECLARE HELPER FUNCTIONS   *
*                              *
*******************************/

void detect_face_and_eyes_in_bounds(cv::Rect& face_rect, cv::Point& left_eye, cv::Point& right_eye, cv::Mat& full_mat, cv::Rect& search_bounds, std::shared_ptr<cv::FaceDetectorYN>& face_model);

/*******************************
*                              *
*   ACTIONS                    *
*                              *
*******************************/

/** Takes a frame from the webcam and detects faces in the webcam.
 * Draws rectangles and everything, and gives back a mat with the drawn stuff.
 */
void face_action(cv::Mat& frame) {
    using namespace face_action_variables;

    begin = std::chrono::steady_clock::now();
       
        
    shared_variables::cap >> frame; // Capture a frame

    if (frame.empty()) {
        std::cerr << "Error: Could not capture frame." << std::endl;
        return;
    }

    if (search_bounds == cv::Rect(0,0,0,0)) {
        // First frame, set search bounds to whole frame
        search_bounds = cv::Rect(0, 0, frame.cols, frame.rows);
    }


    cv::rectangle(frame, search_bounds, cv::Scalar(255, 0, 0), 2);

    cv::putText(frame, "FPS: " + std::to_string(last_fps) + " Percent time in CV: " + std::to_string(percent_time_in_cv) + "%", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

    
    cv_begin = std::chrono::steady_clock::now();
    detect_face_and_eyes_in_bounds(face_rect, left_eye, right_eye, frame, search_bounds, face_detector);

    if (face_rect.width != -1) {
        // Face detected

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

        // Draw left eye, right eye

        cv::circle(frame, left_eye, 5, cv::Scalar(0, 0, 255), -1);  
        cv::circle(frame, right_eye, 5, cv::Scalar(0, 0, 255), -1);

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

    end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - begin;
    percent_time_in_cv = int(cv_elapsed_seconds.count() / elapsed_seconds.count() * 100.0);
    last_fps = (int)(1.0 / elapsed_seconds.count());
}

void qr_action(cv::Mat output) {
    // TODO
}

/*******************************
*                              *
*   SETUP FUNCTION             *
*                              *
*******************************/


/**
 * Sets up variables in all the namespaces, but only the namespaces,
 * so that they can be used by the main function and by action functions
 * 
 * Returns anything other than 0 if error.
 */
int setup_namespaces() {
    // Load face model
    face_action_variables::face_detector = cv::FaceDetectorYN::create("models/face_detector_model.onnx", "", cv::Size(1, 1), 0.9, 0.3, 1);

    shared_variables::cap.open(0); // Open the default camera
    if (!shared_variables::cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    return 0;
}


int main() {
    // Setup namespaces
    int result = setup_namespaces();
    if (result != 0) {
        return result;
    }

    // Output frame 
    cv::Mat output;
    
    std::cout << std::flush;

    std::string line;

    while (true) {
        std::getline(std::cin, line);

        if (line == "exit") {
            return 0;
        } else if (line == "face") {
            face_action(output);
            cv::imwrite("Window.png", output);
            std::cout << "done\n";
        }
    }
}

/*******************************
*                              *
*   DEFINE HELPER FUNCTIONS    *
*                              *
*******************************/

void detect_face_and_eyes_in_bounds(cv::Rect& face_rect, cv::Point& left_eye, cv::Point& right_eye, cv::Mat& full_mat, cv::Rect& search_bounds, std::shared_ptr<cv::FaceDetectorYN>& face_model) {
    /* Detects and gives the first face in the specified bounds.
    */

    if (search_bounds.width < 63 || search_bounds.height < 63) {
        // Minimum size for the face detector is 63x63, otherwise it crashes(?)
        face_rect = cv::Rect(-1, -1, -1, -1);
        return;
    }
   

    face_model->setInputSize(search_bounds.size());

    cv::Mat sub_mat = cv::Mat(full_mat, search_bounds);
    cv::Mat output_array;

    face_model->detect(sub_mat, output_array);

    if (output_array.rows > 0) {
        face_rect = cv::Rect(
            (int)output_array.at<float>(0,0) + search_bounds.x,
            (int)output_array.at<float>(0,1) + search_bounds.y,
            (int)output_array.at<float>(0,2),
            (int)output_array.at<float>(0,3)
        );

        right_eye = cv::Point(
            (int)output_array.at<float>(0,4) + search_bounds.x,
            (int)output_array.at<float>(0,5) + search_bounds.y
        );
        left_eye = cv::Point(
            (int)output_array.at<float>(0,6) + search_bounds.x,
            (int)output_array.at<float>(0,7) + search_bounds.y
        );
    } else {
        face_rect = cv::Rect(-1, -1, -1, -1);
    }
}