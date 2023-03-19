#include <iostream>
#include "cameraqhyccd.h"
#include "objectivecontroller.h"
#include "imageprocess.h"
#include "imageblurmetric.h"
#include "camframe.h"
#include <fstream>

using namespace std;

void cameraExample(int32_t cameraMode);
void objectiveExample();
void findFocus();
void blurDetectionExample();
void testCode();

int main() {
    /// TEST FPS AND WB
    cameraExample(single);
    return 0;
}

void objectiveExample(){
    ObjectiveController myController("COM6");
    double result = myController.getCurrentFocusing();
    cout << result << endl;
    myController.setFocusing(0);
    myController.setFocusing(5000);
    myController.setFocusing(3500);
    myController.setFocusing(2600);
    result = myController.getCurrentFocusing();
    cout << result << endl;
    myController.setFocusing(4300);
    result = myController.getCurrentFocusing();

    cout << result << endl;
}

void cameraExample(int32_t cameraMode){
    CameraQHYCCD* myCamera;
    if(!CameraQHYCCD::initSDK()) {
        cout << "Init SDK fail" << endl;
        return;
    }

    int num = 0;
    num = CameraQHYCCD::searchCamera();
    if(num >= 1) {
        char id[32];
        cout << "Camera found: " << num << endl;
        if(!CameraQHYCCD::getID(0,id)) {
            cout << "ID getting fail" << endl;
            return;
        }
        myCamera = new CameraQHYCCD(id);

        int writeToFileFlag = false;
        int32_t mode = cameraMode;

        cout << "Select camera stream: " << endl << "0.single" << endl << "1.live" << endl;
        cin >> mode;

        if((mode < 0) || (mode > 1))
            cout << "error stream mode" << endl;
        myCamera->connect((StreamMode)mode);

        double min, max, step;

        if (myCamera->getControlMinMaxStep(gain, min, max, step))
            cout << "gain max: " << max << " " << "gain min: " << min  << " " << "gain step: " << step << " " << endl;
        else
            cout << "gain is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(offset, min, max, step))
            cout << "offset max: " << max << " " << "offset min: " << min << " " << "offset step: " << step << " " << endl;
        else
            cout << "offset is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(exposure, min, max, step))
            cout << "exposure max: " << max << " " << "exposure min: " << min << " " << "exposure step: " << step << " " << endl;
        else
            cout << "exposure is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(speed, min, max, step))
            cout << "speed max: " << max << " " << "speed min: " << min << " " << "speed step: " << step << " " << endl;
        else
            cout << "speed is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(transferbit, min, max, step))
            cout << "transferbit max: " << max << " " << "transferbit min: " << min << " " << "transferbit step: " << step << " " << endl;
        else
            cout << "transferbit is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(fps, min, max, step))
            cout << "usbtraffic max: " << max << " " << "usbtraffic min: " << min << " " << "usbtraffic step: " << step << " " << endl;
        else
            cout << "usbtraffic is not available in this camera" << endl;

        cout << "FPS: " << myCamera->getFps() << endl;
        cout << "Exposure: " << myCamera->getExposure() << " ms" << endl;
        cout << "Gain: " << myCamera->getGain() << endl;
        cout << "Bit:  " << myCamera->getImageBitMode() << endl;

        myCamera->setExposure(10);
        myCamera->setGain(30);
        myCamera->setImageBitMode(bit8);

        cout << "Exposure: " << myCamera->getExposure() << " ms" << endl;
        cout << "Gain: " << myCamera->getGain() << endl;
        cout << "Bit:  " << myCamera->getImageBitMode() << endl;

        uint32_t startX = 0, startY = 0, sizeX = 0, sizeY = 0;
        myCamera->getImageSize(startX, startY, sizeX, sizeY);

        cout << "StartX: " << startX << endl;
        cout << "StartY: " << startY << endl;
        cout << "SizeX:  " << sizeX << endl;
        cout << "SizeY:  " << sizeY << endl;

        myCamera->setImageSize(startX, startY, sizeX, sizeY);
        myCamera->getImageSize(startX, startY, sizeX, sizeY);

        cout << "StartX: " << startX << endl;
        cout << "StartY: " << startY << endl;
        cout << "SizeX:  " << sizeX << endl;
        cout << "SizeY:  " << sizeY << endl;

        CamParameters params = myCamera->getCameraParameters();
        uint32_t length = params.mMaximgh * params.mMaximgw * 2;
        CamFrame frame;
        frame.allocateFrame(length);

        if(!params.mIsMono && params.mIsLiveMode){
            cv::namedWindow("Camera image", cv::WINDOW_NORMAL);
            cv::resizeWindow("Camera image", 600, 600);

            cv::namedWindow("WB image", cv::WINDOW_NORMAL);
            cv::resizeWindow("WB image", 600, 600);

            cv::namedWindow("Debayer image", cv::WINDOW_NORMAL);
            cv::resizeWindow("Debayer image", 600, 600);

            cv::namedWindow("GrayScale image", cv::WINDOW_NORMAL);
            cv::resizeWindow("GrayScale image", 600, 600);
        }

        if(!(params.mIsLiveMode)){
            if(myCamera->startSingleCapture()) {
                myCamera->getImage(frame.mWidth, frame.mHeight, frame.mBpp,
                                   frame.mChannels, frame.pData);
                cout << frame.mWidth << " " << frame.mHeight << endl;

                int type = ImageProcess::getOpenCvType((BitMode)frame.mBpp, frame.mChannels);
                cv::Mat camImg(frame.mHeight, frame.mWidth, type, frame.pData);
                cv::imshow("Camera image", camImg);

                if(!params.mIsMono){
                    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                    ImageProcess::debayerImg(camImg, camImg);
                    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                    cv::imshow("Debayer image", camImg);

                    begin = std::chrono::steady_clock::now();
                    ImageProcess::whiteBalanceImg(camImg, camImg);
                    end = std::chrono::steady_clock::now();
                    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                    cv::imshow("White balanced image", camImg);
                }

                if(writeToFileFlag) {
                    ofstream binaryFile ("file2.raw", ios::out | ios::binary);
                    binaryFile.write ((char*)frame.pData, frame.mLength);
                    binaryFile.close();
                    cv::waitKey(0);
                }
            }
        }
        else {
            if(myCamera->startLiveCapture()) {
                bool ready = false;

                for(int i = 0; i < 15; i++) {
                    while(ready == false)
                        ready = myCamera->getImage(frame.mWidth, frame.mHeight, frame.mBpp,
                                                   frame.mChannels, frame.pData);

                    int type = ImageProcess::getOpenCvType((BitMode)frame.mBpp, frame.mChannels);
                    cv::Mat camImg(frame.mHeight, frame.mWidth, type, frame.pData);

                    cv::imshow("Camera image", camImg);
                    cv::waitKey(100);

                    if(!params.mIsMono){
                        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                        ImageProcess::debayerImg(camImg, camImg);
                        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                        cv::imshow("Debayer image", camImg);

                        cv::Mat grayScale;
                        cv::cvtColor(camImg, grayScale, cv::COLOR_BGR2GRAY);
                        cv::imshow("GrayScale image", grayScale);

                        begin = std::chrono::steady_clock::now();
                        ImageProcess::whiteBalanceImg(camImg, camImg);
                        end = std::chrono::steady_clock::now();
                        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                        cv::imshow("WB image", camImg);

                        double sobel = 0,  scharr = 0, laplacian = 0, fft = 0;

                        ImageBlurMetric::getBlurSobel(grayScale, sobel);
                        ImageBlurMetric::getBlurScharr(grayScale, scharr);
                        ImageBlurMetric::getBlurLaplacian(grayScale, laplacian);
                        ImageBlurMetric::getBlurFFT(grayScale, fft);

                        std::cout << "Sobel     " << sobel << endl;
                        std::cout << "Scharr    " << scharr << endl;
                        std::cout << "Laplacian     " << laplacian << endl;
                        std::cout << "FFT " << fft << endl;
                    }

                    ready = false;
                }
                myCamera->stopLiveCapture();
            }
        }

        myCamera->disconnect();
        CameraQHYCCD::ReleaseSDK();
    }
    else
        cout << "Camera found   : " << num << endl;
}

