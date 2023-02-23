#include "imageblurmetric.h"

double ImageBlurMetric::getBlurLaplacian(cv::Mat image){
    cv::Mat output, outputAbs;
    cv::Scalar mean,stddev; //0:1st channel, 1:2nd channel and 2:3rd channel

    cv::Laplacian(image, output, CV_16S, 3, 1, 0, cv::BORDER_DEFAULT);
    // изначально выходное изображение было в формате 16 signed,
    // чтобы избежать переполнение
    cv::convertScaleAbs(output, outputAbs);
    cv::meanStdDev(outputAbs, mean, stddev, cv::Mat());

    double variance = stddev.val[0] * stddev.val[0];
    return variance;
}
