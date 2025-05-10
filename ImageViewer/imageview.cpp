#include "imageview.h"
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QDebug>
#include <EditableTextItem.h>
#include <QlineEdit>
#include <Qvalidator>

ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    // 初始化场景和视图属性
    QGraphicsScene *scene = new QGraphicsScene(this);
    setScene(scene);
    // 禁用拖拽模式并启用鼠标追踪
    setDragMode(QGraphicsView::NoDrag);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor); // 设置十字光标
}

void ImageView::setImage(const cv::Mat &mat) {
    currentImage = mat.clone();

    // 清除旧图像项
    if (pixmapItem) {
        scene()->removeItem(pixmapItem);
        delete pixmapItem;
    }

    // 将 OpenCV Mat 转换为 QImage
    QImage qimg;
    if (currentImage.type() == CV_8UC3) {
        cv::cvtColor(currentImage, currentImage, cv::COLOR_BGR2RGB);
        qimg = QImage(
                   currentImage.data,
                   currentImage.cols,
                   currentImage.rows,
                   currentImage.step,
                   QImage::Format_RGB888
                   ).copy();
    } else if (currentImage.type() == CV_8UC1) {
        qimg = QImage(
                   currentImage.data,
                   currentImage.cols,
                   currentImage.rows,
                   currentImage.step,
                   QImage::Format_Grayscale8
                   ).copy();
    }

    // 添加新图像项到场景
    pixmapItem = scene()->addPixmap(QPixmap::fromImage(qimg));
    scene()->setSceneRect(pixmapItem->boundingRect());
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void ImageView::mouseMoveEvent(QMouseEvent *event) {
    // 转换为场景坐标
    QPointF scenePos = mapToScene(event->pos());
    int x = static_cast<int>(scenePos.x());
    int y = static_cast<int>(scenePos.y());

    // 边界检查
    if (x >= 0 && x < currentImage.cols && y >= 0 && y < currentImage.rows) {
        // 获取像素值
        if (currentImage.type() == CV_8UC3) {
            cv::Vec3b pixel = currentImage.at<cv::Vec3b>(y, x);
            emit pixelHovered(x, y, QColor(pixel[2], pixel[1], pixel[0]));
        } else if (currentImage.type() == CV_8UC1) {
            uchar gray = currentImage.at<uchar>(y, x);
            emit pixelHovered(x, y, QColor(gray, gray, gray));
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

// 创建文本框
void ImageView::addTextItem()
{
    // 创建一个EditableTextItem
    //EditableTextItem* textItem = new EditableTextItem("hellow");
    QLineEdit* textItem = new QLineEdit("1");
    scene()->addWidget(textItem);
    textItem->setValidator(new QIntValidator);
}

