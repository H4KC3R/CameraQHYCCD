#include "imageblurmetric.h"

double ImageBlurMetric::getBlurLaplacian(cv::Mat image){
    cv::Mat output, outputAbs;
    cv::Scalar mean,stddev; //0:1st channel, 1:2nd channel and 2:3rd channel

    cv::Laplacian(image, output, CV_64F, 3, 1, 0, cv::BORDER_DEFAULT);
    // изначально выходное изображение было в формате 64 float,
    // чтобы избежать переполнение
    cv::convertScaleAbs(output, outputAbs);
    cv::meanStdDev(outputAbs, mean, stddev, cv::Mat());

    double variance = stddev.val[0] * stddev.val[0];
    return variance;
}

double ImageBlurMetric::getBlurSobel(cv::Mat image) {
    cv::Mat outputX, outputY;

    cv::Sobel(image, outputX, CV_64F, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::Sobel(image, outputY, CV_64F, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    cv::Mat1f magn;

    cv::magnitude(outputX, outputY, magn);
    cv::Scalar mean = cv::mean(magn);

    return mean.val[0];
}

double ImageBlurMetric::getBlurScharr(cv::Mat image) {
    cv::Mat outputX, outputY;

    cv::Scharr(image, outputX, CV_64F, 1, 0, 1, 0, cv::BORDER_DEFAULT);
    cv::Scharr(image, outputY, CV_64F, 0, 1, 1, 0, cv::BORDER_DEFAULT);

    cv::Mat1f magn;

    cv::magnitude(outputX, outputY, magn);
    cv::Scalar mean = cv::mean(magn);

    return mean.val[0];
}

double ImageBlurMetric::getBlurFFT(cv::Mat image) {

}
