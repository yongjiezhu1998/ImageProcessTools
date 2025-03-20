#include "imageviewer.h"
#include "ProcessorFactory.h"
// #include "ui_imageviewer.h"
#include <QWheelEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QMessageBox>
#include <QStatusBar>
#include <QComboBox>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include "GaussianBlur.h"
#include "Blur.h"
#include "otsuProcessor.h"

ImageViewer::ImageViewer(QWidget *parent) : QMainWindow(parent) {
    // ========== 创建左侧工具箱 ==========
    toolBox = new QToolBox(this);

    // 添加工具页：基本操作
    QWidget *basicToolsPage = new QWidget;
    QVBoxLayout *basicLayout = new QVBoxLayout(basicToolsPage);

    QPushButton *btnOpen = new QPushButton("打开图片", basicToolsPage);
    QPushButton *btnSave = new QPushButton("保存图片", basicToolsPage);
    QPushButton *btnReset = new QPushButton("重置缩放", basicToolsPage);
    basicLayout->addWidget(btnOpen);
    basicLayout->addWidget(btnSave);
    basicLayout->addWidget(btnReset);
    basicLayout->addStretch(); // 占位填充

    toolBox->addItem(basicToolsPage, "基本操作");

    // 添加工具页：图像处理
    QWidget *processPage = new QWidget;
    QVBoxLayout *processLayout = new QVBoxLayout(processPage);

    QPushButton *resetProcess = new QPushButton("恢复原图", processPage);
    QPushButton *btnGrayscale = new QPushButton("灰度化", processPage);
    QPushButton *btnCanny = new QPushButton("边缘检测", processPage);
    QPushButton *btnGaussian = new QPushButton("高斯滤波", processPage);

    processLayout->addWidget(resetProcess);
    processLayout->addWidget(btnGrayscale);
    processLayout->addWidget(btnCanny);
    processLayout->addWidget(btnGaussian);
    processLayout->addStretch();

    toolBox->addItem(processPage, "图像处理");

    // 添加工具页：高斯滤波器处理
    QWidget *gaussianBlurPage = gaussianBlurBoxLayout();
    toolBox->addItem(gaussianBlurPage, "高斯滤波器");

    // 中值滤波
    QWidget *blurPage = blurBoxLayout();
    toolBox->addItem(blurPage, "中值滤波器");

    // 形态学操作
    QWidget *morphologicalPage = morphologicalBoxLayout();
    toolBox->addItem(morphologicalPage, "形态学操作");


    // ========== 创建图形视图 ==========
    scene = new QGraphicsScene(this);
    // view = new QGraphicsView(scene);
    view = new ImageView(this);
    setCentralWidget(view);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // ========== 主布局 ==========
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(toolBox);
    splitter->addWidget(view);
    splitter->setSizes({200, 824});
    splitter->setStretchFactor(1, 1); // 右侧视图区域可拉伸

    setCentralWidget(splitter); // 设置为主窗口中心部件

    // ========== 信号连接 ==========
    connect(btnOpen, &QPushButton::clicked, this, &ImageViewer::openImage);
    connect(btnSave, &QPushButton::clicked, this, &ImageViewer::saveImage);
    connect(btnReset, &QPushButton::clicked, this, &ImageViewer::resetZoom);
    connect(resetProcess, &QPushButton::clicked, this, &ImageViewer::onResetProcess);
    connect(btnGrayscale, &QPushButton::clicked, this, &ImageViewer::onGrayscaleTriggered);
    connect(btnCanny, &QPushButton::clicked, this, &ImageViewer::onCannyEdgeTriggered);
    connect(btnGaussian, &QPushButton::clicked, this, &ImageViewer::onGaussianBlurTriggered);
    connect(view, &ImageView::pixelHovered, this, &ImageViewer::onPixelHovered);

    // 设置工具箱样式
    toolBox->setStyleSheet(
        "QToolBox::tab {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #F0F0F0, stop:1 #E0E0E0);"
        "   border-radius: 4px;"
        "   margin: 2px;"
        "}"
        "QToolBox::tab:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #D0D0FF, stop:1 #C0C0FF);"
        "}"
    );

    this->setFixedSize(1000, 800);

    // 在 ImageViewer 构造函数中添加
    setMouseTracking(true); // 启用全局鼠标追踪
}

