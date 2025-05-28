#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Create a black 400x400 image
    cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

    // Draw a green circle at the center
    cv::circle(image, cv::Point(200, 200), 100, cv::Scalar(0, 255, 0), -1);

    // Show the image
    cv::imshow("OpenCV Test - Green Circle", image);

    std::cout << "Press any key on the image window to exit..." << std::endl;
    cv::waitKey(0); // Wait for a key press

    return 0;
}

