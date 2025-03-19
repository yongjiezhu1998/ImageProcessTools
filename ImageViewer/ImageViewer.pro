QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    imageview.cpp \
    main.cpp \
    imageviewer.cpp

HEADERS += \
    Blur.h \
    CannyEdgeProcessor.h \
    GaussianBlur.h \
    ImageProcessor.h \
    ProcessorFactory.h \
    ResetProcess.h \
    grayscaleprocessor.h \
    imageview.h \
    imageviewer.h

FORMS += \
    imageviewer.ui

INCLUDEPATH +=D:\OpenCV\opencv\build\include\
              D:\OpenCV\opencv\build\include\opencv\
              D:\OpenCV\opencv\build\include\opencv2

LIBS += -LD:/OpenCV/opencv/build/x64/vc16/lib -lopencv_world4110d

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
