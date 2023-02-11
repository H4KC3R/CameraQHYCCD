#include <stdint.h>
#include "camenums.h"

#ifndef CAMSTRUCT_H
#define CAMSTRUCT_H

struct CamParameters
{
    char m_model[12];         // camera model
    bool m_isMono;            // mono or color
    bool m_isLiveMode;        // camera mode
    bool m_isConnected = false;

    CameraStatus m_status = idle;

    double m_chipw;           // chip size width (in mm)
    double m_chiph;           // chip size height (in mm)
    double m_pixelw;          // chip pixel size width (in um)
    double m_pixelh;          // chip pixel size height (in um)
    uint32_t m_maximgw;       // maximum chip output image width
    uint32_t m_maximgh;       // maximum chip output image height
    uint32_t m_bpp;           // chip pixel depth

    uint8_t m_wbin;
    uint8_t m_hbin;
};

#endif // CAMSTRUCT_H
