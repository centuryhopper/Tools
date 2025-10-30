#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <vector>

#include "document_scanner.hpp"

using namespace cv;
using namespace std;

vector<Point> DocumentScanner::getContours(Mat &img)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    vector<Point> biggest;
    int maxArea = 0;

    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);
        // cout << area << endl;

        if (area > 1000)
        {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            if (conPoly[i].size() == 4)
            {
                maxArea = max(area, maxArea);
                biggest = {conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3]};
            }

            drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
            //   rectangle(imgOriginal, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
        }
    }

    return biggest;

    // imshow("Contours", img);
}

void DocumentScanner::drawPoints(std::vector<cv::Point> points, cv::Scalar color)
{
    for (int i = 0; i < points.size(); i++)
    {
        circle(imgOriginal, points[i], 10, color, FILLED);
        putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
    }
}

/*
    sumPoints:
    (23,45) -> 68
    (400,30) -> 430
    (350,400) -> 750
    (25,300) -> 325

    diffPoints:
    (23,45) -> -22
    (400,30) -> 370
    (350,400) -> -50
    (25,300) -> -275

*/
std::vector<cv::Point> DocumentScanner::reorderPoints()
{
    // create a new vector to hold the reordered points
    vector<Point> newPoints(4);
    // create a vector of all the summations and differences
    vector<int> sumPoints, diffPoints;
    for (int i = 0; i < 4; i++)
    {
        sumPoints.push_back(initialPoints[i].x + initialPoints[i].y);
        diffPoints.push_back(initialPoints[i].x - initialPoints[i].y);
    }

    // top-left point has the smallest sum
    newPoints[0] = initialPoints[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()];
    // bottom-left point has the largest difference
    newPoints[1] = initialPoints[max_element(diffPoints.begin(), diffPoints.end()) - diffPoints.begin()];
    // top-right point has the smallest difference
    newPoints[2] = initialPoints[min_element(diffPoints.begin(), diffPoints.end()) - diffPoints.begin()];
    // bottom-right point has the largest sum
    newPoints[3] = initialPoints[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()];

    return newPoints;
}

cv::Mat DocumentScanner::getWarp(cv::Mat &img, std::vector<cv::Point> points, float w, float h)
{
    Point2f src[4] = {
        points[0],
        points[1],
        points[2],
        points[3],
    };
    Point2f dst[4] = {
        {0.0f, 0.0f},
        {w, 0.0f},
        {0.0f, h},
        {w, h},
    };
    Mat matrix = getPerspectiveTransform(src, dst);
    Mat imgWarp;
    warpPerspective(img, imgWarp, matrix, Point(w, h));

    return imgWarp;
}

DocumentScanner::DocumentScanner()
{
    // cout << "DocumentScanner initialized." << endl;
    imgOriginal = imread("../Resources/paper.jpg");
    if (imgOriginal.empty())
    {
        throw std::runtime_error("Failed to load image ../Resources/paper.jpg");
    }
    // resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5);
}

void DocumentScanner::preProcess(const cv::Mat &img)
{
    cv::Mat imgGray, imgBlur, imgCanny, imgDil;

    // convert to grayscale
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    // apply Gaussian blur to reduce noise and improve edge detection
    GaussianBlur(imgGray, imgBlur, Size(3, 3), 0);
    // perform Canny edge detection
    Canny(imgBlur, imgCanny, 25, 75);
    // dilate the edges to strengthen them
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imgCanny, imgDil, kernel);

    this->imgThreshold = imgDil;
}