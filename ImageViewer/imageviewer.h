#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

// ImageViewer.h
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBar>
#include <QGraphicsPixmapItem>
#include <opencv2/opencv.hpp>
#include <QFileDialog>

class ImageViewer : public QMainWindow {
    Q_OBJECT
public:
    ImageViewer(QWidget *parent = nullptr);

public slots:
    void openImage();          // 打开图片
    void processImage();       // 处理图片（示例：灰度化）
    void resetZoom();          // 重置缩放

private:
    void updateDisplay();      // 更新显示

    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsPixmapItem *pixmapItem;

    cv::Mat originalMat;       // OpenCV原始图像
    cv::Mat processedMat;      // 处理后的图像
    bool isProcessed = false;  // 当前显示状态
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);
};
#endif // IMAGEVIEWER_H
