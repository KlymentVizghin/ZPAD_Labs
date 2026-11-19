#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "FaceDetector.hpp"
#include "Display.hpp"

int main(int argc, char* argv[]) {
    std::unique_ptr<CameraProvider> camera;
    if (argc > 1) {
        camera = std::make_unique<CameraProvider>(argv[1]);
    } else {
        camera = std::make_unique<CameraProvider>(0);
    }

    if (!camera->isOpened()) {
        std::cerr << "Error: Could not open video source!" << std::endl;
        return -1;
    }

    FaceDetector faceDetector("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");
    if (!faceDetector.isLoaded()) {
        std::cerr << "Warning: Face detector model failed to load!" << std::endl;
    }

    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    Display display("Lab 7 - OpenCV DNN & Multithreading");

    std::cout << "Controls:\n"
              << "  1     - Normal\n"
              << "  2     - Invert\n"
              << "  3     - Canny Edge\n"
              << "  f     - Face Detection (ResNet-10 SSD)\n"
              << "  q/ESC - Quit\n";

    while (true) {
        cv::Mat frame = camera->getFrame();
        if (frame.empty()) break;

        ProcessMode mode = keyProcessor.getCurrentMode();
        std::vector<FaceBox> faces;

        if (mode == ProcessMode::FACE && faceDetector.isLoaded()) {
            faceDetector.submitFrame(frame);
            faces = faceDetector.getDetections();
        }

        cv::Mat processedFrame = frameProcessor.process(frame, mode, faces);
        display.show(processedFrame);

        int key = cv::waitKey(30);
        keyProcessor.handleKey(key);

        if (keyProcessor.getCurrentMode() == ProcessMode::EXIT) {
            break;
        }
    }

    faceDetector.stop();
    return 0;
}
