#include "FaceDetector.hpp"
#include <iostream>

FaceDetector::FaceDetector(const std::string& prototxtPath, const std::string& modelPath)
    : modelLoaded(false), running(false), hasNewFrame(false) {
    try {
        net = cv::dnn::readNetFromCaffe(prototxtPath, modelPath);
        if (net.empty()) {
            std::cerr << "Error: Loaded neural network is empty!" << std::endl;
            return;
        }
        modelLoaded = true;
        running = true;
        workerThread = std::thread(&FaceDetector::workerLoop, this);
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV DNN Error: " << e.what() << std::endl;
        std::cerr << "Make sure you ran ./preinstall.sh to download model files." << std::endl;
    }
}

FaceDetector::~FaceDetector() {
    stop();
}

void FaceDetector::stop() {
    if (running) {
        {
            std::lock_guard<std::mutex> lock(inputMutex);
            running = false;
        }
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
}

bool FaceDetector::isLoaded() const {
    return modelLoaded;
}

void FaceDetector::submitFrame(const cv::Mat& frame) {
    if (!modelLoaded || frame.empty()) return;
    {
        std::lock_guard<std::mutex> lock(inputMutex);
        latestInputFrame = frame.clone();
        hasNewFrame = true;
    }
    cv.notify_one();
}

std::vector<FaceBox> FaceDetector::getDetections() {
    std::lock_guard<std::mutex> lock(resultMutex);
    return latestDetections;
}

void FaceDetector::workerLoop() {
    while (true) {
        cv::Mat frameCopy;
        {
            std::unique_lock<std::mutex> lock(inputMutex);
            cv.wait(lock, [this]() { return !running || hasNewFrame; });
            if (!running) break;

            frameCopy = latestInputFrame.clone();
            hasNewFrame = false;
        }

        if (frameCopy.empty()) continue;

        // Підготовка blob 300x300 для ResNet-10 SSD
        cv::Mat blob = cv::dnn::blobFromImage(
            frameCopy, 1.0, cv::Size(300, 300),
            cv::Scalar(104.0, 177.0, 123.0), false, false
        );
        net.setInput(blob);
        cv::Mat detections = net.forward();

        // Розбір результатів (матриця N x 7)
        std::vector<FaceBox> newBoxes;
        cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

        for (int i = 0; i < detectionMat.rows; ++i) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > 0.5f) {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameCopy.cols);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameCopy.rows);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameCopy.cols);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameCopy.rows);

                cv::Rect box(cv::Point(x1, y1), cv::Point(x2, y2));
                // Обмежуємо рамку межами кадру
                box &= cv::Rect(0, 0, frameCopy.cols, frameCopy.rows);

                if (box.area() > 0) {
                    newBoxes.push_back({box, confidence});
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(resultMutex);
            latestDetections = std::move(newBoxes);
        }
    }
}
