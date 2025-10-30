// #include <fmt/core.h>
// #include <iostream>
// #include <opencv2/opencv.hpp>

// int main() {
//   cv::Mat image(200, 200, CV_8UC3, cv::Scalar(0, 255, 0));
//   cv::imshow("Hello OpenCV", image);
//   cv::waitKey(0);

//   int a = 1;

//   // --- fmt demo ---
//   fmt::print("a: {}\n", a);

//   return 0;
// }

#include <fmt/core.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <filesystem>

#include "virtual_painter.hpp"
#include <document_scanner.hpp>

using namespace std;
using namespace cv;

void showImage();
void showVideo();
void testCode();
void turnOnCamera();
void resizeAndCropImage();
void drawingShapesAndText();
void warpImages();
void colorDetection();
void shapesContourDetection();
void faceDetection();




int main()
{
  // std::cout << "hello there" << std::endl;
  // turnOnCamera();
  // showImage();
  // resizeAndCropImage();
  // showVideo();
  // drawingShapesAndText();
  // warpImages();
  // colorDetection();
  // faceDetection();
  // shapesContourDetection();

  // VirtualPainter vp;
  // vp.getColors();

  // DocumentScanner ds;
  // auto imgOriginal = ds.getImgOriginal();
  // ds.preProcess(imgOriginal);

  // // get contours (biggest)
  // auto imgThreshold = ds.getImgThreshold();
  // ds.setInitialPoints(ds.getContours(imgThreshold));
  // // ds.drawPoints(ds.getInitialPoints(), Scalar(0, 0, 255));
  // auto reorderedPoints = ds.reorderPoints();
  // ds.drawPoints(reorderedPoints, Scalar(0, 255, 0));

  // // warp
  // float w=250, h=350;
  // Mat imgWarp = ds.getWarp(imgOriginal, reorderedPoints, w, h);

  // // crop
  // Rect roi(5, 5, w-(2*5), h-(2*5));
  // Mat imgCrop = imgWarp(roi);

  // imshow("Original Image", ds.getImgOriginal());
  // imshow("Scanned Image", imgThreshold);
  // imshow("Image Warp", imgWarp);
  // imshow("Image Crop", imgCrop);
  // waitKey(0);


  return 0;
}


void faceDetection()
{
  string path = "../Resources/Me.jpeg";
  Mat img = imread(path), imgResize;

  if (img.empty())
  {
    cerr << "Error: Could not load image at " << path << endl;
    return;
  }

  resize(img, imgResize, Size(), 0.5, 0.5);

  CascadeClassifier faceCascade;
  faceCascade.load("../Resources/haarcascade_frontalface_default.xml");

  if (faceCascade.empty())
  {
    cout << "XML file not loaded" << endl;
    return;
  }

  vector<Rect> faces;
  faceCascade.detectMultiScale(imgResize, faces, 1.1, 10);

  for (int i = 0; i < faces.size(); i++)
  {
    rectangle(imgResize, faces[i].tl(), faces[i].br(), Scalar(255, 0, 255), 3);
  }

  imshow("Image", imgResize);
  waitKey(0);
}

void shapesContourDetection()
{
  auto getContours = [](Mat imgDil, Mat img)
  {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // drawContours(img, contours, -1, Scalar(255, 0, 255), 2);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    for (int i = 0; i < contours.size(); i++)
    {
      int area = contourArea(contours[i]);
      cout << area << endl;
      string objectType;

      if (area > 1000)
      {
        float peri = arcLength(contours[i], true);
        approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
        // cout << conPoly[i].size() << endl;
        boundRect[i] = boundingRect(conPoly[i]);

        int objCor = (int)conPoly[i].size();

        if (objCor == 3)
        {
          objectType = "Tri";
        }
        else if (objCor == 4)
        {
          float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
          // cout << aspRatio << endl;
          if (aspRatio > 0.95 && aspRatio < 1.05)
          {
            objectType = "Square";
          }
          else
          {
            objectType = "Rect";
          }
        }
        else if (objCor > 4)
        {
          objectType = "Circle";
        }

        drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
        rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
        putText(img, objectType, {boundRect[i].x, boundRect[i].y - 5}, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 2);
      }
    }
  };

  string path = "../Resources/shapes.png";
  Mat img = imread(path);
  if (img.empty())
  {
    cerr << "Error: Could not load image at " << path << endl;
    return;
  }
  Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

  // Preprocessing
  cvtColor(img, imgGray, COLOR_BGR2GRAY);
  GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
  Canny(imgBlur, imgCanny, 25, 75);
  Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
  dilate(imgCanny, imgDil, kernel);

  getContours(imgDil, img);

  imshow("Image", img);
  // imshow("Image Gray", imgGray);
  // imshow("Image Blur", imgBlur);
  // imshow("Image Canny", imgCanny);
  // imshow("Image Dil", imgDil);

  waitKey(0);
}

