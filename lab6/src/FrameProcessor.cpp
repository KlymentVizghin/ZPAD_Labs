#include "FrameProcessor.hpp"

cv::Mat FrameProcessor::process(const cv::Mat& inputFrame, ProcessMode mode) {
    cv::Mat outputFrame;
    switch (mode) {
        case ProcessMode::INVERT:
            cv::bitwise_not(inputFrame, outputFrame);
            break;
        case ProcessMode::CANNY:
            cv::cvtColor(inputFrame, outputFrame, cv::COLOR_BGR2GRAY);
            cv::Canny(outputFrame, outputFrame, 50, 150);
            break;
        case ProcessMode::NORMAL:
        default:
            outputFrame = inputFrame.clone();
            break;
    }
    return outputFrame;
}
