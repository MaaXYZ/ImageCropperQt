#include "QCropWidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QWidget>

#include <opencv2/opencv.hpp>

component::QCropWidget::QCropWidget(QWidget* parent)
    : QLabel(parent) {
}

void component::QCropWidget::setImage(const cv::Mat& image) {
    m_image = image;
    QImage qimage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    qimage = qimage.scaled(size(), Qt::KeepAspectRatio);
    setPixmap(QPixmap::fromImage(qimage));
    m_imageRect = QRect(0, 0, qimage.width(), qimage.height());
}

void component::QCropWidget::selectedRoi(int* x, int* y, int* width, int* height) const {
    roiFromRect(m_cropRect, x, y, width, height);
}

void component::QCropWidget::offsetRoi(int* x, int* y, int* width, int* height) const {
    int x0, y0, w0, h0;
    selectedRoi(&x0, &y0, &w0, &h0);
    int x1, y1, w1, h1;
    roiFromRect(m_secondaryCropRect, &x1, &y1, &w1, &h1);
    *x = x1 - x0;
    *y = y1 - y0;
    *width = w1 - w0;
    *height = h1 - h0;
}

void component::QCropWidget::mousePressEvent(QMouseEvent* event) {
    if (m_image.empty() || event->pos().x() >= m_imageRect.right()) {
        return;
    }
    m_isCropping = true;
    m_shouldDrawRect = true;
    if (m_cropSecondary) {
        m_secondaryCropRect.setTopLeft(event->pos());
        m_secondaryCropRect.setBottomRight(event->pos());
    } else {
        m_cropRect.setTopLeft(event->pos());
        m_cropRect.setBottomRight(event->pos());
    }
    update();
}

void component::QCropWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_image.empty() || event->pos().x() >= m_imageRect.right()) {
        return;
    }
    if (m_isCropping) {
        if (m_cropSecondary) {
            m_secondaryCropRect.setBottomRight(event->pos());
        } else {
            m_cropRect.setBottomRight(event->pos());
        }
        update();
        emit cropChanged(m_cropRect, m_cropSecondary);
    }
}

void component::QCropWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (m_image.empty()) {
        return;
    }
    update();
    m_isCropping = false;
    emit cropChanged(m_cropRect, m_cropSecondary);
}

void component::QCropWidget::resizeEvent(QResizeEvent* event) {
    QLabel::resizeEvent(event);
    if (!m_image.empty()) {
        setImage(m_image);
    }
    m_isCropping = false;
    m_shouldDrawRect = false;
    update();
}

void component::QCropWidget::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
    if (m_shouldDrawRect) {
        m_painter.begin(this);
        m_painter.setPen(m_primaryPen);
        m_painter.drawRect(m_cropRect);

        if (m_cropSecondary) {
            m_painter.setPen(m_secondaryPen);
            m_painter.drawRect(m_secondaryCropRect);
        }
        m_painter.end();
    }
}

void component::QCropWidget::roiFromRect(const QRect& rect, int* x, int* y, int* width, int* height)
    const {
    *x = rect.topLeft().x() * 1280 / m_imageRect.width();
    *y = rect.topLeft().y() * 720 / m_imageRect.height();
    *width = rect.width() * 1280 / m_imageRect.width();
    *height = rect.height() * 720 / m_imageRect.height();
}

void component::QCropWidget::switchCrop(Qt::CheckState state) {
    switch (state) {
    case Qt::Unchecked:
        m_cropSecondary = false;
        break;
    case Qt::PartiallyChecked:
    case Qt::Checked:
        m_cropSecondary = true;
        break;
    }
    m_secondaryCropRect = QRect();
    update();
}

void component::QCropWidget::resetCrop() {
    m_isCropping = false;
    m_shouldDrawRect = false;
    update();
}
