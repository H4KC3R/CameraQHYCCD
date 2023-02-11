#include "imageprocess.h"

int ImageProcess::getOpenCvType(BitMode bpp, int channels) {
    if(bpp == bit8)
        return CV_MAKE_TYPE(CV_8U, channels);
    else
        return CV_MAKE_TYPE(CV_16U, channels);

}

CamImage ImageProcess::whiteBalanceImg(const CamImage& src) {
    CamImage result;
    result = src;
    cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB();
    wb->balanceWhite(src.img, result.img);
    return result;
}

CamImage ImageProcess::debayerImg(const CamImage& src) {
    CamImage result;
    result = src;
    cv::cvtColor(src.img, result.img, cv::COLOR_BayerRG2BGR);
    return result;
}

//void ImageProcess::contrast_img(CAM_Image *src, CAM_Image *result) {
//    return;
//}

//void ImageProcess::gc_img(CAM_Image *src, CAM_Image *result)
//{
//    return;
//}
