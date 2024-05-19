#include "ImageCropper.h"
#include "QCropWidget.h"
#include "adb.h"

#include <opencv2/opencv.hpp>

#include <QClipboard>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QImage>
#include <QInputDialog>
#include <QPixmap>
#include <QProcess>
#include <QRect>

ImageCropper::ImageCropper(QWidget* parent)
    : QMainWindow(parent) {
    ui.setupUi(this);

    createActions();

    QObject::connect(this, &ImageCropper::adbConnected, this, &ImageCropper::adbConnectedSlot);

    createUI();
    detectConnection();
}

ImageCropper::~ImageCropper() {
}

void ImageCropper::createActions() {
    connectionMenu = menuBar()->addMenu(tr("&Connection"));
    connectAction = connectionMenu->addAction(tr("&Connect"));

    QObject::connect(connectAction, &QAction::triggered, this, &ImageCropper::connect);
}

void ImageCropper::createUI() {
    auto layout = new QHBoxLayout();

    centralWidget()->setLayout(layout);

    imageWidget = new component::QCropWidget();
    QObject::connect(
        imageWidget,
        &component::QCropWidget::cropChanged,
        this,
        &ImageCropper::doCrop);
    QObject::connect(
        this,
        &ImageCropper::screenshot,
        imageWidget,
        &component::QCropWidget::resetCrop);
    layout->addWidget(imageWidget, 9);

    auto buttonsBox = new QGroupBox(tr("Actions"));
    auto buttonsLayout = new QVBoxLayout();
    buttonsBox->setLayout(buttonsLayout);

    // buttonsBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    // buttonsBox->setMinimumWidth(100);
    layout->addWidget(buttonsBox, 1);

    screencapButton = new QPushButton(tr("Screencap"));
    screencapButton->setEnabled(false); // disable until connected
    buttonsLayout->addWidget(screencapButton);
    QObject::connect(screencapButton, &QPushButton::clicked, this, &ImageCropper::screenshot);
    QObject::connect(
        screencapButton,
        &QPushButton::clicked,
        imageWidget,
        &component::QCropWidget::resetCrop);

    copyRoiButton = new QPushButton(tr("Copy ROI"));
    copyRoiButton->setEnabled(false); // disable until roi is selected
    buttonsLayout->addWidget(copyRoiButton, 1);
    QObject::connect(copyRoiButton, &QPushButton::clicked, this, &ImageCropper::copyRoi);

    exportButton = new QPushButton(tr("Export"));
    exportButton->setEnabled(false); // disable until roi is selected
    buttonsLayout->addWidget(exportButton, 1);
    QObject::connect(exportButton, &QPushButton::clicked, this, &ImageCropper::exportImage);

    cropSecondaryButton = new QCheckBox(tr("Secondary crop"));
    cropSecondaryButton->setEnabled(false); // disable until roi is selected
    buttonsLayout->addWidget(cropSecondaryButton);
    QObject::connect(
        cropSecondaryButton,
        &QCheckBox::checkStateChanged,
        imageWidget,
        &component::QCropWidget::switchCrop);

    copyOffsetRoiButton = new QPushButton(tr("Copy offset ROI"));
    copyOffsetRoiButton->setEnabled(false); // disable until roi is selected
    buttonsLayout->addWidget(copyOffsetRoiButton, 1);
    QObject::connect(
        copyOffsetRoiButton,
        &QPushButton::clicked,
        this,
        &ImageCropper::copyOffsetRoi);

    buttonsLayout->addStretch();
}

void ImageCropper::detectConnection() {
    std::cout << "Detecting connection" << std::endl;
    bool connected = adb::is_connected();
    if (connected) {
        emit adbConnected();
        screencapButton->setEnabled(true);
    }
}

void ImageCropper::connect() {
    bool ok;
    QString address = QInputDialog::getText(
        this,
        tr("Connect to device"),
        tr("Enter device address:"),
        QLineEdit::Normal,
        "",
        &ok);

    if (ok && !address.isEmpty()) {
        QProcess process;
        process.start("adb connect " + address);
        process.waitForFinished();
        detectConnection();
    }
}

void ImageCropper::adbConnectedSlot() {
    connectAction->setEnabled(false);
    connectAction->setText(tr("Connected"));
}

void ImageCropper::screenshot() {
    image = adb::screenshot();
    imageWidget->setImage(image);

    copyRoiButton->setEnabled(false);
    exportButton->setEnabled(false);
    cropSecondaryButton->setEnabled(false);
}

void ImageCropper::doCrop(const QRect& rect, bool secondary) {
    // convert topleft to coordinates in the original image
    if (secondary) {
        imageWidget->offsetRoi(&offsetRoi.x, &offsetRoi.y, &offsetRoi.width, &offsetRoi.height);
    } else {
        imageWidget->selectedRoi(&roi.x, &roi.y, &roi.width, &roi.height);
    }

    copyRoiButton->setEnabled(true);
    exportButton->setEnabled(true);
    cropSecondaryButton->setEnabled(true);
    if (secondary) {
        copyOffsetRoiButton->setEnabled(true);
    }
}

void ImageCropper::copyRoi() {
    // copy the roi to the clipboard
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(tr("[%1, %2, %3, %4]").arg(roi.x).arg(roi.y).arg(roi.width).arg(roi.height));
}

void ImageCropper::copyOffsetRoi() {
    // copy the offset roi to the clipboard
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(tr("[%1, %2, %3, %4]")
                           .arg(offsetRoi.x)
                           .arg(offsetRoi.y)
                           .arg(offsetRoi.width)
                           .arg(offsetRoi.height));
}

void ImageCropper::exportImage() {
    auto filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png)"));

    if (!filename.isEmpty()) {
        auto imageWidth = image.cols;
        auto imageHeight = image.rows;

        // scale roi back to original image size
        auto x = roi.x * imageWidth / 1280;
        auto y = roi.y * imageHeight / 720;
        auto width = roi.width * imageWidth / 1280;
        auto height = roi.height * imageHeight / 720;

        cv::Mat roi = image(cv::Rect(x, y, width, height));
        cv::imwrite(filename.toStdString(), roi);
    }
}
