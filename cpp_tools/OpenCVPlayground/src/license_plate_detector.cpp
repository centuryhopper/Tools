#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <vector>

#include "license_plate_detector.hpp"

using namespace cv;
using namespace std;

LicensePlateDetector::LicensePlateDetector()
{
    plateCascade.load("../Resources/haarcascade_russian_plate_number.xml");
    if (plateCascade.empty())
    {
        std::runtime_error("Cascade Classifier Load Error");
    }    


}

void LicensePlateDetector::detectAndDisplay()
{
    vector<Rect> plates;
    VideoCapture cap(0);
    while (true)
    {
        cap.read(img);

        plateCascade.detectMultiScale(img, plates, 1.1, 10);

        for (int i = 0; i < plates.size(); i++)
        {
            Mat imgCrop = img(plates[i]);
            // imshow(to_string(i), imgCrop);
            // imwrite("../Outputs/plate_" + to_string(i) + ".png", imgCrop);
            rectangle(img, plates[i].tl(), plates[i].br(), Scalar(255, 0, 255), 3);
        }

        imshow("License Plate Detector", img);
        waitKey(1);
    }
}
