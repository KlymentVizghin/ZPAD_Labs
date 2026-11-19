#include "CameraProvider.hpp"

CameraProvider::CameraProvider(int cameraId) : isFile(false) {
    cap.open(cameraId);
}

CameraProvider::CameraProvider(const std::string& videoPath) : isFile(true) {
    cap.open(videoPath);
}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) cap.release();
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    cap >> frame;
    // Якщо відеофайл закінчився, перемотуємо на початок
    if (frame.empty() && isFile && cap.isOpened()) {
        cap.set(cv::CAP_PROP_POS_FRAMES, 0);
        cap >> frame;
    }
    return frame;
}

bool CameraProvider::isOpened() const {
    return cap.isOpened();
}
