
#pragma once
#include <vector>
#include <string>

class VirtualPainter
{
private:
    int hmin = 0, smin = 0, vmin = 0;
    int hmax = 255, smax = 255, vmax = 255;
    std::unordered_map<std::string, std::vector<std::pair<int, int>>> newPoints;
    std::unordered_map<std::string, cv::Scalar> myColorValues = {
        {"purple", cv::Scalar(255, 0, 255)}, // Purple
        {"green", cv::Scalar(0, 255, 0)}      // Green
    };

public:
    std::unordered_map<std::string, std::vector<int>> colors = {
        {"purple", {124, 143, 48, 170, 117, 255}},
        // {"orange", {4,25,60,255,100,255}},
        {"green", {35, 85, 50, 255, 100, 255}},
        // TODO: get the other color values down
        // {},
        // {},
        // {},
        // {},
        // {},
    };
    VirtualPainter();
    void drawOnCanvas(cv::Mat img);
    void getColors();
    void findColors(cv::Mat &img, cv::Mat &imgHSV, cv::Mat &mask);
    cv::Point vpGetContours(const cv::Mat &mask, cv::Mat &img);
};