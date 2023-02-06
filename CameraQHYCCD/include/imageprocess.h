#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

struct camImage{
    cv::Mat img;

    int time;

    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t bpp = 0;
    uint32_t channels = 0;

    int32_t length;
};


class ImageProcess
{
public:
    ImageProcess() = delete;

    static void wb_img(camImage& src, camImage& result);

    static void debayer_img(camImage& src, camImage& result);

    static void contrast_img(camImage& src, camImage& result);

    static void gc_img(camImage& src, camImage& result);
};

#endif // IMAGEPROCESS_H
