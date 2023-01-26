#include "cameraqhyccd.h"
#include <iostream>
#include <exception>

bool CameraQHYCCD::isSDK_Inited = false;

CameraQHYCCD::CameraQHYCCD(char* id) {
    camhandle = OpenQHYCCD(id);
    if(camhandle == NULL)
        throw std::logic_error("Failed to open camera!");
    int ret = GetQHYCCDModel(id,cam.model);
    if(ret != QHYCCD_SUCCESS)
        throw std::logic_error("Failed to read model!");
}

bool CameraQHYCCD::initSDK() {
    if(isSDK_Inited)
        return false;

    int ret = InitQHYCCDResource();
    if(ret == QHYCCD_SUCCESS)
        isSDK_Inited = true;

    return isSDK_Inited;
}

int CameraQHYCCD::searchCamera(){
    int num = 0;
    num = ScanQHYCCD();
    return num;
}

bool CameraQHYCCD::getID(int num, char* id){
    int ret = GetQHYCCDId(num,id);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::connect(streamMode mode) {
    int ret = SetQHYCCDStreamMode(camhandle, mode);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = InitQHYCCD(camhandle);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = IsQHYCCDControlAvailable(camhandle,CAM_COLOR);
    if(ret == BAYER_GB || ret == BAYER_GR || ret == BAYER_BG ||ret == BAYER_RG)
        cam.isMono = false;
    else
        cam.isMono = true;

    uint32_t imagew, imageh, bpp;
    double chipw, chiph, pixelw, pixelh;

    ret = GetQHYCCDChipInfo(camhandle,&chipw,&chiph,&imagew,&imageh,&pixelw,&pixelh,&bpp);
    if(ret == QHYCCD_SUCCESS){
        cam.chipw = chipw;
        cam.chiph = chiph;
        cam.pixelw = pixelw;
        cam.pixelh = pixelh;
        cam.maximgw = imagew;
        cam.maximgh = imageh;
    }
    else
        return false;

    ret = SetQHYCCDBinMode(camhandle,1,1);
    if(ret != QHYCCD_SUCCESS)
        return false;

    return true;
}

bool CameraQHYCCD::getControlMinMaxStep(cameraControls control, double *min,double *max,double *step) {
    int ret = IsQHYCCDControlAvailable(camhandle,(CONTROL_ID)control);
    if(ret != QHYCCD_SUCCESS)
        return false;
    ret = GetQHYCCDParamMinMaxStep(camhandle,(CONTROL_ID)control,min,max,step);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::disconnect() {
    int ret = CloseQHYCCD(camhandle);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::ReleaseSDK() {
    int ret = ReleaseQHYCCDResource();
    isSDK_Inited = false;
    return (ret == QHYCCD_SUCCESS);
}




