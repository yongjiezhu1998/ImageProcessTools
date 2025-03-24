#ifndef OPENPROCESSOR_H
#define OPENPROCESSOR_H
#include "MorphologyProcessor.h"
// 开运算
class OpenProcessor : public MorphologyProcessor {
public:
    using MorphologyProcessor::MorphologyProcessor;

    cv::Mat process(const cv::Mat &input) override {
        cv::Mat kernel = cv::getStructuringElement(kernelShape, cv::Size(kernelSize, kernelSize));
        cv::Mat output;
        cv::morphologyEx(input, output, cv::MORPH_OPEN, kernel, cv::Point(-1,-1), iterations);
        return output;
    }
};

#endif // OPENPROCESSOR_H
