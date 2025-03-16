#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "ImageProcessor.h"


#include "ImageProcessor.h"
#include <opencv2/opencv.hpp>

class GaussianBlur : public ImageProcessor {
public:
    // 新增：参数化构造函数
    GaussianBlur(int kernelSize, double sigmaX, double sigmaY = 0)
        : kernelSize(kernelSize), sigmaX(sigmaX), sigmaY(sigmaY) {
        // 若sigmaY未指定，默认与sigmaX相同（OpenCV标准行为）
        if (sigmaY == 0) this->sigmaY = sigmaX;
    }

    cv::Mat process(const cv::Mat &input) override {
        cv::Mat output;
        cv::GaussianBlur(
            input, output,
            cv::Size(kernelSize, kernelSize), // 核大小（宽高相同）
            sigmaX, sigmaY                    // X/Y方向标准差
            );
        return output;
    }

private:
    int kernelSize;   // 核大小（正奇数）
    double sigmaX;    // X方向标准差
    double sigmaY;    // Y方向标准差（默认等于sigmaX）
};
#endif // GAUSSIANBLUR_H
