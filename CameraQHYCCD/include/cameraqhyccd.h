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

    CAM_Image img;

public:
    CameraQHYCCD(char *id);

    ~CameraQHYCCD();

    static bool initSDK();

    static int searchCamera();

    static bool getID(int num, char* id);

    bool connect(streamMode mode);

    bool getControlMinMaxStep(cameraControls control, double *min, double *max, double *step);

    bool setImageSize(uint32_t x, uint32_t y, uint32_t xsize, uint32_t ysize);

    bool getImageSize(uint32_t* startX, uint32_t* startY, uint32_t* sizeX, uint32_t* sizeY);

    bool setImageBitMode(bitMode bit);

    uint32_t getImageBitMode();

    bool setGain(double value);

    double getGain(void);

    bool setExposure(double ms);

    double getExposure(void);

    bool startSingleCapture();

    bool stopSingleCapture();

    bool startLiveCapture();

    bool stopLiveCapture();

    bool getImage();

    bool disconnect();

    static bool ReleaseSDK();
};

#endif // CAMERAQHYCCD_H
