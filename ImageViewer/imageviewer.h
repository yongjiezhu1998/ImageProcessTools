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

class ImageViewer : public QMainWindow {
    Q_OBJECT
public:
    ImageViewer(QWidget *parent = nullptr);

public slots:
    void openImage();           // 打开图片
    void saveImage();           // 保存图片
    void processImage();        // 处理图片（示例：灰度化）
    void resetZoom();           // 重置缩放
    void applyGrayscale();      // 新增：灰度化工具
    void applyCannyEdge();      // 新增：边缘检测工具

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void updateDisplay();      // 更新显示

    QToolBox *toolBox;       // 新增：左侧工具箱
    QGraphicsScene *scene;
    ImageView *view; // 自定义视图
    // QGraphicsView *view;
    QGraphicsPixmapItem *pixmapItem;


    cv::Mat originalMat;       // OpenCV原始图像
    cv::Mat processedMat;      // 处理后的图像
    bool isProcessed = false;  // 当前显示状态
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);

    // bool event(QEvent *e);
    void onPixelHovered(int x, int y, const QColor &color);
};
#endif // IMAGEVIEWER_H
