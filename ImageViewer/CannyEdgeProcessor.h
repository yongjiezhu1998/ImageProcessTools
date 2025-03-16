#ifndef CANNYEDGEPROCESSOR_H
#define CANNYEDGEPROCESSOR_H

#include "ImageProcessor.h"

class CannyEdgeProcessor : public ImageProcessor {
public:
    CannyEdgeProcessor(int lowThreshold = 50, int highThreshold = 150)
        : lowThreshold(lowThreshold), highThreshold(highThreshold) {}

    cv::Mat process(const cv::Mat &input) override {
        cv::Mat gray, edges;
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, edges, lowThreshold, highThreshold);
        return edges;
    }

private:
    int lowThreshold;
    int highThreshold;
};

#endif // CANNYEDGEPROCESSOR_H
