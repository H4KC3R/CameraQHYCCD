#include "cameraqhyccd.h"
#include <iostream>
#include <exception>

bool CameraQHYCCD::mIsSdkInited = false;

CameraQHYCCD::CameraQHYCCD(std::string id) {
    if(id.empty())
        throw std::logic_error("Id address is null");

    char camId[id.length() + 1];
    strcpy(camId, id.c_str());
    pCamhandle = OpenQHYCCD(camId);
    if(pCamhandle == NULL)
        throw std::logic_error("Failed to open camera!");
    uint32_t ret = GetQHYCCDModel(camId, mParams.model);
    if(ret != QHYCCD_SUCCESS)
        throw std::logic_error("Failed to read model!");
}

bool CameraQHYCCD::initSDK() {
    if(mIsSdkInited)
        return false;

    int ret = InitQHYCCDResource();
    if(ret == QHYCCD_SUCCESS)
        mIsSdkInited = true;

    return mIsSdkInited;
}

int32_t CameraQHYCCD::searchCamera(){
    int32_t num = 0;
    num = ScanQHYCCD();
    return num;
}

std::string CameraQHYCCD::getID(int32_t num){
    char id[40];
    std::string camId;

    uint32_t ret = GetQHYCCDId(num, id);
    if(ret == QHYCCD_SUCCESS)
        camId = id;
    else
        camId = "";

    return camId;
}

std::string CameraQHYCCD::getModel(char *id) {
    char model[20];
    std::string camModel;
    int ret = GetQHYCCDModel(id, model);
    if(ret == QHYCCD_SUCCESS)
        camModel = model;
    else
        camModel = "";
    return camModel;

}

bool CameraQHYCCD::connect(StreamMode mode) {
    uint32_t ret = SetQHYCCDStreamMode(pCamhandle, mode);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = InitQHYCCD(pCamhandle);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = IsQHYCCDControlAvailable(pCamhandle, CAM_COLOR);
    if(ret == BAYER_GB || ret == BAYER_GR || ret == BAYER_BG ||ret == BAYER_RG)
        mParams.isMono = false;
    else
        mParams.isMono = true;

    uint32_t imagew, imageh, bpp;
    double chipw, chiph, pixelw, pixelh;

    ret = GetQHYCCDChipInfo(pCamhandle, &chipw, &chiph, &imagew, &imageh, &pixelw, &pixelh, &bpp);
    if(ret == QHYCCD_SUCCESS){
        mParams.chipw = chipw;
        mParams.chiph = chiph;
        mParams.pixelw = pixelw;
        mParams.pixelh = pixelh;
        mParams.maximgw = imagew;
        mParams.maximgh = imageh;
    }
    else
        return false;

    mParams.wbin = 1;
    mParams.hbin = 1;
    ret = SetQHYCCDBinMode(pCamhandle, mParams.wbin, mParams.hbin);
    if(ret != QHYCCD_SUCCESS)
        return false;

    mParams.isLiveMode = (mode == live);
    mParams.isConnected = true;

    if(!mParams.isMono)
        SetQHYCCDDebayerOnOff(pCamhandle,false);

    return true;
}

bool CameraQHYCCD::getControlMinMaxStep(CameraControls control, double& min, double& max, double& step) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = IsQHYCCDControlAvailable(pCamhandle, (CONTROL_ID)control);
    if(ret != QHYCCD_SUCCESS)
        return false;
    ret = GetQHYCCDParamMinMaxStep(pCamhandle, (CONTROL_ID)control, &min, &max, &step);
    if(control == exposure){
        min /= 1000;
        max /= 1000;
    }
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageSize(uint32_t x, uint32_t y, uint32_t xsize, uint32_t ysize) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = SetQHYCCDResolution(pCamhandle, x, y, xsize/mParams.wbin, ysize/mParams.hbin);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::getImageSize(uint32_t& startX, uint32_t& startY, uint32_t& sizeX, uint32_t& sizeY) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = GetQHYCCDEffectiveArea(pCamhandle, &startX, &startY, &sizeX, &sizeY);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageBitMode(BitMode bit){
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    if(bit == bit8) {
        uint32_t ret = IsQHYCCDControlAvailable(pCamhandle, CAM_8BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(pCamhandle, 8);
        return (ret == QHYCCD_SUCCESS);
    }
    else if(bit == bit16){
        uint32_t ret = IsQHYCCDControlAvailable(pCamhandle, CAM_16BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(pCamhandle, 16);
        return (ret == QHYCCD_SUCCESS);
    }
    else
        return false;
}

uint32_t CameraQHYCCD::getImageBitMode(){
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    return ((uint32_t)GetQHYCCDParam(pCamhandle, CONTROL_TRANSFERBIT));
}

bool CameraQHYCCD::setUsbTraffic(double value) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = SetQHYCCDParam(pCamhandle, CONTROL_USBTRAFFIC, value);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getUsbTraffic() {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    return GetQHYCCDParam(pCamhandle, CONTROL_USBTRAFFIC);
}

bool CameraQHYCCD::setGain(double gain) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = SetQHYCCDParam(pCamhandle, CONTROL_GAIN, gain);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getGain(void) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    return GetQHYCCDParam(pCamhandle, CONTROL_GAIN);
}

bool CameraQHYCCD::setExposure(double ms) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = SetQHYCCDParam(pCamhandle, CONTROL_EXPOSURE, ms * 1000.0);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getExposure(void) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    // return in ms
    return (GetQHYCCDParam(pCamhandle, CONTROL_EXPOSURE) / 1000);
}

uint32_t CameraQHYCCD::getImgLength() {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    return GetQHYCCDMemLength(pCamhandle);
}

CamParameters CameraQHYCCD::getCameraParameters() {
    return mParams;
}

bool CameraQHYCCD::startSingleCapture() {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    if(mParams.isLiveMode)
        return false;

    uint32_t ret = ExpQHYCCDSingleFrame(pCamhandle);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::stopSingleCapture() {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = CancelQHYCCDExposing(pCamhandle);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::startLiveCapture() {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = BeginQHYCCDLive(pCamhandle);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::stopLiveCapture() {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = StopQHYCCDLive(pCamhandle);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::getImage(uint32_t& w, uint32_t& h, uint32_t& bpp, uint32_t& channels, uint8_t* imgdata) {
    if(!mParams.isConnected)
        throw std::logic_error("Camera is not connected");

    uint32_t ret = 0;
    if(mParams.isLiveMode)
        ret = GetQHYCCDLiveFrame(pCamhandle, &w, &h, &bpp, &channels, imgdata);
    else
        ret = GetQHYCCDSingleFrame(pCamhandle, &w, &h, &bpp, &channels, imgdata);

    return (ret == QHYCCD_SUCCESS);
}

CameraQHYCCD::~CameraQHYCCD() {
    if(mParams.isConnected)
        disconnect();
}

bool CameraQHYCCD::disconnect() {
    if(mParams.isLiveMode)
        stopLiveCapture();
    else
        stopSingleCapture();

    uint32_t ret = CloseQHYCCD(pCamhandle);
    mParams.isConnected = false;
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::ReleaseSDK() {
    uint32_t ret = ReleaseQHYCCDResource();
    mIsSdkInited = false;
    return (ret == QHYCCD_SUCCESS);
}



