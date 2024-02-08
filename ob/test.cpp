#include <iostream>
#include <fstream>
#include <istream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
using namespace cv::dnn;
string MODEL_PATH = "yolo";
double MIN_CONF = 0.1;
double NMS_THRESH = 0.1;
bool Thread = false; // Set default value
// string url = "";  // Set default value
bool USE_GPU = false; // Set default value
std::vector<std::tuple<float, cv::Rect, cv::Point>> detect_people(cv::Mat& frame, cv::dnn::Net& net, const std::vector<std::string>& ln, int personIdx)
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
    //cout<<"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"<<endl;
   net.forward(layerOutputs, ln);
     
    // cout<<"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"<<endl;
  
    // initialize our lists of detected bounding boxes, centroids, and confidences, respectively
    vector<Rect> boxes;
    vector<Point> centroids;
    vector<float> confidences;
    // loop over each of the layer outputs
    for (const auto& output : layerOutputs) {
        // Loop over each of the detection
        for (int row = 0; row < output.rows; ++row) {
            cv::Mat detection = output.row(row).clone();

            // Ensure the detection matrix is not empty
            if (!detection.empty()) {
                // Extract the class ID and confidence
                std::vector<float> scores(detection.begin<float>() + 5, detection.end<float>());
                int classID = std::distance(scores.begin(), std::max_element(scores.begin(), scores.end()));
                float confidence = scores[classID];
                // detected was a person and (2) that the minimum
                // confidence is met
                if (classID == personIdx && confidence > MIN_CONF)
                {
                   // cout << classID << "-" << confidence << endl;
                    // cout<< detection <<"-" << classID << "-" << confidence << endl;
                     // scale the bounding box coordinates back relative to
                     // the size of the image, keeping in mind that YOLO
                     // actually returns the center (x, y)-coordinates of
                     // the bounding box followed by the boxes' width and
                     // height
                    cv::Mat box = detection.rowRange(0, 1).clone();
                    box = box.mul(cv::Mat(box.size(), box.type(), cv::Scalar(W, H, W, H)));

                    // Extract values from 'box'
                  
                    int centerX = static_cast<int>(box.at<float>(0, 0));
                    int centerY = static_cast<int>(box.at<float>(0, 1));
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


int main(int argc, char** argv)
{
    // if (argc < 1)
    //{
    //     std::cerr << "Usage: " << argv[0] << " [options]" << std::endl;
    //     return -1;
    // }
    // Initialize the command line parser
    // cv::CommandLineParser parser(argc, argv);

    // Check if a specific command-line option is provided
    // if (parser.has("help")) {
    //   std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    // Provide additional help information if needed
    //  return 0;
    //}
   
    // string inputVideo = parser.get<string>("-i");
    // string outputVideo = parser.get<string>("-o");
   // string labelsPath = "C:/Users/vikassaran/Downloads/opencv1/ob/coco.names";
   // vector<string> LABELS;
   // ifstream labelsFile(labelsPath);
   // if (labelsFile.is_open())
   // {
   //     string label;
   //     while (getline(labelsFile, label))
   //     {
   //         LABELS.push_back(label);
   //     }
   //     labelsFile.close();
   // }
   // else
   // {
   //     cerr << "Error opening labels file" << endl;
   //     return -1;
   // }
    string weightsPath = "C:/Users/vikassaran/Downloads/opencv1/ob/yolov4-tiny.weights";
    string configPath = "C:/Users/vikassaran/Downloads/opencv1/ob/yolov4-tiny.cfg";
    cout << "[INFO] loading YOLO from disk..." << endl;
    cv::dnn::Net net;
      // Read the Darknet model in C++
    auto result = cv::dnn::readNetFromDarknet(configPath, weightsPath);

    result.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    result.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    net=result;

    // Check if GPU is enabled
    if (USE_GPU)
    {
        // Set CUDA as the preferable backend and target
        cout << "[INFO] setting preferable backend and target to CUDA..." << endl;
        net.setPreferableBackend(DNN_BACKEND_CUDA);
        net.setPreferableTarget(DNN_TARGET_CUDA);
    }

    vector<string> ln = net.getLayerNames();
    // Uncomment the line below if you want to print all layers

    //for (int i = 0; i < ln.size(); i++)
    //{
    //    cerr <<i<<" " << ln[i] << endl;
    //}

    // Get unconnected output layers
    vector<int> unconnectedIndices = net.getUnconnectedOutLayers();
    
      //  cerr << "Updated ln:" << unconnectedIndices[1]  << endl;
    // Uncomment the line below if you want to print unconnected indices
    // cout << "Unconnected Out Layer Indices:" << unconnectedIndices << endl;

    // Update ln using unconnected indices
    vector<string> lnUpdated;
    for (int i = 0; i < unconnectedIndices.size(); i++)
    {
        int p = unconnectedIndices[i] - 1;
        //cerr << "Updated ln:" << ln[p]<< endl;
       // cout<<6<<endl;
        lnUpdated.push_back(ln[p]);
       // cerr << "Updated ln:" << lnUpdated[i] << endl;
    }

    // VideoCapture vs;
    // if (argv[1] == "")
    //  {
    //      cerr << "[INFO] Starting the live stream.." << endl;
    //    VideoCapture vs(0);
    //  if (Thread)
    //  {
    // doubt//   thread::ThreadingClass cap(config.url);
    // }
    // waitKey(200); // sleep for .2 seconds
    //}
    // otherwise, grab a reference to the video file
    // else
    // {
    cout << "[INFO] Starting the video.." << endl;
    VideoCapture vs("C:/Users/vikassaran/Downloads/hacathon/Problem_Statement_1/video.mp4");
    // if ()
    //  {
    //      // doubt//   thread::ThreadingClass cap(argv[1]);
    // }
    //}
    //VideoWriter writer();
  
    // Loop over the frames from the video stream
    int p=0;
     std::vector<std::tuple<float, cv::Rect, cv::Point>> results ;
    std::vector<std::tuple<float, cv::Rect, cv::Point>> lastfame ;
    while (true)
    {   
        p++;
        // Read the next frame from the file
        Mat frame;
        bool grabbed = vs.read(frame);
        
        // If the frame was not grabbed, we have reached the end of the stream
        if (!grabbed)
        {
            break;
        }

        // Resize the frame and then detect people (and only people) in it
        putText(frame, "people detector", Point(130, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0), 3);
       
        resize(frame, frame, Size(700, 700)); 
        // Assuming you want to resize to 700x700
       
        if(p%2==0){
        lastfame = detect_people(frame, net, lnUpdated, 0);
        results=lastfame;
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

        // Check to see if the output frame should be displayed on the screen

        // Show the output frame
        imshow("Real-Time Monitoring/Analysis Window", frame);
        int key = cv::waitKey(1) & 0xFF;
        // If the 'q' key was pressed, break from the loop
        if (key == 'q')
        {
            break;
        }

        // Update and display the FPS

        // If an output video file path has been supplied and the video writer has not been initialized, do so now
        // if (argv[1] == "" && writer == nullptr)
        // {
        // Initialize our video writer
        // librarynhi \h//int fourcc = cv::VideoWriter_fourcc('M', 'J', 'P', 'G');
        // cv::VideoWriter writer= VideoWriter(outputVideo, fourcc, 25, Size(frame.cols, frame.rows), true);
        // }

        // If the video writer is not nullptr, write the frame to the output video file
        // if (writer != nullptr)
        //{
        //   writer.write(frame);
        // }
    }

    // Stop the timer and display FPS information

    // Release the video writer
    // if (writer != nullptr)
    //{
    //    writer.release();
    //}

    // Close any open windows
    cv::destroyAllWindows();
}
