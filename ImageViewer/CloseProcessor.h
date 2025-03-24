#ifndef CLOSEPROCESSOR_H
#define CLOSEPROCESSOR_H
#include "MorphologyProcessor.h"

// 闭运算
class CloseProcessor : public MorphologyProcessor {
public:
    using MorphologyProcessor::MorphologyProcessor;

    cv::Mat process(const cv::Mat &input) override {
        cv::Mat kernel = cv::getStructuringElement(kernelShape, cv::Size(kernelSize, kernelSize));
        cv::Mat output;
        cv::morphologyEx(input, output, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1), iterations);
        return output;
    }
};

#endif // CLOSEPROCESSOR_H
