#include <stdint.h>
#include "camenums.h"

#ifndef CAMSTRUCT_H
#define CAMSTRUCT_H

struct CAM
{
    char model[12];         // camera model
    bool isMono;            // mono or color
    bool isLiveMode;        // camera mode
    bool isConnected = false;

    cameraStatus status = idle;

    double chipw;           // chip size width (in mm)
    double chiph;           // chip size height (in mm)
    double pixelw;          // chip pixel size width (in um)
    double pixelh;          // chip pixel size height (in um)
    uint32_t maximgw;       // maximum chip output image width
    uint32_t maximgh;       // maximum chip output image height
    uint32_t bpp;           // chip pixel depth

    uint8_t wbin;
    uint8_t hbin;
};

struct CAM_Image{
    unsigned char *ImgData;
    int length;

    uint32_t w;
    uint32_t h;

    uint32_t bpp;
    uint32_t channels;
};

#endif // CAMSTRUCT_H
