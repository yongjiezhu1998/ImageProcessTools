#ifndef DILATIONPROCESSOR_H
#define DILATIONPROCESSOR_H

#include "MorphologyProcessor.h"
#include "QDebug"

// 膨胀
class DilationProcessor : public MorphologyProcessor {
public:
    using MorphologyProcessor::MorphologyProcessor;

    cv::Mat process(const cv::Mat &input) override {
        cv::Mat kernel = cv::getStructuringElement(kernelShape, cv::Size(kernelSize, kernelSize));
        cv::Mat output;
        cv::dilate(input, output, kernel, cv::Point(-1,-1), iterations);
        return output;
    }
};

#endif // DILATIONPROCESSOR_H
