#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <opencv2/opencv.hpp>

namespace component {
class QCropWidget : public QLabel {
    Q_OBJECT

public:
    QCropWidget(QWidget* parent = nullptr);

    void setImage(const cv::Mat& image);

    void selectedRoi(int* x, int* y, int* width, int* height) const;
    void offsetRoi(int* x, int* y, int* width, int* height) const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

private:
    bool m_isCropping = false;
    bool m_shouldDrawRect = false;
    bool m_cropSecondary = false;
    QRect m_imageRect;
    QRect m_cropRect;
    QRect m_secondaryCropRect;
    QPainter m_painter = QPainter(this);
    QPen m_primaryPen = QPen(Qt::red, 2);
    QPen m_secondaryPen = QPen(Qt::blue, 2);
    cv::Mat m_image;

    void roiFromRect(const QRect& rect, int* x, int* y, int* width, int* height) const;

signals:
    void cropChanged(const QRect& rect, bool secondary);

public slots:
    void resetCrop();
    void switchCrop(Qt::CheckState state);
};
}
