#include "imageviewer.h"
#include "ProcessorFactory.h"
#include "GrayscaleProcessor.h"
#include "CannyEdgeProcessor.h"
#include "ResetProcess.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 注册算法
    ProcessorFactory::registerProcessor("Grayscale", []() {
        return std::make_unique<GrayscaleProcessor>();
    });

    ProcessorFactory::registerProcessor("CannyEdge", []() {
        return std::make_unique<CannyEdgeProcessor>(50, 150); // 可配置参数
    });

    ProcessorFactory::registerProcessor("ResetProcess", []() {
        return std::make_unique<ResetProcess>(); // 可配置参数
    });

    ImageViewer w;
    w.show();
    return a.exec();
}
