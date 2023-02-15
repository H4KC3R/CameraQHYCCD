#ifndef AUTOEXPOSUREHANDLER_H
#define AUTOEXPOSUREHANDLER_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xphoto/white_balance.hpp>
#include <opencv2/imgproc.hpp>


class AutoExposureHandler
{
private:
    int mProcessCounter;
    int mMaxFrameCoeff;
    int mDivideCoeff;
    int mDivideCoeffMax;
    int mDivideCoeffDefault

    double mGain;
    double mExposure;
    double mMinGainCoeff;
    double mMaxGainCoeff;
    double mMaxPercent;
    double mMinRelCoef;
    double mMaxRelCoeff;
    double mMean;

public:

    AutoExposureHandler();

    bool correct(cv::Mat image);

};

#endif // AUTOEXPOSUREHANDLER_H
