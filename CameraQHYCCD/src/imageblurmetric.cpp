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
    int  = 60;
    int cx = image.cols/2;
    int cy = image.rows/2;

    cv::Mat fImage;
    image.convertTo(fImage, CV_32F);

    // FFT
    cv::Mat fourierTransform;
    cv::dft(fImage, fourierTransform, cv::DFT_SCALE|cv::DFT_COMPLEX_OUTPUT);

    //center low frequencies in the middle
    //by shuffling the quadrants.
    cv::Mat q0(fourierTransform, cv::Rect(0, 0, cx, cy));       // Top-Left - Create a ROI per quadrant
    cv::Mat q1(fourierTransform, cv::Rect(cx, 0, cx, cy));      // Top-Right
    cv::Mat q2(fourierTransform, cv::Rect(0, cy, cx, cy));      // Bottom-Left
    cv::Mat q3(fourierTransform, cv::Rect(cx, cy, cx, cy));     // Bottom-Right

    // swap quadrants (Top-Left with Bottom-Right)
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    // swap quadrant (Top-Right with Bottom-Left)
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    // Block the low frequencies
    // #define BLOCK could also be a argument on the command line of course
    fourierTransform(cv::Rect(cx-block, cy-block, 2*block, 2*block)).setTo(0);

    //shuffle the quadrants to their original position
    cv::Mat orgFFT;
    fourierTransform.copyTo(orgFFT);
    cv::Mat p0(orgFFT, cv::Rect(0, 0, cx, cy));       // Top-Left - Create a ROI per quadrant
    cv::Mat p1(orgFFT, cv::Rect(cx, 0, cx, cy));      // Top-Right
    cv::Mat p2(orgFFT, cv::Rect(0, cy, cx, cy));      // Bottom-Left
    cv::Mat p3(orgFFT, cv::Rect(cx, cy, cx, cy));     // Bottom-Right

    p0.copyTo(tmp);
    p3.copyTo(p0);
    tmp.copyTo(p3);

    // swap quadrant (Top-Right with Bottom-Left)
    p1.copyTo(tmp);
    p2.copyTo(p1);
    tmp.copyTo(p2);

    cv::Mat invFFT;
    cv::Mat logFFT;
    double minVal,maxVal;

    cv::dft(orgFFT, invFFT, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);

    invFFT = cv::abs(invFFT);
    cv::minMaxLoc(invFFT,&minVal,&maxVal,NULL,NULL);

    //check for impossible values
    if(maxVal<=0.0)
        return -1;

    cv::log(invFFT,logFFT);
    logFFT *= 20;

    cv::Scalar result= cv::mean(logFFT);

    return result.val[0];
}
