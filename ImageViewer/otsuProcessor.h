#ifndef OTSUPROCESSOR_H
#define OTSUPROCESSOR_H

#include "ImageProcessor.h"

class OtsuProcessor : public ImageProcessor {
public:
    // 新增：参数化构造函数
    OtsuProcessor()
    {}

    cv::Mat process(const cv::Mat &input) override {
        if (input.empty()) {
            throw std::invalid_argument("输入图像为空");
        }

        // 确保输入为单通道灰度图
        cv::Mat gray;
        if (input.channels() > 1) {
            cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = input.clone();
        }

        cv::Mat output;
        cv::threshold(gray, output, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        return output;
    }
};

#endif // OTSUPROCESSOR_H
