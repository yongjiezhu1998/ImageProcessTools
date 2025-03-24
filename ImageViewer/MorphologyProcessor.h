#ifndef MORPHOLOGYPROCESSOR_H
#define MORPHOLOGYPROCESSOR_H

#include "ImageProcessor.h"


class MorphologyProcessor : public ImageProcessor {
public:
    MorphologyProcessor(int op, int shape, int ksize, int iterations)
        : operation(op), kernelShape(shape), kernelSize(ksize), iterations(iterations)
    {
        // 确保核大小为奇数
        if (ksize % 2 == 0) {
            throw std::invalid_argument("Kernel size must be odd");
        }
    }

protected:
    int operation;      // 操作类型 (cv::MORPH_DILATE等)
    int kernelShape;    // 结构元素形状 (cv::MORPH_RECT等)
    int kernelSize;     // 结构元素大小
    int iterations;     // 迭代次数
};


#endif // MORPHOLOGYPROCESSOR_H
