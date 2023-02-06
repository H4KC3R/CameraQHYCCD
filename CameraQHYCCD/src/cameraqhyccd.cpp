#include "cameraqhyccd.h"
#include <iostream>
#include <exception>

bool CameraQHYCCD::m_isSdkInited = false;

CameraQHYCCD::CameraQHYCCD(char* id) {
    if(id == NULL)
        throw std::logic_error("Id address is null");
    m_camhandle = OpenQHYCCD(id);
    if(m_camhandle == NULL)
        throw std::logic_error("Failed to open camera!");
    uint32_t ret = GetQHYCCDModel(id, params.m_model);
    if(ret != QHYCCD_SUCCESS)
        throw std::logic_error("Failed to read model!");
}

bool CameraQHYCCD::initSDK() {
    if(m_isSdkInited)
        return false;

    int ret = InitQHYCCDResource();
    if(ret == QHYCCD_SUCCESS)
        m_isSdkInited = true;

    return m_isSdkInited;
}

int32_t CameraQHYCCD::searchCamera(){
    int32_t num = 0;
    num = ScanQHYCCD();
    return num;
}

bool CameraQHYCCD::getID(int32_t num, char* id){
    uint32_t ret = GetQHYCCDId(num, id);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::connect(streamMode mode) {
    uint32_t ret = SetQHYCCDStreamMode(m_camhandle, mode);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = InitQHYCCD(m_camhandle);
    if(ret != QHYCCD_SUCCESS)
        return false;

    ret = IsQHYCCDControlAvailable(m_camhandle, CAM_COLOR);
    if(ret == BAYER_GB || ret == BAYER_GR || ret == BAYER_BG ||ret == BAYER_RG)
        params.m_isMono = false;
    else
        params.m_isMono = true;

    uint32_t imagew, imageh, bpp;
    double chipw, chiph, pixelw, pixelh;

    ret = GetQHYCCDChipInfo(m_camhandle, &chipw, &chiph, &imagew, &imageh, &pixelw, &pixelh, &bpp);
    if(ret == QHYCCD_SUCCESS){
        params.m_chipw = chipw;
        params.m_chiph = chiph;
        params.m_pixelw = pixelw;
        params.m_pixelh = pixelh;
        params.m_maximgw = imagew;
        params.m_maximgh = imageh;
    }
    else
        return false;

    params.m_wbin = 1;
    params.m_hbin = 1;
    ret = SetQHYCCDBinMode(m_camhandle, params.m_wbin, params.m_hbin);
    if(ret != QHYCCD_SUCCESS)
        return false;

    params.m_isLiveMode = (mode == live);
    params.m_isConnected = true;

    if(!params.m_isMono)
        SetQHYCCDDebayerOnOff(m_camhandle,false);

    return true;
}

bool CameraQHYCCD::getControlMinMaxStep(cameraControls control, double& min, double& max, double& step) {
    uint32_t ret = IsQHYCCDControlAvailable(m_camhandle, (CONTROL_ID)control);
    if(ret != QHYCCD_SUCCESS)
        return false;
    ret = GetQHYCCDParamMinMaxStep(m_camhandle, (CONTROL_ID)control, &min, &max, &step);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageSize(uint32_t x, uint32_t y, uint32_t xsize, uint32_t ysize) {
    uint32_t ret = SetQHYCCDResolution(m_camhandle, x, y, xsize/params.m_wbin, ysize/params.m_hbin);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::getImageSize(uint32_t& startX, uint32_t& startY, uint32_t& sizeX, uint32_t& sizeY) {
    uint32_t ret = GetQHYCCDEffectiveArea(m_camhandle, &startX, &startY, &sizeX, &sizeY);
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::setImageBitMode(bitMode bit){
    if(bit == bit8) {
        uint32_t ret = IsQHYCCDControlAvailable(m_camhandle, CAM_8BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(m_camhandle, 8);
        return (ret == QHYCCD_SUCCESS);
    }
    else {
        uint32_t ret = IsQHYCCDControlAvailable(m_camhandle, CAM_16BITS);
        if(ret == QHYCCD_SUCCESS)
            ret = SetQHYCCDBitsMode(m_camhandle, 16);
        return (ret == QHYCCD_SUCCESS);
    }
}

uint32_t CameraQHYCCD::getImageBitMode(){
    return ((uint32_t)GetQHYCCDParam(m_camhandle, CONTROL_TRANSFERBIT));
}

bool CameraQHYCCD::setGain(double value) {
    uint32_t ret = SetQHYCCDParam(m_camhandle, CONTROL_GAIN, value);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getGain(void) {
    return GetQHYCCDParam(m_camhandle, CONTROL_GAIN);
}

bool CameraQHYCCD::setExposure(double ms) {
    uint32_t ret = SetQHYCCDParam(m_camhandle, CONTROL_EXPOSURE, ms * 1000.0);
    return (ret == QHYCCD_SUCCESS);
}

double CameraQHYCCD::getExposure(void) {
    // return in ms
    return (GetQHYCCDParam(m_camhandle, CONTROL_EXPOSURE) / 1000);
}

uint32_t CameraQHYCCD::getImgLength() {
    return GetQHYCCDMemLength(m_camhandle);
}

bool CameraQHYCCD::startSingleCapture() {
    if(params.m_isLiveMode)
        return false;

    params.m_status = singleCapture;
    uint32_t ret = ExpQHYCCDSingleFrame(m_camhandle);
    if(ret == QHYCCD_SUCCESS)
        return true;
    else {
        params.m_status = failed;
        return false;
    }
}

bool CameraQHYCCD::stopSingleCapture() {
    uint32_t ret = CancelQHYCCDExposing(m_camhandle);
    if(ret == QHYCCD_SUCCESS) {
        params.m_status = idle;
        return true;
    }
    else {
        params.m_status = failed;
        return false;
    }
}

bool CameraQHYCCD::startLiveCapture() {
    uint32_t ret = BeginQHYCCDLive(m_camhandle);
    if(ret == QHYCCD_SUCCESS) {
        params.m_status = liveCapture;
        return true;
    }
    else {
        params.m_status = failed;
        return false;
    }
}

bool CameraQHYCCD::stopLiveCapture() {
    uint32_t ret = StopQHYCCDLive(m_camhandle);
    if(ret == QHYCCD_SUCCESS) {
        params.m_status = idle;
        return true;
    }
    else {
        params.m_status = failed;
        return false;
    }
}

bool CameraQHYCCD::getImage(uint32_t& w, uint32_t& h, uint32_t& bpp, uint32_t& channels, uint8_t* imgdata) {
    uint32_t ret = 0;
    if(params.m_status == singleCapture) {
        ret = GetQHYCCDSingleFrame(m_camhandle, &w, &h, &bpp, &channels, imgdata);
        params.m_status = idle;
    }
    else if(params.m_status == liveCapture)
        ret = GetQHYCCDLiveFrame(m_camhandle, &w, &h, &bpp, &channels, imgdata);
    else
        return false;

    return (ret == QHYCCD_SUCCESS);
}

CameraQHYCCD::~CameraQHYCCD() {
    if(params.m_isConnected)
        disconnect();
}

bool CameraQHYCCD::disconnect() {
    if(params.m_status == liveCapture)
        stopLiveCapture();
    else if(params.m_status == singleCapture)
        stopSingleCapture();
    uint32_t ret = CloseQHYCCD(m_camhandle);
    params.m_isConnected = false;
    return (ret == QHYCCD_SUCCESS);
}

bool CameraQHYCCD::ReleaseSDK() {
    uint32_t ret = ReleaseQHYCCDResource();
    m_isSdkInited = false;
    return (ret == QHYCCD_SUCCESS);
}



