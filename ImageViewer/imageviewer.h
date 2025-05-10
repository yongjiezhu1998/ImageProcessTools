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
#include <QSettings>

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
    void onOtsuTriggered();
    void onOpenCamera();

public Q_SLOTS:
    void on_open_triggered();
    void on_fixWindow_triggered(bool checked);


private:
    void updateDisplay();      // 更新显示
    void updateDisplay(const cv::Mat &displayMat);

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
    cv::Mat ostuMat;
    bool isProcessed = false;  // 当前显示状态

    // 形态学操作全局变量
    int shape;
    int ksize;
    int iter;

    QPushButton* btnOpen;
    QPushButton* btnSave;
    QPushButton* btnReset;

    QPushButton* btnResetProcess;
    QPushButton* btnGrayscale;
    QPushButton* btnCanny;
    QPushButton* btnGaussian;
    QPushButton* btnCreteLineEdit;

    QPushButton* openCameraButton;
    QPushButton* closeCameraButton;

    cv::VideoCapture cap;

    
    void initUI();

    void setupConnections();

    void initStyle();       

    void createGraphicsView();

    void setupMainLayout();

    // 创建左侧工具箱
    void createToolBox();

    QWidget* createBasicToolsPage();

    QWidget* createProcessPage();

    QWidget* createGaussianBlurPage();

    QWidget* createBlurPage();

    QWidget* createMorphologicalPage();

    QWidget* createCameraPage();

    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);

    // bool event(QEvent *e);
    void onPixelHovered(int x, int y, const QColor &color);
    void applyProcessor(const std::string &processorName);
    QWidget* gaussianBlurBoxLayout();
    QWidget* blurBoxLayout();
    QWidget * morphologicalBoxLayout();
    QWidget* cameraBoxLayout();
    void applyMorphologicalOperation(int operation);
};
#endif // IMAGEVIEWER_H
