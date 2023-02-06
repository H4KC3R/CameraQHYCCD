#include "imageprocess.h"

void ImageProcess::wb_img(camImage& src, camImage& result) {
    cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB();
    wb->balanceWhite(src.img, result.img);
    return;
}

void ImageProcess::debayer_img(camImage& src, camImage& result) {
    cv::cvtColor(src.img, result.img, cv::COLOR_BayerRG2BGR);
}

//void ImageProcess::contrast_img(CAM_Image *src, CAM_Image *result) {
//    return;
//}

//void ImageProcess::gc_img(CAM_Image *src, CAM_Image *result)
//{
//    return;
//}
