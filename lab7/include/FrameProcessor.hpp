#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "KeyProcessor.hpp"
#include "FaceDetector.hpp"

class FrameProcessor {
public:
    cv::Mat process(const cv::Mat& inputFrame, ProcessMode mode, const std::vector<FaceBox>& faces = {});
};
