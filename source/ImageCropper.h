#pragma once

#include "ui_ImageCropper.h"
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QRect>
#include <QtWidgets/QMainWindow>
#include <opencv2/opencv.hpp>

#include "QCropWidget.h"

class ImageCropper : public QMainWindow {
    Q_OBJECT

public:
    ImageCropper(QWidget* parent = nullptr);
    ~ImageCropper();

private:
    Ui::ImageCropperClass ui;
    void createActions();
    void createUI();

    void detectConnection();

    QMenu* connectionMenu;
    QAction* connectAction;

    component::QCropWidget* imageWidget;

    QPushButton* screencapButton;
    QPushButton* copyRoiButton;
    QPushButton* exportButton;
    QCheckBox* cropSecondaryButton;
    QPushButton* copyOffsetRoiButton;

    cv::Mat image;

    struct Roi {
        int x;
        int y;
        int width;
        int height;
    };

    Roi roi;
    Roi offsetRoi;

signals:
    void adbConnected();

public slots:
    void connect();
    void adbConnectedSlot();
    void screenshot();

    void doCrop(const QRect& rect, bool secondary);
    void copyRoi();
    void copyOffsetRoi();
    void exportImage();
};
