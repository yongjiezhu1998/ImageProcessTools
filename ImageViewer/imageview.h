#include <QGraphicsView>
#include <QMouseEvent>
#include <opencv2/opencv.hpp>
// imageview.h
#include <QGraphicsPixmapItem> // 添加此行

class ImageView : public QGraphicsView {
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent = nullptr);

    void setImage(const cv::Mat &mat); // 设置当前显示的图像数据

signals:
    // 鼠标移动时发出坐标和像素值信号
    void pixelHovered(int x, int y, const QColor &color);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void addTextItem();

private:
    cv::Mat currentImage; // 当前显示的图像数据
    QGraphicsPixmapItem *pixmapItem = nullptr;
    //QGraphicsScene* scene;
};
