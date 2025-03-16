#ifndef RESETPROCESS_H
#define RESETPROCESS_H
#include "ImageProcessor.h"

class ResetProcess : public ImageProcessor {
public:
    cv::Mat process(const cv::Mat &input) override {
        // cv::Mat input;
        // cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        return input;
    }
};

#endif // RESETPROCESS_H
