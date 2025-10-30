
#pragma once
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

class DocumentScanner
{
private:
    cv::Mat imgOriginal, imgThreshold;
    std::vector<cv::Point> initialPoints;
public:
    DocumentScanner();
    cv::Mat getImgOriginal() const { return imgOriginal; }
    void setImgOriginal(cv::Mat i) { imgOriginal = i; }
    void setImgThreshold(cv::Mat i) { imgThreshold = i; }
    cv::Mat getImgThreshold() const { return imgThreshold; }

    void setInitialPoints(std::vector<cv::Point> pts) { initialPoints = pts; }
    std::vector<cv::Point> getInitialPoints() const { return initialPoints; }

    void preProcess(const cv::Mat &img);
    std::vector<cv::Point> getContours(cv::Mat &img);
    void drawPoints(std::vector<cv::Point> points, cv::Scalar color);
    std::vector<cv::Point> reorderPoints();
    cv::Mat getWarp(cv::Mat &img, std::vector<cv::Point> points, float w, float h);
};