QWidget* ImageViewer::gaussianBlurBoxLayout()
{
    QWidget *gaussianBlurPage = new QWidget;
    QFormLayout *formLayout = new QFormLayout(gaussianBlurPage);

    // 核大小选择（正奇数）
    kernelSizeCombo = new QComboBox();
    for (int i = 3; i <= 15; i += 2) {
        kernelSizeCombo->addItem(QString::number(i) + "x" + QString::number(i), i);
    }
    formLayout->addRow("核大小:", kernelSizeCombo);

    // X/Y标准差输入（带同步锁）
    sigmaXSpin = new QDoubleSpinBox(gaussianBlurPage);
    sigmaXSpin->setRange(0.1, 10.0);
    sigmaXSpin->setValue(1.5);
    sigmaXSpin->setSingleStep(0.1);

    sigmaYSpin = new QDoubleSpinBox(gaussianBlurPage);
    sigmaYSpin->setRange(0.1, 10.0);
    sigmaYSpin->setValue(1.5);
    sigmaYSpin->setSingleStep(0.1);

    QCheckBox *syncSigmaCheck = new QCheckBox("同步 X/Y", gaussianBlurPage);
    syncSigmaCheck->setChecked(true);
    sigmaYSpin->setEnabled(false);

    // 将标准差控件放入水平布局
    QHBoxLayout *sigmaLayout = new QHBoxLayout();
    sigmaLayout->addWidget(sigmaXSpin);
    sigmaLayout->addWidget(new QLabel("→", gaussianBlurPage)); // 箭头分隔符
    sigmaLayout->addWidget(sigmaYSpin);
    sigmaLayout->addWidget(syncSigmaCheck);
    formLayout->addRow("标准差 (σ):", sigmaLayout);

    // 应用按钮
    QPushButton *applyButton = new QPushButton("应用滤波", gaussianBlurPage);
    formLayout->addRow(applyButton);

    connect(sigmaXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val) {
        if (syncSigmaCheck->isChecked()) {
            sigmaYSpin->blockSignals(true); // 避免循环触发
            sigmaYSpin->setValue(val);
            sigmaYSpin->blockSignals(false);
        }
    });

    connect(syncSigmaCheck, &QCheckBox::toggled, [=](bool checked) {
        sigmaYSpin->setEnabled(!checked);
        if (checked) sigmaYSpin->setValue(sigmaXSpin->value());
    });

    // 注册高斯滤波器处理器（绑定当前参数）
    ProcessorFactory::registerProcessor("GaussianBlur", [this]() -> std::unique_ptr<ImageProcessor> {
        // 从当前界面控件获取参数
        int kernelSize = this->kernelSizeCombo->currentData().toInt();
        double sigmaX = this->sigmaXSpin->value();
        double sigmaY = this->sigmaYSpin->value();

        // 创建并返回带参数的处理器
        return std::make_unique<GaussianBlur>(kernelSize, sigmaX, sigmaY);
    });

    // connect
    connect(applyButton, &QPushButton::clicked, this, &ImageViewer::onGaussianBlurTriggered);


    return gaussianBlurPage;
}

QWidget *ImageViewer::blurBoxLayout()
{
    QWidget *blurPage = new QWidget;
    QFormLayout *formLayout = new QFormLayout(blurPage);

    // 核大小选择（正奇数）
    kernelSizeCombo = new QComboBox();
    for (int i = 3; i <= 15; i += 2) {
        kernelSizeCombo->addItem(QString::number(i) + "x" + QString::number(i), i);
    }
    formLayout->addRow("核大小:", kernelSizeCombo);

    // 应用按钮
    QPushButton *applyButton = new QPushButton("应用滤波", blurPage);
    formLayout->addRow(applyButton);

    // 注册中值滤波器处理器（绑定当前参数）
    ProcessorFactory::registerProcessor("Blur", [this]() -> std::unique_ptr<ImageProcessor> {
        // 从当前界面控件获取参数
        int kernelSize = kernelSizeCombo->currentData().toInt();

        // 创建并返回带参数的处理器
        return std::make_unique<Blur>(kernelSize);
    });

    // connect
    connect(applyButton, &QPushButton::clicked, this, &ImageViewer::onBlurTriggered);

    return blurPage;
}

