#include "imageviewer.h"
#include "ProcessorFactory.h"
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
#include "DilationProcessor.h"
#include "ErosionProcessor.h"
#include "OpenProcessor.h"
#include "CloseProcessor.h"

ImageViewer::ImageViewer(QWidget *parent) : QMainWindow(parent) {
    
    initUI();

    setupConnections();// 信号槽连接

    initStyle();
}

void ImageViewer::initUI()
{
     createToolBox();       // 创建左侧工具箱
     createGraphicsView();  // 创建图形视图
     setupMainLayout();     // 主布局管理
}

void ImageViewer::setupConnections()
{
    // ========== 信号连接 ==========
    connect(btnOpen, &QPushButton::clicked, this, &ImageViewer::openImage);
    connect(btnSave, &QPushButton::clicked, this, &ImageViewer::saveImage);
    connect(btnReset, &QPushButton::clicked, this, &ImageViewer::resetZoom);
    connect(btnResetProcess, &QPushButton::clicked, this, &ImageViewer::onResetProcess);
    connect(btnGrayscale, &QPushButton::clicked, this, &ImageViewer::onGrayscaleTriggered);
    connect(btnCanny, &QPushButton::clicked, this, &ImageViewer::onCannyEdgeTriggered);
    connect(btnGaussian, &QPushButton::clicked, this, &ImageViewer::onGaussianBlurTriggered);
    connect(view, &ImageView::pixelHovered, this, &ImageViewer::onPixelHovered);
}

void ImageViewer::initStyle()
{
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
}

