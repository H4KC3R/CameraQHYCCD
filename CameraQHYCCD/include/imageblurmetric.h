#ifndef IMAGEBLURMETRIC_H
#define IMAGEBLURMETRIC_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

namespace ImageBlurMetric {

double getBlurLaplacian(cv::Mat image);

double getBlurSobel();

double getBlurScharr();

double getBlurDFT();

}

#endif // IMAGEBLURMETRIC_H
