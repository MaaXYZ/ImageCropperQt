#include "adb.h"
#include <QBuffer>
#include <QImageReader>
#include <QProcess>

bool adb::is_connected() {
    QProcess process;
    QStringList args;
    args << "devices";
    process.start("./adb", args);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    std::cout << output.toStdString() << std::endl;
    // get what's after "List of devices attached"
    int index = output.indexOf("List of devices attached");
    if (index == -1) {
        return false;
    }
    output = output.mid(index + 24);
    return output.contains("device");
}

cv::Mat adb::screenshot() {
    QProcess process;
    QStringList args;
    args << "exec-out"
         << "screencap"
         << "-p";

    process.start("./adb", args);
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    QBuffer buffer(&output);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer);
    QImage image = reader.read();

    cv::Mat mat;

    switch (image.format()) {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied: {
        mat = cv::Mat(
            image.height(),
            image.width(),
            CV_8UC4,
            (void*)image.constBits(),
            image.bytesPerLine());
        std::vector<cv::Mat> channels;
        split(mat, channels);
        channels.pop_back();
        cv::merge(channels, mat);
    } break;
    case QImage::Format_RGB888:
        mat = cv::Mat(
            image.height(),
            image.width(),
            CV_8UC3,
            (void*)image.constBits(),
            image.bytesPerLine());
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(
            image.height(),
            image.width(),
            CV_8UC1,
            (void*)image.constBits(),
            image.bytesPerLine());
        break;
    }

    cv::cvtColor(mat, mat, CV_BGR2RGB);

    return mat;
}
