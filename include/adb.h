#include <QProcess>
#include <opencv2/opencv.hpp>

namespace adb {
bool is_connected();
cv::Mat screenshot();
}
