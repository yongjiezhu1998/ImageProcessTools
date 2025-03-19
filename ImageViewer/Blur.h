#ifndef BLUR_H
#define BLUR_H

#include "ImageProcessor.h"


#include "ImageProcessor.h"
#include <opencv2/opencv.hpp>

class Blur : public ImageProcessor {
public:
    // 新增：参数化构造函数
    Blur(int kernelSize)
        : kernelSize(kernelSize) {

    }

    cv::Mat process(const cv::Mat &input) override {
        cv::Mat output;
        cv::blur(
            input, output,
            cv::Size(kernelSize, kernelSize) // 核大小（宽高相同）                   // X/Y方向标准差
            );
        return output;
    }

private:
    int kernelSize;   // 核大小（正奇数）
};

#endif // BLUR_H
