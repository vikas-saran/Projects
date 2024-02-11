#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <istream>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <cstdlib> // for malloc and free

using namespace std;
using namespace cv;
using namespace cv::dnn;

double MIN_CONF = 0.1;
double NMS_THRESH = 0.1;
 const bool USE_GPU = false; // Adjust this according to your setup

// Global variables to store the base address and the original new/delete operators
void *baseAddress = nullptr;
std::map<void *, std::size_t> allocatedMemory;
std::size_t totalAllocatedMemory = 0;

// Custom memory manager to track allocated memory blocks
void *customAllocate(std::size_t size)
{
    // Allocate memory using malloc
    void *ptr = std::malloc(size);

    // Track the allocated memory block and its size
    allocatedMemory[ptr] = size;
    totalAllocatedMemory += size; // Update total allocated memory size

    // Return the allocated memory address
    return ptr;
}

// Custom memory deallocator
void customDeallocate(void *ptr)
{
    // Find the memory block in the allocated memory map
    auto it = allocatedMemory.find(ptr);
    if (it != allocatedMemory.end())
    {
        // Get the size of the memory block
        std::size_t size = it->second;

        // Remove the memory block from the map
        allocatedMemory.erase(it);

        // Free the allocated memory
        std::free(ptr);

        // Update total allocated memory size
        totalAllocatedMemory -= size;

        std::cout << "Memory at " << ptr << " deallocated." << std::endl;
    }
    else
    {
        std::cerr << "Error: Attempting to deallocate memory that was not allocated." << std::endl;
    }
}

std::vector<std::tuple<float, cv::Rect, cv::Point>> detect_people(cv::Mat &frame, cv::dnn::Net &net, const std::vector<std::string> &ln, int personIdx)
{
    // grab the dimensions of the frame and initialize the list of results
    int H = frame.rows;
    int W = frame.cols;
    std::vector<std::tuple<float, cv::Rect, cv::Point>> results;
    // construct a blob from the input frame
    Mat blob;
    dnn::blobFromImage(frame, blob, 1 / 255.0, Size(320, 320), Scalar(0, 0, 0), true, false);

    // perform a forward pass of the YOLO object detector, giving us our bounding boxes
    // and associated probabilities
    net.setInput(blob);

    vector<Mat> layerOutputs;
    net.forward(layerOutputs, ln);
    // initialize our lists of detected bounding boxes, centroids, and confidences, respectively
    vector<Rect> boxes;
    vector<Point> centroids;
    vector<float> confidences;
    // loop over each of the layer outputs
    for (const auto &output : layerOutputs)
    {
        // Loop over each of the detection
        for (int row = 0; row < output.rows; ++row)
        {
            cv::Mat detection = output.row(row).clone();

            // Ensure the detection matrix is not empty
            if (!detection.empty())
            {
                // Extract the class ID and confidence
                std::vector<float> scores(detection.begin<float>() + 5, detection.end<float>());
                int classID = std::distance(scores.begin(), std::max_element(scores.begin(), scores.end()));
                float confidence = scores[classID];
                // detected was a person and (2) that the minimum
                // confidence is met
                if (classID == personIdx && confidence > MIN_CONF)
                {
                    // the size of the image, keeping in mind that YOLO
                    // actually returns the center (x, y)-coordinates of
                    // the bounding box followed by the boxes' width and
                    // height
                    cv::Mat box = detection.rowRange(0, 1).clone();
                    box = box.mul(cv::Mat(box.size(), box.type(), cv::Scalar(W, H, W, H)));

                    // Extract values from 'box'

                    int centerX = static_cast<int>(box.at<float>(0, 0));
                    int centerY = static_cast<int>(box.at<float>(0, 1));
                    /// since we din't resize the picture correction on yasix to amtch the square
                    centerY -= (centerY / 3) - 20;
                    int width = static_cast<int>(box.at<float>(0, 2));
                    int height = static_cast<int>(box.at<float>(0, 3));

                    // use the center (x, y)-coordinates to derive the top
                    // and and left corner of the bounding box
                    int x = centerX - (width / 2);
                    int y = centerY - (height / 2);

                    // update our list of bounding box coordinates,
                    // centroids, and confidences
                    boxes.push_back(Rect(x, y, width, height));

                    centroids.push_back(Point(centerX, centerY));
                    confidences.push_back(static_cast<float>(confidence));
                }
            }
        }
    }
    // apply non-maxima suppression to suppress weak, overlapping
    // bounding boxes
    vector<int> idxs;
    cv::dnn::NMSBoxes(boxes, confidences, MIN_CONF, NMS_THRESH, idxs);

    // ensure at least one detection exists
    if (!idxs.empty())
    {
        // loop over the indexes we are keeping
        for (int i : idxs)
        {
            // extract the bounding box coordinates
            int x = boxes[i].x;
            int y = boxes[i].y;
            int w = boxes[i].width;
            int h = boxes[i].height;

            // update our results list to consist of the person
            // prediction probability, bounding box coordinates,
            // and the centroid
            auto r = make_tuple(confidences[i], Rect(x, y, w, h), centroids[i]);
            results.push_back(r);
        }
    }

    // return the list of results
    return results;
}




