#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

struct FaceBox {
    cv::Rect rect;
    float confidence;
};

class FaceDetector {
private:
    cv::dnn::Net net;
    bool modelLoaded;

    std::thread workerThread;
    std::atomic<bool> running;

    std::mutex inputMutex;
    std::condition_variable cv;
    cv::Mat latestInputFrame;
    bool hasNewFrame;

    std::mutex resultMutex;
    std::vector<FaceBox> latestDetections;

    void workerLoop();

public:
    FaceDetector(const std::string& prototxtPath, const std::string& modelPath);
    ~FaceDetector();

    bool isLoaded() const;
    void submitFrame(const cv::Mat& frame);
    std::vector<FaceBox> getDetections();
    void stop();
};
