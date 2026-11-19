#include "FrameProcessor.hpp"
#include <string>
#include <iomanip>
#include <sstream>

cv::Mat FrameProcessor::process(const cv::Mat& inputFrame, ProcessMode mode, const std::vector<FaceBox>& faces) {
    cv::Mat outputFrame;
    switch (mode) {
        case ProcessMode::INVERT:
            cv::bitwise_not(inputFrame, outputFrame);
            break;
        case ProcessMode::CANNY:
            cv::cvtColor(inputFrame, outputFrame, cv::COLOR_BGR2GRAY);
            cv::Canny(outputFrame, outputFrame, 50, 150);
            break;
        case ProcessMode::FACE:
            outputFrame = inputFrame.clone();
            for (const auto& face : faces) {
                cv::rectangle(outputFrame, face.rect, cv::Scalar(0, 255, 0), 2);

                std::ostringstream ss;
                ss << "Face: " << std::fixed << std::setprecision(1) << (face.confidence * 100.0f) << "%";
                int textY = std::max(face.rect.y - 10, 20);
                cv::putText(outputFrame, ss.str(), cv::Point(face.rect.x, textY),
                            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
            }
            break;
        case ProcessMode::NORMAL:
        default:
            outputFrame = inputFrame.clone();
            break;
    }
    return outputFrame;
}