void findFocus(){
    ObjectiveController myController("COM6");
    CameraQHYCCD* myCamera;
    CamFrame myImg;

    cv::namedWindow("Start image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Start image", 600, 600);

    cv::namedWindow("Focused image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Focused image", 600, 600);

    cv::namedWindow("Final image", cv::WINDOW_NORMAL);
    cv::resizeWindow("Final image", 600, 600);

    if(!CameraQHYCCD::initSDK())
        return;

    int num = 0;
    num = CameraQHYCCD::searchCamera();
    if(num < 1)
        return;

    char id[32];
    if(!CameraQHYCCD::getID(0,id))
        return;

    myCamera = new CameraQHYCCD(id);
    myCamera->connect(live);
    myCamera->setExposure(200);
    myCamera->setGain(0);
    myCamera->setImageBitMode(bit8);
    myCamera->setImageSize(700, 1750, 2000, 1500);

    myImg.mLength = myCamera->getImgLength();
    myImg.allocateFrame(myImg.mLength * 2);

    myController.setFocusing(10000);
    if(myCamera->startLiveCapture()) {
        bool ready = false;
        while(ready == false)
            ready = myCamera->getImage(myImg.mWidth, myImg.mHeight, myImg.mBpp,
                                       myImg.mChannels, myImg.pData);
        myCamera->stopLiveCapture();
    }

    int type = ImageProcess::getOpenCvType((BitMode)myImg.mBpp, myImg.mChannels);
    cv::Mat camImg(myImg.mHeight, myImg.mWidth, type, myImg.pData);
    ImageProcess::debayerImg(camImg, camImg);
    cv::imshow("Start image", camImg);

    int step = 200;
    double position = 10000;
    double finish = 0;

    double focusPosition = 0;
    double sharpValue = 0;

    while(position >= finish){
        cout << position << endl;
        myController.setFocusing(position);
        if(myCamera->startLiveCapture()) {
            bool ready = false;
            while(ready == false)
                ready = myCamera->getImage(myImg.mWidth, myImg.mHeight, myImg.mBpp,
                                           myImg.mChannels, myImg.pData);
            myCamera->stopLiveCapture();
        }

        int type = ImageProcess::getOpenCvType((BitMode)myImg.mBpp, myImg.mChannels);
        cv::Mat img(myImg.mHeight, myImg.mWidth, type, myImg.pData);

        cv::Mat grayScale;
        cv::cvtColor(img, grayScale, cv::COLOR_BGR2GRAY);

        ImageProcess::debayerImg(img, img);

        double metric;
        ImageBlurMetric::getBlurFFT(grayScale, metric);
        std::string label = std::to_string(metric);
        cout << metric << endl;

        cv::putText(grayScale, label, cv::Point(30,250), cv::FONT_HERSHEY_PLAIN, 10.0, CV_RGB(255,0,0), 5.0);

        cv::imshow("Focused image", grayScale);
        cv::waitKey(100);

        if(metric >= sharpValue){
            sharpValue = metric;
            focusPosition = position;
        }
        position -= step;
    }

    cout << sharpValue << endl;
    cout << focusPosition << endl;
    cv::waitKey(0);
}

void blurDetectionExample(){
    cv::Mat frame = imread("B:/H4CK3R/CameraQHYCCD/CameraQHYCCD/doge.png", cv::IMREAD_GRAYSCALE);
    cv::Mat frame_1 = imread("B:/H4CK3R/CameraQHYCCD/CameraQHYCCD/doge_10.png", cv::IMREAD_GRAYSCALE);

    cv::namedWindow("Doge", cv::WINDOW_NORMAL);
    cv::resizeWindow("Doge", 600, 600);

    cv::namedWindow("DogeBlur", cv::WINDOW_NORMAL);
    cv::resizeWindow("DogeBlur", 600, 600);

    double metric;
    double metric_1;

    ImageBlurMetric::getBlurLaplacian(frame, metric);
    ImageBlurMetric::getBlurLaplacian(frame_1, metric_1);

    std::string label = std::to_string(metric);
    std::string label_1 = std::to_string(metric_1);

    cv::putText(frame, label, cv::Point(10,15), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1.0);
    cv::imshow("Doge", frame);

    cv::putText(frame_1, label_1, cv::Point(10,15), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1.0);
    cv::imshow("DogeBlur", frame_1);

    cv::waitKey(0);
}

void testCode() {
    return;
}
