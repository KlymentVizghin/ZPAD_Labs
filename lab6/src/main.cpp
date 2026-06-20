#include <opencv2/opencv.hpp>
#include <iostream>
#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Error: Could not open camera!" << std::endl;
        return -1;
    }

    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    Display display("Lab 6 - OpenCV Video Processing");

    std::cout << "Controls:\n 1 - Normal\n 2 - Invert\n 3 - Canny Edge\n q / ESC - Quit\n";

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;

        cv::Mat processedFrame = frameProcessor.process(frame, keyProcessor.getCurrentMode());
        display.show(processedFrame);

        int key = cv::waitKey(30);
        keyProcessor.handleKey(key);

        if (keyProcessor.getCurrentMode() == ProcessMode::EXIT) {
            break;
        }
    }

    return 0;
}
