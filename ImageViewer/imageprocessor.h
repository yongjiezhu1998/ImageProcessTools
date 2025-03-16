#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <QObject>

class ImageProcessor : public QObject {
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ImageProcessor() = default;

    // 统一处理接口
    virtual cv::Mat process(const cv::Mat &input) = 0;

signals:
    // 处理完成信号（可选，用于异步操作）
    void processed(const cv::Mat &result);
};

#endif // IMAGEPROCESSOR_H
