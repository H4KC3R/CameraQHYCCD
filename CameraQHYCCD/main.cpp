#include <iostream>
#include "cameraqhyccd.h"

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
        myCamera->connect(single);

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

        if (myCamera->getControlMinMaxStep(mechanicalshutter, &min, &max, &step))
            cout << "mechanicalshutter max: " << max << " " << "mechanicalshutter min: " << min << " " << "mechanicalshutter step: " << step << " " << endl;
        else
            cout << "mechanicalshutter is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(bit8_depth, &min, &max, &step))
            cout << "bit8_depth max: " << max << " " << "bit8_depth min: " << min << " " << "bit8_depth step: " << step << " " << endl;
        else
            cout << "bit8_depth is not available in this camera" << endl;

        if (myCamera->getControlMinMaxStep(bit16_depth, &min, &max, &step))
            cout << "bit16_depth max: " << max << " " << "bit16_depth min: " << min << " " << "bit16_depth step: " << step << " " << endl;
        else
            cout << "bit16_depth is not available in this camera" << endl;

        myCamera->disconnect();
        CameraQHYCCD::ReleaseSDK();
    }
    else
        cout << "Camera found   : " << num << endl;
    system("pause");
    return 0;
}
