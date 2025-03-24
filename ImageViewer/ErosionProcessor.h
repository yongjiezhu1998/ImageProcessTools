#ifndef EROSIONPROCESSOR_H
#define EROSIONPROCESSOR_H

#include "MorphologyProcessor.h"

// 腐蚀
class ErosionProcessor : public MorphologyProcessor {
public:
    using MorphologyProcessor::MorphologyProcessor;

    cv::Mat process(const cv::Mat &input) override {
        cv::Mat kernel = cv::getStructuringElement(kernelShape, cv::Size(kernelSize, kernelSize));
        cv::Mat output;
        cv::erode(input, output, kernel, cv::Point(-1,-1), iterations);
        return output;
    }
};

#endif // EROSIONPROCESSOR_H
