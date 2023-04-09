#include <stdint.h>

#ifndef CAMSTRUCT_H
#define CAMSTRUCT_H

struct CamParameters
{
    char model[20];         // camera model
    bool isMono;            // mono or color
    bool isLiveMode;        // camera mode
    bool isConnected = false;

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

#endif // CAMSTRUCT_H
