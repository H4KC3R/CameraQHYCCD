#include <stdint.h>

#ifndef CAMSTRUCT_H
#define CAMSTRUCT_H

struct CAM
{
    char model[12];         // camera model
    bool isMono;            // mono or color

    double chipw;           // chip size width
    double chiph;           // chip size height
    double pixelw;          // chip pixel size width
    double pixelh;          // chip pixel size height
    uint32_t maximgw;       // maximum chip output image width
    uint32_t maximgh;       // maximum chip output image height
    uint32_t bpp;           // chip pixel depth
};


#endif // CAMSTRUCT_H