void ImageViewer::createGraphicsView()
{
    // ========== 创建图形视图 ==========
    scene = new QGraphicsScene(this);
    view = new ImageView(this);
    setCentralWidget(view);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void ImageViewer::setupMainLayout()
{
    // ========== 主布局 ==========
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(toolBox);
    splitter->addWidget(view);
    splitter->setSizes({ 200, 824 });
    splitter->setStretchFactor(1, 1); // 右侧视图区域可拉伸

    setCentralWidget(splitter); // 设置为主窗口中心部件

    this->setFixedSize(1000, 800);
    this->setMouseTracking(true); // 启用全局鼠标追踪
}

void ImageViewer::createToolBox() {
    toolBox = new QToolBox(this);
    toolBox->addItem(createBasicToolsPage(), "基本操作");
    toolBox->addItem(createProcessPage(), "图像处理");
    toolBox->addItem(createGaussianBlurPage(), "高斯滤波器");
    toolBox->addItem(createBlurPage(), "中值滤波器");
    //toolBox->addItem(createMorphologicalPage(), "形态学操作");
}

QWidget* ImageViewer::createBasicToolsPage() {
    QWidget* basicToolsPage = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(basicToolsPage);

    btnOpen = new QPushButton("打开图片", basicToolsPage);
    btnSave = new QPushButton("保存图片", basicToolsPage);
    btnReset = new QPushButton("重置缩放", basicToolsPage);

    layout->addWidget(btnOpen);
    layout->addWidget(btnSave);
    layout->addWidget(btnReset);
    layout->addStretch();
    return basicToolsPage;
}

QWidget* ImageViewer::createProcessPage()
{
    QWidget* processPage = new QWidget;
    QVBoxLayout* processLayout = new QVBoxLayout(processPage);

    btnResetProcess = new QPushButton("恢复原图", processPage);
    btnGrayscale = new QPushButton("灰度化", processPage);
    btnCanny = new QPushButton("边缘检测", processPage);
    btnGaussian = new QPushButton("高斯滤波", processPage);

    processLayout->addWidget(btnResetProcess);
    processLayout->addWidget(btnGrayscale);
    processLayout->addWidget(btnCanny);
    processLayout->addWidget(btnGaussian);
    processLayout->addStretch();

    return processPage;
}

QWidget* ImageViewer::createGaussianBlurPage()
{
    QWidget* gaussianBlurPage = gaussianBlurBoxLayout();

    return gaussianBlurPage;
}

QWidget* ImageViewer::createBlurPage()
{
    QWidget* blurPage = blurBoxLayout();

    return blurPage;
}

QWidget* ImageViewer::createMorphologicalPage()
{
    QWidget* morphologicalPage = morphologicalBoxLayout();

    return morphologicalPage;
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
    QWidget *morphPage = new QWidget;
    QFormLayout *formLayout = new QFormLayout(morphPage);

    /*addRow*/
    QPushButton *applyButton = new QPushButton("OTSU自动阈值法", morphPage);
    formLayout->addRow(applyButton);

    // 注册OUST二值分割
    ProcessorFactory::registerProcessor("OTSU", []() -> std::unique_ptr<ImageProcessor> {
        return std::make_unique<OtsuProcessor>();
    });

    connect(applyButton, &QPushButton::clicked, this, &ImageViewer::onOtsuTriggered);

    // return morphologicalBlurPage;
    // 操作类型选择
    QComboBox *operationCombo = new QComboBox(morphPage);
    operationCombo->addItem("膨胀", cv::MORPH_DILATE);
    operationCombo->addItem("腐蚀", cv::MORPH_ERODE);
    operationCombo->addItem("开运算", cv::MORPH_OPEN);
    operationCombo->addItem("闭运算", cv::MORPH_CLOSE);
    formLayout->addRow("操作类型:", operationCombo);

    // 结构元素形状
    QComboBox *kernelShapeCombo = new QComboBox(morphPage);
    kernelShapeCombo->addItem("矩形", cv::MORPH_RECT);
    kernelShapeCombo->addItem("椭圆", cv::MORPH_ELLIPSE);
    kernelShapeCombo->addItem("十字", cv::MORPH_CROSS);
    formLayout->addRow("结构元素形状:", kernelShapeCombo);
    shape = kernelShapeCombo->currentData().toInt();

    // 结构元素大小（奇数）
    QSpinBox *kernelSizeSpin = new QSpinBox(morphPage);
    kernelSizeSpin->setMinimum(1);
    kernelSizeSpin->setMaximum(31);
    kernelSizeSpin->setSingleStep(2);
    kernelSizeSpin->setValue(3);
    formLayout->addRow("结构元素大小:", kernelSizeSpin);
    ksize = kernelSizeSpin->value();

    // 迭代次数
    QSpinBox *iterationsSpin = new QSpinBox(morphPage);
    iterationsSpin->setMinimum(1);
    iterationsSpin->setMaximum(10);
    iterationsSpin->setValue(1);
    formLayout->addRow("迭代次数:", iterationsSpin);
    iter = iterationsSpin->value();

    // 应用按钮
    QPushButton *Button = new QPushButton("应用形态学操作", morphPage);
    formLayout->addRow(Button);

    ProcessorFactory::registerProcessor("MORPH_DILATE", [this]()->std::unique_ptr<ImageProcessor> {
        return std::make_unique<DilationProcessor>(cv::MORPH_DILATE, shape, ksize, iter);
    });

    ProcessorFactory::registerProcessor("MORPH_ERODE", [this]()-> std::unique_ptr<ImageProcessor>{
        return std::make_unique<ErosionProcessor>(cv::MORPH_ERODE, shape, ksize, iter);
    });

    ProcessorFactory::registerProcessor("MORPH_OPEN", [this]()->std::unique_ptr<ImageProcessor>{
        return std::make_unique<OpenProcessor>(cv::MORPH_OPEN, shape, ksize, iter);
    });

    ProcessorFactory::registerProcessor("MORPH_CLOSE", [this]()->std::unique_ptr<ImageProcessor> {
        return std::make_unique<CloseProcessor>(cv::MORPH_CLOSE, shape, ksize, iter);
    });

    // 连接信号槽
    connect(Button, &QPushButton::clicked, this, [this, operationCombo]() {
        applyMorphologicalOperation(
            operationCombo->currentData().toInt()
            );
    });



    return morphPage;
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

// ImageViewer.cpp
void ImageViewer::updateDisplay(const cv::Mat &displayMat) {
    scene->clear();

    if (displayMat.empty()) {
        qWarning() << "显示图像为空!";
        return;
    }

    // 深拷贝图像数据并转换为 RGB 格式
    cv::Mat displayMatCopy;
    if (displayMat.channels() == 3) {
        cv::cvtColor(displayMat, displayMatCopy, cv::COLOR_BGR2RGB); // BGR → RGB
    } else if (displayMat.channels() == 1) {
        cv::cvtColor(displayMat, displayMatCopy, cv::COLOR_GRAY2RGB); // 灰度 → RGB
    } else {
        qWarning() << "不支持的图像通道数:" << displayMat.channels();
        return;
    }

    // 确保图像为 8UC3 格式
    if (displayMatCopy.type() != CV_8UC3) {
        displayMatCopy.convertTo(displayMatCopy, CV_8UC3);
    }

    // 创建 QImage 并深拷贝数据
    QImage qimg(
        displayMatCopy.data,
        displayMatCopy.cols,
        displayMatCopy.rows,
        displayMatCopy.step,
        QImage::Format_RGB888
        );

    // 添加图像项到场景
    pixmapItem = scene->addPixmap(QPixmap::fromImage(qimg));
    scene->setSceneRect(pixmapItem->boundingRect());

    // 自适应缩放
    if (!view->transform().isIdentity()) {
        view->resetTransform();
    }
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // 强制刷新视图
    view->viewport()->update();
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
    ostuMat = processor->process(originalMat);

    // 更新视图
    view->setImage(ostuMat);
}

void ImageViewer::applyMorphologicalOperation(int operation) {
    if (originalMat.empty()) return;

    // 根据操作类型选择处理器
    std::unique_ptr<ImageProcessor> processor;
    switch (operation) {
    case 0:
        processor = ProcessorFactory::create("MORPH_DILATE");
        qDebug() << "MORPH_DILATE";
        break;
    case 1:
        processor = ProcessorFactory::create("MORPH_ERODE");
        break;
    case 2:
        processor = ProcessorFactory::create("MORPH_OPEN");
        break;
    case 3:
        processor = ProcessorFactory::create("MORPH_CLOSE");
        break;
    default:
        QMessageBox::warning(this, "错误", "未知的操作类型");
        return;
    }

    // 执行处理并显示
    if (ostuMat.empty())
    {
        return;
    }
    cv::Mat result = processor->process(ostuMat);
    // updateDisplay(result);
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

