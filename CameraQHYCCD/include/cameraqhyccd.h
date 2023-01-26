#ifndef CAMERAQHYCCD_H
#define CAMERAQHYCCD_H

#include "qhyccd.h"
#include "camstruct.h"
#include "camenums.h"

class CameraQHYCCD
{
private:
    qhyccd_handle *camhandle;
    static bool isSDK_Inited;
    bool connected = false;
    CAM cam;
public:
    CameraQHYCCD(char *id);

    ~CameraQHYCCD();

    static bool initSDK();

    static int searchCamera();

    static bool getID(int num, char* id);

    bool connect(streamMode mode);

    bool getControlMinMaxStep(cameraControls control, double *min,double *max,double *step);

    bool disconnect();

    static bool ReleaseSDK();
};

#endif // CAMERAQHYCCD_H
