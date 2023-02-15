#include "autoexposurehandler.h"


AutoExposureHandler::AutoExposureHandler()
{

}

bool AutoExposureHandler::correct(cv::Mat image) {
    int bins = 256;
    int histSize[] = {bins};
    // Set ranges for histogram bins
    float lranges[] = {0, 256};
    const float* ranges[] = {lranges};
    // create matrix for histogram
    cv::Mat hist;
    int channels[] = {0}; // hsv

    cv::calcHist( &image, 1, channels, cv::Mat(), // do not use mask
                hist, 1, histSize, ranges);
    cv::normalize(hist, hist, 1, 0, cv::NORM_L1);

    double percent = hist.at <float> (255.0);

    double mean = cv::mean(image)[0];
    double relMean = mean / mMean;
    double rel = percent / mMaxPercent;

    if (relMean > 1 && rel > 1)
      rel = relMean > rel ? relMean : rel;
    else
        rel = relMean;

    if (!(rel <= mMaxRelCoeff && rel >= mMinRelCoef)) {
        ++mProcessCounter;
        if (mProcessCounter > mMaxFrameCoeff && mDivideCoeff < mDivideCoeff) {
            mDivideCoeff += 2;
            mProcessCounter = 0;
        }
        if (mProcessCounter > mMaxFrameCoeff * 3 && mDivideCoeff == mDivideCoeffMax) {
              mDivideCoeff = mDivideCoeffDefault;
          }
          double neededExposure = params->exposure() / rel;
          double exposureStep = neededExposure - params->exposure();
          double gainStep = (exposureStep / mDivideCoeff) * 15;
          gainStep = gainStep < 1 ? 1 : gainStep;

          params->set_exposure(params->exposure() + exposureStep / divideCoeff);

          if (params->exposure() > lowExp && params->gain() <= params->max_gain_coeff())
          {
              params->set_gain(params->gain() + gainStep);
          }
          else if (params->exposure() < lowExp)
          {
              params->set_gain(params->min_gain_coeff());
          }
          //qDebug()<< neededExposure << exposureStep << gainStep << divideCoeff;

          if (params->exposure() > maxExp)
          {
              params->set_exposure(maxExp);
          }
          if (params->gain() > params->max_gain_coeff())
          {
              params->set_gain(params->max_gain_coeff());
          }

          return true;
      }
      else
      {
          processCounter = 0;
          return false;
      }

}
