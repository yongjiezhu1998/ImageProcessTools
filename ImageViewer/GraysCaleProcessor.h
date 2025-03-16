#ifndef GRAYSCALEPROCESSOR_H
#define GRAYSCALEPROCESSOR_H

#include "ImageProcessor.h"

class GrayscaleProcessor : public ImageProcessor {
public:
    cv::Mat process(const cv::Mat &input) override {
        cv::Mat output;
        cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        return output;
    }
};

#endif // GRAYSCALEPROCESSOR_H
