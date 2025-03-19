#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
#include "imageview.h"
// ImageViewer.h
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBar>
#include <QGraphicsPixmapItem>
#include <opencv2/opencv.hpp>
#include <QFileDialog>
#include <QToolBox>
#include <QDoubleSpinBox>
#include <QComboBox>

class ImageViewer : public QMainWindow {
    Q_OBJECT
public:
    ImageViewer(QWidget *parent = nullptr);

public slots:
    void openImage();           // 打开图片
    void saveImage();           // 保存图片
    void processImage();        // 处理图片（示例：灰度化）
    void resetZoom();           // 重置缩放
    void resetProcess();        // 重置操作

    void onCannyEdgeTriggered();
    void onGrayscaleTriggered();
    void onGaussianBlurTriggered();
    void onResetProcess();
    void onBlurTriggered();



private:
    void updateDisplay();      // 更新显示

    QToolBox *toolBox;       // 新增：左侧工具箱
    QGraphicsScene *scene;
    ImageView *view; // 自定义视图
    // QGraphicsView *view;
    QGraphicsPixmapItem *pixmapItem;
    QDoubleSpinBox *sigmaXSpin;
    QDoubleSpinBox *sigmaYSpin;
    QComboBox *kernelSizeCombo;


    cv::Mat originalMat;       // OpenCV原始图像
    cv::Mat processedMat;      // 处理后的图像
    bool isProcessed = false;  // 当前显示状态
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);

    // bool event(QEvent *e);
    void onPixelHovered(int x, int y, const QColor &color);
    void applyProcessor(const std::string &processorName);
    QWidget* gaussianBlurBoxLayout();
    QWidget* blurBoxLayout();
};
#endif // IMAGEVIEWER_H