void colorDetection()
{
  string path = "../Resources/lambo.png";
  string trackBarWindowName = "Trackbars";
  Mat img = imread(path);
  Mat imgHSV, mask;
  int hmin = 0, smin = 110, vmin = 153;
  int hmax = 19, smax = 240, vmax = 255;

  // convert image to hsv space
  cvtColor(img, imgHSV, COLOR_BGR2HSV);

  namedWindow(trackBarWindowName, (640, 200));
  createTrackbar("Hue Min", trackBarWindowName, &hmin, 179);
  createTrackbar("Hue Max", trackBarWindowName, &hmax, 179);
  createTrackbar("Sat Min", trackBarWindowName, &smin, 255);
  createTrackbar("Sat Max", trackBarWindowName, &smax, 255);
  createTrackbar("Val Min", trackBarWindowName, &vmin, 255);
  createTrackbar("Val Max", trackBarWindowName, &vmax, 255);

  while (true)
  {
    Scalar lower(hmin, smin, vmin);
    Scalar upper(hmax, smax, vmax);
    inRange(imgHSV, lower, upper, mask);

    imshow("Image", img);
    imshow("Image HSV", imgHSV);
    imshow("Image Mask", mask);
    waitKey(1);
  }
}

void drawingShapesAndText()
{
  Mat img(512, 512, CV_8UC3, Scalar(255, 255, 255));
  circle(img, Point(256, 256), 155, Scalar(0, 69, 255), FILLED);
  rectangle(img, Point(130, 226), Point(382, 286), Scalar(255, 255, 255), FILLED);
  line(img, Point(130, 296), Point(382, 296), Scalar(255, 255, 255), 2);

  putText(img, "Welcome to Leo's OpenCV Playground!", Point(137, 262), FONT_HERSHEY_DUPLEX, 0.40, Scalar(0, 69, 255));

  imshow("Image", img);
  waitKey(0);
}

void warpImages()
{
  string path = "../Resources/cards.jpg";
  Mat img = imread(path);
  float w = 250, h = 350;
  Point2f src[4] = {
      {529, 142},
      {771, 190},
      {405, 395},
      {674, 457},
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

  // drawing after warping to prevent warped image from having these points
  for (int i = 0; i < 4; i++)
  {
    circle(img, src[i], 10, Scalar(0, 0, 255), FILLED);
  }

  imshow("Image", img);
  imshow("Image Warp", imgWarp);
  waitKey(0);
}

void resizeAndCropImage()
{
  string path = "../Resources/Me.jpeg";
  Mat img = imread(path);
  Mat imgResize, imgCrop;

  std::cout << "Image size: " << img.size() << std::endl;

  // resize
  // doesn't maintain aspect ratio
  // resize(img, imgResize, Size(200, 200));

  // resize image to 50% smaller (maintaining aspect ratio)
  resize(img, imgResize, Size(), 0.5, 0.5);

  // crop
  // roi = region of interest
  Rect roi(120, 185, 290, 425);
  imgCrop = imgResize(roi);

  // imshow("Image", img);
  // imshow("Image Resize", imgResize);
  imshow("Image Crop", imgCrop);
  waitKey(0);
}

void showImage()
{
  string path = "../Resources/Me.jpeg";
  Mat img = imread(path);
  // fmt::print("path: {}\n", std::filesystem::current_path());
  // std::cout << "Current path: " << std::filesystem::current_path() << std::endl;

  if (img.empty())
  {
    cerr << "Error: Could not load image at " << path << endl;
    return;
  }

  Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

  // basic functions
  cvtColor(img, imgGray, COLOR_BGR2GRAY);
  GaussianBlur(img, imgBlur, Size(7, 7), 3, 0);
  Canny(imgBlur, imgCanny, 25, 75);

  Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
  dilate(imgCanny, imgDil, kernel);
  erode(imgDil, imgErode, kernel);

  imshow("Original", img);
  imshow("Grayscale", imgGray);
  imshow("Blur", imgBlur);
  imshow("Canny", imgCanny);
  imshow("Dilation", imgDil);
  imshow("Erode", imgErode);
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


