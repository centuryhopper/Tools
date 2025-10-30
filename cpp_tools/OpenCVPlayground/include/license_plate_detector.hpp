
#pragma once
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

class LicensePlateDetector
{
private:
    cv::CascadeClassifier plateCascade;
    cv::Mat img;
public:
    LicensePlateDetector();
    void detectAndDisplay();
};
