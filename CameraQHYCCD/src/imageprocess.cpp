#include "imageprocess.h"

//void ImageProcess::wb_img(CAM_Image *src, CAM_Image *result) {
//    cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB();
//    wb->balanceWhite(src->ImgData, result->ImgData);
//    return;
//}

void ImageProcess::debayer_img(CAM_Image *src, CAM_Image *result) {
    cv::cvtColor(src->img, result->img, cv::COLOR_BayerRG2BGR);
}

//void ImageProcess::contrast_img(CAM_Image *src, CAM_Image *result) {
//    return;
//}

//void ImageProcess::gc_img(CAM_Image *src, CAM_Image *result)
//{
//    return;
//}
