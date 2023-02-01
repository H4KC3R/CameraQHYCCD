#include "cameraqhyccd.h"
#include <iostream>
#include <exception>

bool CameraQHYCCD::isSDK_Inited = false;

CameraQHYCCD::CameraQHYCCD(char* id) {
    if(id == NULL)
        throw std::logic_error("Id address is null");
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

    ret = IsQHYCCDControlAvailable(camhandle, CAM_COLOR);
    if(ret == BAYER_GB || ret == BAYER_GR || ret == BAYER_BG ||ret == BAYER_RG)
        cam.isMono = false;
    else
        cam.isMono = true;

    uint32_t imagew, imageh, bpp;
    double chipw, chiph, pixelw, pixelh;

    ret = GetQHYCCDChipInfo(camhandle, &chipw, &chiph, &imagew, &imageh, &pixelw, &pixelh, &bpp);
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

    cam.wbin = 1;
    cam.hbin = 1;
    ret = SetQHYCCDBinMode(camhandle, cam.wbin, cam.hbin);
    if(ret != QHYCCD_SUCCESS)
        return false;

    cam.isLiveMode = (mode == live);
    cam.isConnected = true;

    img.length = GetQHYCCDMemLength(camhandle);
    img.ImgData = new unsigned char[img.length * 2]();

    return true;
}

bool CameraQHYCCD::getControlMinMaxStep(cameraControls control, double *min, double *max,double *step) {
    int ret = IsQHYCCDControlAvailable(camhandle, (CONTROL_ID)control);
    if(ret != QHYCCD_SUCCESS)
        return false;
    ret = GetQHYCCDParamMinMaxStep(camhandle, (CONTROL_ID)control, min, max, step);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageSize(uint32_t x, uint32_t y, uint32_t xsize, uint32_t ysize) {
    int ret = SetQHYCCDResolution(camhandle, x, y, xsize/cam.wbin, ysize/cam.hbin);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::getImageSize(uint32_t* startX, uint32_t* startY, uint32_t* sizeX, uint32_t* sizeY) {
    int ret = GetQHYCCDEffectiveArea(camhandle, startX, startY, sizeX, sizeY);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageBitMode(bitMode bit){
    if(bit == bit8) {
        int ret = IsQHYCCDControlAvailable(camhandle, CAM_8BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(camhandle, 8);
        return (ret == QHYCCD_SUCCESS);
    }
    else {
        int ret = IsQHYCCDControlAvailable(camhandle, CAM_16BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(camhandle, 16);
        return (ret == QHYCCD_SUCCESS);
    }
}

uint32_t CameraQHYCCD::getImageBitMode(){
    return ((uint32_t)GetQHYCCDParam(camhandle, CONTROL_TRANSFERBIT));
}

bool CameraQHYCCD::setGain(double value) {
    int ret = SetQHYCCDParam(camhandle, CONTROL_GAIN, value);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getGain(void) {
    return GetQHYCCDParam(camhandle, CONTROL_GAIN);
}

bool CameraQHYCCD::setExposure(double ms) {
    int ret = SetQHYCCDParam(camhandle, CONTROL_EXPOSURE, ms * 1000.0);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getExposure(void) {
    // return in ms
    return (GetQHYCCDParam(camhandle, CONTROL_EXPOSURE) / 1000);
}

bool CameraQHYCCD::startSingleCapture() {
    if(cam.isLiveMode)
        return false;

    cam.status = singleCapture;
    int ret = ExpQHYCCDSingleFrame(camhandle);

    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::stopSingleCapture() {

}

bool CameraQHYCCD::startLiveCapture() {
    int ret = BeginQHYCCDLive(camhandle);
}

bool CameraQHYCCD::stopLiveCapture() {
    int ret = StopQHYCCDLive(camhandle);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::getImage() {
    int ret = 0;
    if(cam.status == singleCapture) {
        ret = GetQHYCCDSingleFrame(camhandle, &img.w, &img.h, &img.bpp, &img.channels, img.ImgData);
        cam.status = idle;
    }
    else if(cam.status == liveCapture)
        ret = GetQHYCCDLiveFrame(camhandle, &img.w, &img.h, &img.bpp, &img.channels, img.ImgData);
    else
        return false;

    return (ret == QHYCCD_SUCCESS);
}

CameraQHYCCD::~CameraQHYCCD() {

    if(img.ImgData != NULL)
        delete img.ImgData;

    if(cam.isConnected)
        disconnect();
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



