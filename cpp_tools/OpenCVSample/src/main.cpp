#include <fmt/core.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

void showImage();
void showVideo();
void testCode();
void turnOnCamera();

int main()
{
  // turnOnCamera();
  // showVideo();
  return 0;
}

void showImage()
{
  string path = "Resources/test.png";
  Mat img = imread(path);
  imshow("Image", img);
  waitKey(0);
}

void showVideo()
{
  string path = "../Resources/test_video.mp4";
  VideoCapture cap(path);
  // cv::VideoCapture cap(path, cv::CAP_FFMPEG); 
  Mat img;
  while (true)
  {
    cap.read(img);
    imshow("Video", img);
    waitKey(20);
  }
}

void turnOnCamera()
{
  VideoCapture cap(0);
  Mat img;
  while (true)
  {
    cap.read(img);
    imshow("Camera", img);
    waitKey(1);
  }
}



void testCode()
{
  // Create a black 400x400 image
  cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

  // Draw a green circle at the center
  cv::circle(image, cv::Point(200, 200), 100, cv::Scalar(0, 255, 0), -1);

  // Show the image
  cv::imshow("OpenCV Test - Green Circle", image);

  std::cout << "Press any key on the image window to exit..." << std::endl;
  cv::waitKey(0); // Wait for a key press

  fmt::print("Hello from fmt + OpenCV!\n");
}