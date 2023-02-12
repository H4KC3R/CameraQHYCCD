#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include "camenums.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>

struct CamImage{
    cv::Mat img;

    int time;

    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t bpp = 0;
    uint32_t channels = 0;

    int32_t length;

    CamImage& operator=(CamImage other){
        if (this == &other)
            return *this;

        img = other.img.clone();
        time = other.time;

        w = other.w;
        h = other.h;
        bpp = other.bpp;
        channels = other.channels;

        length = other.length;

        return *this;
    }
};


class ImageProcess
{   
public:
    ImageProcess() = delete;

    static int getOpenCvType(BitMode bpp, int channels);

    static CamImage whiteBalanceImg(const CamImage& src);

    static CamImage debayerImg(const CamImage& src);

    static CamImage contrastImg(const CamImage& src, double coeff);

    static CamImage gammaContrastImg(const CamImage& src, double kGamma);
};

#endif // IMAGEPROCESS_H