QWidget *ImageViewer::morphologicalBoxLayout()
{
    QWidget *morphologicalBlurPage = new QWidget;
    QFormLayout *formLayout = new QFormLayout(morphologicalBlurPage);

    /*addRow*/
    QPushButton *applyButton = new QPushButton("OTSU自动阈值法", morphologicalBlurPage);
    formLayout->addRow(applyButton);

    // 注册OUST二值分割
    ProcessorFactory::registerProcessor("OTSU", []() -> std::unique_ptr<ImageProcessor> {
        return std::make_unique<OtsuProcessor>();
    });

    connect(applyButton, &QPushButton::clicked, this, &ImageViewer::onOtsuTriggered);

    return morphologicalBlurPage;

}




void ImageViewer::openImage()
{
    QString path = QFileDialog::getOpenFileName(
        this, "选择图片", "",
        "图片文件 (*.png *.jpg *.bmp)"
        );

    if (!path.isEmpty()) {
        originalMat = cv::imread(path.toLocal8Bit().constData());
        if (originalMat.empty()) {
            QMessageBox::critical(this, "错误", "图片加载失败");
            return;
        }
        view->setImage(originalMat);
    }
}


void ImageViewer::saveImage() {
    // 检查是否有处理后的图像
    if (processedMat.empty()) {
        QMessageBox::warning(this,
                             tr("保存错误"),
                             tr("没有可保存的处理结果，请先处理图像！")
                             );
        return;
    }

    // 设置文件格式过滤器
    QString filters =
        "PNG 图像 (*.png);;"
        "JPEG 图像 (*.jpg *.jpeg);;"
        "位图文件 (*.bmp);;"
        "所有文件 (*.*)";

    // 获取保存路径
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("保存处理结果"),
        QDir::homePath() + "/Untitled.png", // 默认文件名
        filters
        );

    if (fileName.isEmpty()) return; // 用户取消操作

    // 自动补全文件扩展名
    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();
    if (suffix.isEmpty()) {
        QString selectedFilter = filters.split(";;").first();
        if (selectedFilter.contains("png")) fileName += ".png";
        else if (selectedFilter.contains("jpg")) fileName += ".jpg";
        else if (selectedFilter.contains("bmp")) fileName += ".bmp";
    }

    // 转换文件路径
    std::string filePath = fileName.toStdString();

    // 保存图像
    bool isSaved = false;
    try {
        isSaved = cv::imwrite(filePath, processedMat);
    } catch (const cv::Exception& e) {
        QMessageBox::critical(
            this,
            tr("保存错误"),
            tr("OpenCV 保存失败: ") + QString(e.what())
            );
        return;
    }

    // 处理保存结果
    if (isSaved) {
        statusBar()->showMessage(
            tr("图像已保存至: ") + fileName,
            5000 // 显示5秒
            );
    } else {
        QMessageBox::critical(
            this,
            tr("保存错误"),
            tr("无法保存图像到指定路径！\n请检查：\n1. 路径有效性\n2. 文件权限\n3. 格式支持")
            );
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

void ImageViewer::onPixelHovered(int x, int y, const QColor &color)
{
    statusBar()->showMessage(
        QString("坐标: (%1, %2) | RGB: (%3, %4, %5)")
            .arg(x).arg(y)
            .arg(color.red()).arg(color.green()).arg(color.blue())
        );
}

// 重置图像处理
void ImageViewer::resetProcess()
{
    if (processedMat.empty()) return;    // 如果没有处理就return

    view->setImage(originalMat);
}


void ImageViewer::applyProcessor(const std::string &processorName) {
    if (originalMat.empty()) return;

    // 创建处理器
    auto processor = ProcessorFactory::create(processorName);
    if (!processor) {
        QMessageBox::warning(this, "错误", "未找到指定算法");
        return;
    }

    // 执行处理
    cv::Mat result = processor->process(originalMat);

    // 更新视图
    view->setImage(result);
}

void ImageViewer::onGrayscaleTriggered() {
    applyProcessor("Grayscale");
}

void ImageViewer::onGaussianBlurTriggered()
{
    applyProcessor("GaussianBlur");
}

void ImageViewer::onBlurTriggered()
{
    applyProcessor("Blur");
}

void ImageViewer::onOtsuTriggered()
{
    applyProcessor("OTSU");
}

void ImageViewer::onCannyEdgeTriggered()
{
    applyProcessor("CannyEdge");
}

void ImageViewer::onResetProcess()
{
    applyProcessor("ResetProcess");
}

