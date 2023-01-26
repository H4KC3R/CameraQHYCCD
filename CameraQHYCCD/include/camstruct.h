#include <stdint.h>

#ifndef CAMSTRUCT_H
#define CAMSTRUCT_H

struct CAM
{
    uint32_t gain;
    uint32_t offset;
    uint32_t exptime;
    uint32_t bin;
    uint32_t imagew;
    uint32_t maximgw;
    uint32_t imageh;
    uint32_t maximgh;
    uint32_t bpp;
    uint32_t channels;
    double chipw;
    double chiph;
    double pixelw;
    double pixelh;
    int length;
    bool getData;
    bool quit;
    bool hasquit;
    bool canget;
    bool canTransferData;

    char model[12];

    unsigned char *ImgData;
    unsigned char *ImgDataSave;
};


#endif // CAMSTRUCT_H
