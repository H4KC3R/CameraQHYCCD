#include <iostream>
#include "cameraqhyccd.h"
#include "objectivecontroller.h"
#include "imageprocess.h"
#include "imageblurmetric.h"
#include <fstream>

using namespace std;

void cameraExample();
void objectiveExample();

int main() {
    cameraExample();
    return 0;
}

void objectiveExample(){
    ObjectiveController myController("COM7");
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

void cameraExample(){
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
        int32_t mode = live;

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

        if (myCamera->getControlMinMaxStep(usbtraffic, min, max, step))
            cout << "usbtraffic max: " << max << " " << "usbtraffic min: " << min << " " << "usbtraffic step: " << step << " " << endl;
        else
            cout << "usbtraffic is not available in this camera" << endl;

        cout << "Exposure: " << myCamera->getExposure() << " ms" << endl;
        cout << "Gain: " << myCamera->getGain() << endl;
        cout << "Bit:  " << myCamera->getImageBitMode() << endl;

        myCamera->setExposure(215);
        myCamera->setGain(0);
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

        CamImage myImg;

        if(!(myCamera->params.mIsLiveMode)){
            if(myCamera->startSingleCapture()) {
                myImg.length = myCamera->getImgLength();
                uint8_t* data = new uint8_t[myImg.length * 2];
                myCamera->getImage(myImg.w, myImg.h, myImg.bpp, myImg.channels, data);
                cout << myImg.bpp << " " << myImg.channels << endl;
                int type = ImageProcess::getOpenCvType((BitMode)myImg.bpp, myImg.channels);
                myImg.img = cv::Mat(myImg.h, myImg.w, type, data);

                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                CamImage debImg = ImageProcess::debayerImg(myImg);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                cv::imshow("Debayer image", debImg.img);

                begin = std::chrono::steady_clock::now();
                CamImage wbImg = ImageProcess::whiteBalanceImg(debImg);
                end = std::chrono::steady_clock::now();
                std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                cv::imshow("White balanced image", wbImg.img);

                cv::waitKey(0);
                delete[] data;
            }
        }
        else {
            if(myCamera->startLiveCapture()) {
                myImg.length = myCamera->getImgLength();
                uint8_t* data = new uint8_t[myImg.length * 2];
                bool ready = false;
                while(ready == false)
                    ready = myCamera->getImage(myImg.w, myImg.h, myImg.bpp, myImg.channels, data);
                myCamera->stopLiveCapture();

                cout << myImg.bpp << " " << myImg.channels << endl;
                int type = ImageProcess::getOpenCvType((BitMode)myImg.bpp, myImg.channels);
                cout << CV_8UC1 << endl;
                cout << type << endl;
                myImg.img = cv::Mat(myImg.h, myImg.w, type, data);

                cv::namedWindow("Camera image", cv::WINDOW_NORMAL);
                cv::resizeWindow("Camera image", 600, 600);

                cv::namedWindow("WB image", cv::WINDOW_NORMAL);
                cv::resizeWindow("WB image", 600, 600);

                cv::namedWindow("Debayer image", cv::WINDOW_NORMAL);
                cv::resizeWindow("Debayer image", 600, 600);

                cv::imshow("Camera image", myImg.img);

                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                CamImage debImg = ImageProcess::debayerImg(myImg);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                cv::imshow("Debayer image", debImg.img);

                begin = std::chrono::steady_clock::now();
                CamImage wbImg = ImageProcess::whiteBalanceImg(debImg);
                end = std::chrono::steady_clock::now();
                std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                cv::imshow("WB image", wbImg.img);

                double sobel = ImageBlurMetric::getBlurSobel(myImg.img);
                double scharr = ImageBlurMetric::getBlurScharr(myImg.img);
                double laplacian = ImageBlurMetric::getBlurLaplacian(myImg.img);
                double fft = ImageBlurMetric::getBlurFFT(myImg.img);

                std::cout << "Sobel     " << sobel << endl;
                std::cout << "Scharr    " << scharr << endl;
                std::cout << "Laplacian     " << laplacian << endl;
                std::cout << "FFT " << fft << endl;

                //ofstream binaryFile ("file.raw", ios::out | ios::binary);
                //binaryFile.write ((char*)data, myImg.length);
                //binaryFile.close();

                cv::waitKey(0);
                delete[] data;
            }
        }

        myCamera->disconnect();
        CameraQHYCCD::ReleaseSDK();
    }
    else
        cout << "Camera found   : " << num << endl;
}
