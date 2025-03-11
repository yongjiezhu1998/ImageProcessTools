#include "imageviewer.h"
// #include "ui_imageviewer.h"
#include <QWheelEvent>

// ImageViewer.cpp
ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent) {
    // 创建图形场景和视图
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    setCentralWidget(view);

    // 启用抗锯齿
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 设置拖拽模式
    view->setDragMode(QGraphicsView::ScrollHandDrag);

    // 添加控制按钮
    QToolBar *toolBar = addToolBar("控制");
    toolBar->addAction("打开图片", this, &ImageViewer::openImage);
    toolBar->addAction("处理图片", this, &ImageViewer::processImage);
    toolBar->addAction("重置缩放", this, &ImageViewer::resetZoom);
}

void ImageViewer::openImage() {
    QString path = QFileDialog::getOpenFileName(
        this, "选择图片", "",
        "图片文件 (*.png *.jpg *.bmp)"
        );

    if (!path.isEmpty()) {
        originalMat = cv::imread(path.toStdString(), cv::IMREAD_COLOR);
        if (!originalMat.empty()) {
            cv::cvtColor(originalMat, originalMat, cv::COLOR_BGR2RGB);
            processedMat.release();
            isProcessed = false;
            updateDisplay();
        }
    }
}

void ImageViewer::updateDisplay() {
    scene->clear();

    const cv::Mat &displayMat = isProcessed ? processedMat : originalMat;
    if (displayMat.empty()) return;

    // 转换为QImage
    QImage qimg(
        displayMat.data,
        displayMat.cols,
        displayMat.rows,
        displayMat.step,
        displayMat.channels() == 1 ? QImage::Format_Grayscale8
                                   : QImage::Format_RGB888
        );

    // 创建可交互的图形项
    pixmapItem = scene->addPixmap(QPixmap::fromImage(qimg));
    scene->setSceneRect(pixmapItem->boundingRect());

    // 初始缩放适配窗口
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void ImageViewer::processImage() {
    if (originalMat.empty()) return;

    // 示例处理：Canny边缘检测
    cv::cvtColor(originalMat, processedMat, cv::COLOR_RGB2GRAY);
    cv::Canny(processedMat, processedMat, 100, 200);

    isProcessed = true;
    updateDisplay();
}

// 鼠标滚轮缩放
void ImageViewer::wheelEvent(QWheelEvent *event) {
    const double zoomFactor = 1.2;
    if (event->angleDelta().y() > 0) {
        view->scale(zoomFactor, zoomFactor);
    } else {
        view->scale(1/zoomFactor, 1/zoomFactor);
    }
}

// 重置缩放
void ImageViewer::resetZoom() {
    view->resetTransform();
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

// 双击恢复原始尺寸
void ImageViewer::mouseDoubleClickEvent(QMouseEvent*) {
    resetZoom();
}


