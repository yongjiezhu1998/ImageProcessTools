#include "imageviewer.h"
// #include "ui_imageviewer.h"
#include <QWheelEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QMessageBox>
#include <QStatusBar>

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

    processLayout->addWidget(resetProcess);
    processLayout->addWidget(btnGrayscale);
    processLayout->addWidget(btnCanny);
    processLayout->addStretch();

    toolBox->addItem(processPage, "图像处理");

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
    connect(resetProcess, &QPushButton::clicked, this, &ImageViewer::resetProcess);
    connect(btnGrayscale, &QPushButton::clicked, this, &ImageViewer::applyGrayscale);
    connect(btnCanny, &QPushButton::clicked, this, &ImageViewer::applyCannyEdge);
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

    this->setFixedSize(800, 600); // 固定为 800x600 像素

    // 在 ImageViewer 构造函数中添加
    setMouseTracking(true); // 启用全局鼠标追踪
    // view->setMouseTracking(true); // 如果使用 QGraphicsView
    // setAttribute(Qt::WA_Hover); // 可选：增强Hover事件

    // 在视图初始化代码中（构造函数）
    // view->setDragMode(QGraphicsView::NoDrag); // 关键代码
    // view->setCursor(Qt::ArrowCursor);         // 显式设置光标样式
}

void ImageViewer::openImage() {
    QString path = QFileDialog::getOpenFileName(
        this, "选择图片", "",
        "图片文件 (*.png *.jpg *.bmp)"
        );

    if (!path.isEmpty()) {
#if 0
        originalMat = cv::imread(path.toLocal8Bit().constData(), cv::IMREAD_COLOR);
        if (!originalMat.empty()) {
            cv::cvtColor(originalMat, originalMat, cv::COLOR_BGR2RGB);
            processedMat.release();
            isProcessed = false;
            updateDisplay();
        }
#else
        originalMat = cv::imread(path.toLocal8Bit().constData());
        if (originalMat.empty()) {
            QMessageBox::critical(this, "错误", "图片加载失败");
            return;
        }
        view->setImage(originalMat);
#endif
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

// 灰度化处理
void ImageViewer::applyGrayscale() {
    if (originalMat.empty()) return;

    cv::cvtColor(originalMat, processedMat, cv::COLOR_RGB2GRAY);
    isProcessed = true;
    // updateDisplay();
    view->setImage(processedMat);
}

// Canny边缘检测
void ImageViewer::applyCannyEdge() {
    if (originalMat.empty()) return;

    cv::Mat gray;
    cv::cvtColor(originalMat, gray, cv::COLOR_RGB2GRAY);
    cv::Canny(gray, processedMat, 50, 150);
    isProcessed = true;
    // updateDisplay();
    view->setImage(processedMat);
}

// 重置图像处理
void ImageViewer::resetProcess()
{
    if (processedMat.empty()) return;    // 如果没有处理就return

    view->setImage(originalMat);
}

// ImageViewer.cpp
void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
    qDebug() << "Mouse moved at:" << event->pos();
    // event->accept(); // 明确接受事件，阻止继续传递
    if (originalMat.empty()) return;

    // 坐标转换
    QPoint viewPos = event->pos();
    QTransform transform = view->transform().inverted();
    QPointF scenePos = transform.map(QPointF(viewPos));

    int x = static_cast<int>(scenePos.x());
    int y = static_cast<int>(scenePos.y());
    qDebug() << "x:" << x << "y:" << y;

    if (x >= 0 && x < originalMat.cols && y >= 0 && y < originalMat.rows) {
        // 直接指针访问像素
        uchar* data = originalMat.data;
        int channels = originalMat.channels();
        int row = y * originalMat.step;

        int b = data[row + x * channels + 0];
        int g = data[row + x * channels + 1];
        int r = data[row + x * channels + 2];

        statusBar()->showMessage(
            QString("坐标: (%1, %2) | RGB: (%3, %4, %5)")
                .arg(x).arg(y).arg(r).arg(g).arg(b)
            );
    } else {
        statusBar()->clearMessage();
    }

    QMainWindow::mouseMoveEvent(event);
}


