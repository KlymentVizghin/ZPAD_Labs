#pragma once
#include <opencv2/opencv.hpp>

class CameraProvider {
private:
    cv::VideoCapture cap;
public:
    CameraProvider(int cameraId = 0);
    ~CameraProvider();
    cv::Mat getFrame();
    bool isOpened() const;
};
