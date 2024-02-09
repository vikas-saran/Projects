#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <map>

// Global variables to store the base address and the original new/delete operators
void* baseAddress = nullptr;

int main(int argc, char *argv[]) {
    // Check if the user provided the video file path and base address as command-line arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <video_file_path> <base_address>" << std::endl;
        return -1;
    }

    // Convert the provided base address from hexadecimal string to a memory address
    std::stringstream ss;
    ss << std::hex << argv[2];
    ss >> baseAddress;

    // Open the video file
    cv::VideoCapture cap(argv[1]);

    // Check if video opened successfully
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file" << std::endl;
        return -1;
    }

    // Create the background subtractor object
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor = cv::createBackgroundSubtractorMOG2();

    cv::Mat frame, fgMaskDiff, fgMaskSub, fgMaskCombined;

    // Main loop
    while (1) {
        // Capture current frame
        cap >> frame;

        // Check if the frame is empty
        if (frame.empty())
            break;

        // Perform frame differencing
        cv::Mat grayFrame;
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::Mat prevGrayFrame;
        static cv::Mat prevFrame;
        if (prevFrame.empty())
            prevGrayFrame = grayFrame.clone();
        else
            prevGrayFrame = prevFrame.clone();
        prevFrame = grayFrame.clone();
        cv::absdiff(prevGrayFrame, grayFrame, fgMaskDiff);
        cv::threshold(fgMaskDiff, fgMaskDiff, 30, 255, cv::THRESH_BINARY);

        // Apply background subtraction
        bgSubtractor->apply(frame, fgMaskSub);

        // Combine the foreground masks
        cv::bitwise_and(fgMaskDiff, fgMaskSub, fgMaskCombined);

        // Find contours in the combined foreground mask
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(fgMaskCombined, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Draw contours on the frame with thinner thickness
        cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), 1); // Adjust thickness here

        // Display the frame with contours
        cv::imshow("Combined Detection", frame);

        // Break the loop if the user presses the ESC key
        if (cv::waitKey(30) == 27)
            break;
    }

    // Release the video file and close all windows
    cap.release();
    cv::destroyAllWindows();

    // Release all remaining allocated memory blocks
  

    return 0;
}










