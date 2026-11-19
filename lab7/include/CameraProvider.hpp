#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class CameraProvider {
private:
    cv::VideoCapture cap;
    bool isFile;
public:
    CameraProvider(int cameraId = 0);
    CameraProvider(const std::string& videoPath);
    ~CameraProvider();
    cv::Mat getFrame();
    bool isOpened() const;
};