/// --------------main--function------------------- //////////////////////////////////////////////

int main(int argc, char *argv[])
{
    // Check if the user provided the video file path and base address as command-line arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <video_file_path> <base_address>" << std::endl;
        return -1;
    }

    // Convert the provided base address from hexadecimal string to a memory address
    std::stringstream ss;
    ss << std::hex << argv[2];
    ss >> baseAddress;

    cv::dnn::Net *net = new cv::dnn::Net();
    // Read the Darknet model in C++
    auto result = cv::dnn::readNetFromDarknet("C:/Users/vikassaran/Downloads/complete_code/yolov4-tiny.cfg","C:/Users/vikassaran/Downloads/complete_code/yolov4-tiny.weights");
    result.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    result.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    *net = result;

    // Check if GPU is enabled
    if (USE_GPU)
    {
        std::cout << "[INFO] setting preferable backend and target to CUDA..." << std::endl;
        net->setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net->setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }

    /////////////////// could do it with custom allocator but the exct size of string vector isnt coming correct by the following formula
    //////////////////By default, std::vector handles memory allocation and deallocation
    //////////////////internally using the standard allocator provided by the C++ standard library.

    // Get the sizes
    /// size_t lnSize = net->getLayerNames().size();
    /// size_t unconnectedSize = net->getUnconnectedOutLayers().size();

    /// size_t lnMemorySize = lnSize * sizeof(std::string) + sizeof(std::vector<std::string>);
    /// size_t unconnectedMemorySize = unconnectedSize * sizeof(int) + sizeof(std::vector<int>);

    /// std::vector<std::string> *ln = static_cast<std::vector<std::string> *>(customAllocate(lnMemorySize));
    ////std::vector<int> *unconnectedIndices = static_cast<std::vector<int> *>(customAllocate(unconnectedMemorySize));
    ////std::vector<std::string> *lnUpdated = static_cast<std::vector<std::string> *>(customAllocate(unconnectedMemorySize));

    std::vector<std::string> ln = net->getLayerNames();
    // Get unconnected output layers
    std::vector<int> unconnectedIndices = net->getUnconnectedOutLayers();
    // Update ln using unconnected indices
    std::vector<std::string> lnUpdated;
    for (int i = 0; i < unconnectedIndices.size(); i++)
    {
        int p = unconnectedIndices[i] - 1;
        lnUpdated.push_back(ln[p]);
        // cerr << "Updated ln:" << lnUpdated[i] << endl;
    }

    // Open the video file
    cv::VideoCapture cap(argv[1]);

    // Check if video opened successfully
    if (!cap.isOpened())
    {
        std::cerr << "Error opening video file" << std::endl;
        // Deallocate memory for weightsPath, configPath, and net
        delete net;
        return -1;
    }

    // Variables for storing frame width, height, and data pointer
    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int frameSize = frameWidth * frameHeight * 3; // Assuming 3 channels (BGR)

    // Allocate memory for storing frame data
    uchar *frameData = static_cast<uchar *>(customAllocate(frameSize));

    // Allocate memory for various Mat objects
    uchar *grayFrameData = static_cast<uchar *>(customAllocate(frameWidth * frameHeight));
    uchar *prevGrayFrameData = static_cast<uchar *>(customAllocate(frameWidth * frameHeight));
    uchar *fgMaskDiffData = static_cast<uchar *>(customAllocate(frameWidth * frameHeight));

    // Create background subtractor
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor = cv::createBackgroundSubtractorMOG2();

    // Allocate cv::Mat objects without copying data
    cv::Mat frame(frameHeight, frameWidth, CV_8UC3, frameData);
    cv::Mat grayFrame(frameHeight, frameWidth, CV_8UC1, grayFrameData);
    cv::Mat prevGrayFrame(frameHeight, frameWidth, CV_8UC1, prevGrayFrameData);
    cv::Mat fgMaskDiff(frameHeight, frameWidth, CV_8UC1, fgMaskDiffData);

    
    int *p = static_cast<int *>(customAllocate(sizeof(int)));//a counter from 0-15 
    *p = 0;


    // Main loop
    while (true)
    {
       
        // Capture current frame
        cv::Mat tempFrame;
        if (!cap.read(tempFrame))
            break;

        // Copy frame data into allocated memory
        memcpy(frameData, tempFrame.data, frameSize);
        uchar *fgMaskSubData = static_cast<uchar *>(customAllocate(frameWidth * frameHeight));
        uchar *fgMaskCombinedData = static_cast<uchar *>(customAllocate(frameWidth * frameHeight));
        cv::Mat fgMaskSub(frameHeight, frameWidth, CV_8UC1, fgMaskSubData);
        cv::Mat fgMaskCombined(frameHeight, frameWidth, CV_8UC1, fgMaskCombinedData);

        // Perform frame differencing
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::Mat tempPrevGrayFrame = prevGrayFrame.clone();
        prevGrayFrame = grayFrame.clone();
        cv::absdiff(tempPrevGrayFrame, grayFrame, fgMaskDiff);
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

        // Assuming you want to resize to 700x700
        std::vector<std::tuple<float, cv::Rect, cv::Point>> results;
        if (*p % 15 == 0)
        {
            // resize(frame, frame, Size(700, 700));
            results = detect_people(frame, *net, lnUpdated, 0);
            *p=0;
        }
        // Loop over the results
        for (size_t i = 0; i < results.size(); ++i)
        {
            float prob = std::get<0>(results[i]);
            cv::Rect bbox = std::get<1>(results[i]);
            cv::Point centroid = std::get<2>(results[i]);

            // Extract the bounding box and centroid coordinates, then initialize the color of the annotation
            int startX = bbox.x;
            int startY = bbox.y;
            int endX = bbox.x + bbox.width;
            int endY = bbox.y + bbox.height;
            Point c(centroid.x, centroid.y);
            Scalar color(0, 255, 0);
            cv::rectangle(frame, Point(startX, startY), Point(endX, endY), color, 2);
        }
        // Display the frame with contours
        cv::imshow("Combined Detection", frame);
        customDeallocate(fgMaskSubData);
        customDeallocate(fgMaskCombinedData);
        (*p)++;

         int key = cv::waitKey(1) & 0xFF;
        // If the 'q' key was pressed, break from the loop
        if (key == 'q')
        {
            break;
        }
    }

    // Output total allocated memory size
    std::cout << "Total allocated memory size: " << totalAllocatedMemory << " bytes" << std::endl;

    // Deallocate memory for Mat objects
    customDeallocate(frameData);
    customDeallocate(grayFrameData);
    customDeallocate(prevGrayFrameData);
    customDeallocate(fgMaskDiffData);
    delete net;
    
    customDeallocate(p);


    // Output total allocated memory size
    std::cout << "Total allocated memory size: " << totalAllocatedMemory << " bytes" << std::endl;

    // Release the video file and close all windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}