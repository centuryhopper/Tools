#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include "virtual_painter.hpp"

using namespace cv;
using namespace std;


VirtualPainter::VirtualPainter()
{
}

void VirtualPainter::getColors()
{
  VideoCapture cap(0);
  // cv::VideoCapture cap(0, cv::CAP_V4L2); // or CAP_FFMPEG, CAP_ANY

  if (!cap.isOpened())
  {
    std::cerr << "❌ Failed to open camera!" << std::endl;
    return;
  }

  cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

  string trackBarWindowName = "Trackbars";
  cv::Mat img;
  cv::Mat imgHSV, mask;

  // namedWindow(trackBarWindowName, (640, 200));
  // createTrackbar("Hue Min", trackBarWindowName, &hmin, 179);
  // createTrackbar("Hue Max", trackBarWindowName, &hmax, 179);
  // createTrackbar("Sat Min", trackBarWindowName, &smin, 255);
  // createTrackbar("Sat Max", trackBarWindowName, &smax, 255);
  // createTrackbar("Val Min", trackBarWindowName, &vmin, 255);
  // createTrackbar("Val Max", trackBarWindowName, &vmax, 255);

  while (true)
  {
    cap.read(img);

    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    Scalar lower(hmin, smin, vmin);
    Scalar upper(hmax, smax, vmax);
    inRange(imgHSV, lower, upper, mask);

    // imshow("color values debugger", mask);

    findColors(img, imgHSV, mask);
    drawOnCanvas(img);

    imshow("Virtual Painter", img);
    
    // vpGetContours(mask, img);
    waitKey(1);
  }
}

cv::Point VirtualPainter::vpGetContours(const Mat &mask, Mat &img)
{
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  // drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

  vector<vector<Point>> conPoly(contours.size());
  vector<Rect> boundRect(contours.size());

  Point myPoint(0, 0);

  for (int i = 0; i < contours.size(); i++)
  {
    int area = contourArea(contours[i]);
    // cout << area << endl;

    if (area > 1000)
    {
      float peri = arcLength(contours[i], true);
      approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
      // cout << conPoly[i].size() << endl;
      boundRect[i] = boundingRect(conPoly[i]);

      // draw from the center along the horizontal (x) axis
      myPoint.x = boundRect[i].x + boundRect[i].width / 2;
      myPoint.y = boundRect[i].y;
      

      drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
      rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
    }
  }

  return myPoint;

  // imshow("Contours", img);
}

void VirtualPainter::drawOnCanvas(
  cv::Mat img)
{
  for (auto const &[color, points] : newPoints)
  {
    for (auto const &point : points)
    {
      // ->second is the value
      circle(img, Point(point.first, point.second), 10, (myColorValues.find(color))->second, FILLED);
    }
  }
  // for (int i = 0; i < newPoints.size(); i++)
  // {
  //   circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
  // }
}

void VirtualPainter::findColors(cv::Mat &img, cv::Mat &imgHSV, cv::Mat &mask)
{
  int idx = 0;
  for (auto const &[color, values] : colors)
  {
    hmin = values[0];
    hmax = values[1];
    smin = values[2];
    smax = values[3];
    vmin = values[4];
    vmax = values[5];

    // convert image to hsv space
    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    Scalar lower(hmin, smin, vmin);
    Scalar upper(hmax, smax, vmax);

    Mat colorMask;
    // generate mask for this specific color
    inRange(imgHSV, lower, upper, colorMask);
    // merge with the global mask so all colors show up
    bitwise_or(mask, colorMask, mask);

    Point myPoint = vpGetContours(colorMask, img);

    if (myPoint.x != 0 && myPoint.y != 0)
    {
      newPoints[color].push_back({myPoint.x, myPoint.y});
    }

    // fmt::print("hue min: {}, hue max: {}, sat min: {}, sat max: {}, val min: {}, val max: {}\n",
    // hmin, hmax, smin, smax, vmin, vmax);

    // imshow("Image", img);
    // imshow("Image HSV", imgHSV);
    // imshow(std::to_string(idx++), mask);
  }
}
