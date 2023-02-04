#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <opencv2/xphoto/white_balance.hpp>

struct CAM_Image{
    cv::Mat *ImgData;
    int length;

    uint32_t w;
    uint32_t h;

    uint32_t bpp;
    uint32_t channels;
};

class ImageProcess
{
public:
    ImageProcess() = delete;

    static void wb_img(CAM_Image* src, CAM_Image* result);

    static void debayer_img(CAM_Image* src, CAM_Image* result);

    static void contrast_img(CAM_Image* src, CAM_Image* result);

    static void gc_img(CAM_Image* src, CAM_Image* result);
};

#endif // IMAGEPROCESS_H
