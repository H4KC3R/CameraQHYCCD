#include <iostream>
#include "cameraqhyccd.h"
#include "imageprocess.h"
#include <fstream>

using namespace std;

// qhyccd camera usage sample
int main()
{
    CameraQHYCCD* myCamera;
    if(!CameraQHYCCD::initSDK()) {
        cout << "Init SDK fail" << endl;
        return -1;
    }

    int num = 0;
    num = CameraQHYCCD::searchCamera();
    if(num >= 1) {
        char id[32];
        cout << "Camera found: " << num << endl;
        if(!CameraQHYCCD::getID(0,id)) {
            cout << "ID getting fail" << endl;
            return -1;
        }
        myCamera = new CameraQHYCCD(id);
        int mode = live;

        cout << "Select camera stream: " << endl << "0.single" << endl << "1.live" << endl;
        cin >> mode;

        if((mode < 0) || (mode > 1))
            cout << "error stream mode" << endl;
        myCamera->connect((streamMode)mode);

        double min, max, step;

        if (myCamera->getControlMinMaxStep(gain, &min, &max, &step))
            cout << "gain max: " << max << " " << "gain min: " << min  << " " << "gain step: " << step << " " << endl;
        else
            cout << "gain is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(offset, &min, &max, &step))
            cout << "offset max: " << max << " " << "offset min: " << min << " " << "offset step: " << step << " " << endl;
        else
            cout << "offset is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(exposure, &min, &max, &step))
            cout << "exposure max: " << max << " " << "exposure min: " << min << " " << "exposure step: " << step << " " << endl;
        else
            cout << "exposure is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(speed, &min, &max, &step))
            cout << "speed max: " << max << " " << "speed min: " << min << " " << "speed step: " << step << " " << endl;
        else
            cout << "speed is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(transferbit, &min, &max, &step))
            cout << "transferbit max: " << max << " " << "transferbit min: " << min << " " << "transferbit step: " << step << " " << endl;
        else
            cout << "transferbit is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(usbtraffic, &min, &max, &step))
            cout << "usbtraffic max: " << max << " " << "usbtraffic min: " << min << " " << "usbtraffic step: " << step << " " << endl;
        else
            cout << "usbtraffic is not available in this camera" << endl;

        cout << "Exposure: " << myCamera->getExposure() << " ms" << endl;
        cout << "Gain: " << myCamera->getGain() << endl;
        cout << "Bit:  " << myCamera->getImageBitMode() << endl;

        myCamera->setExposure(215);
        myCamera->setGain(15);
        myCamera->setImageBitMode(bit8);

        cout << "Exposure: " << myCamera->getExposure() << " ms" << endl;
        cout << "Gain: " << myCamera->getGain() << endl;
        cout << "Bit:  " << myCamera->getImageBitMode() << endl;

        uint32_t startX = 0, startY = 0, sizeX = 0, sizeY = 0;
        myCamera->getImageSize(&startX, &startY, &sizeX, &sizeY);

        cout << "StartX: " << startX << endl;
        cout << "StartY: " << startY << endl;
        cout << "SizeX:  " << sizeX << endl;
        cout << "SizeY:  " << sizeY << endl;

        myCamera->setImageSize(startX, startY, sizeX, sizeY);
        myCamera->getImageSize(&startX, &startY, &sizeX, &sizeY);

        cout << "StartX: " << startX << endl;
        cout << "StartY: " << startY << endl;
        cout << "SizeX:  " << sizeX << endl;
        cout << "SizeY:  " << sizeY << endl;

        CAM_Image myImg;
        CAM_Image debImg;

        if(!(myCamera->params.isLiveMode)){
            if(myCamera->startSingleCapture()) {
                myImg.length = myCamera->getImgLength();
                uint8_t* data = new uint8_t[myImg.length * 2];
                myCamera->getImage(&myImg.w, &myImg.h, &myImg.bpp, &myImg.channels, data);
                cout << myImg.bpp << " " << myImg.channels << endl;
                int type = CV_MAKE_TYPE(CV_8U, myImg.channels);
                myImg.img = cv::Mat(myImg.h, myImg.w, type, data);
                ImageProcess::debayer_img(&myImg, &debImg);

                cv::namedWindow("Camera image", cv::WINDOW_NORMAL);
                cv::resizeWindow("Camera image", 1200, 1200);
                cv::namedWindow("Debayer image", cv::WINDOW_NORMAL);
                cv::resizeWindow("Debayer image", 1200, 1200);

                cv::imshow("Camera image", myImg.img);
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                cv::imshow("Debayer image", debImg.img);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                cv::waitKey(0);
                delete[] data;
            }
        }
        else {
            if(myCamera->startLiveCapture()) {
                myImg.length = myCamera->getImgLength();
                uint8_t* data = new uint8_t[myImg.length];
                bool ready = false;
                while(ready == false)
                    ready = myCamera->getImage(&myImg.w, &myImg.h, &myImg.bpp, &myImg.channels, data);
                myCamera->stopLiveCapture();
                cout << myImg.bpp << " " << myImg.channels << endl;
                int type = CV_MAKE_TYPE(CV_8U, myImg.channels);
                myImg.img = cv::Mat(myImg.h, myImg.w, type, data);
                ImageProcess::debayer_img(&myImg, &debImg);

                cv::namedWindow("Camera image", cv::WINDOW_NORMAL);
                cv::resizeWindow("Camera image", 1200, 1200);
                cv::namedWindow("Debayer image", cv::WINDOW_NORMAL);
                cv::resizeWindow("Debayer image", 1200, 1200);

                cv::imshow("Camera image", myImg.img);
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                cv::imshow("Debayer image", debImg.img);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

                ostringstream filename;
                filename << "Frame.raw";
                std::ofstream outfile (filename.str().c_str(), ios::out | ios::binary);
                outfile.write ((char*)(data), myImg.length);  // In byte so frame.total() should be enough ?
                outfile.close();

                cv::waitKey(0);
                delete[] data;
            }
        }



        myCamera->disconnect();
        CameraQHYCCD::ReleaseSDK();
    }
    else
        cout << "Camera found   : " << num << endl;
    system("pause");
    return 0;
}
