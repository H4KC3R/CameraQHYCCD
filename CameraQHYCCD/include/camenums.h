#ifndef CAMENUMS_H
#define CAMENUMS_H

enum streamMode {
    single = 0,
    live
};

enum cameraControls {
    gain                = 6,        //!< camera gain
    offset              = 7,        //!< camera offset
    exposure            = 8,        //!< expose time (us)
    speed               = 9,        //!< transfer speed
    transferbit         = 10,       //!< image depth bits
    usbtraffic          = 12,       //!< hblank (?)
    mechanicalshutter   = 25,       //!< mechanical shutter(?)

    bit8_depth          = 34,       //!< 8bit depth(+)
    bit16_depth         = 35        //!< 16bit depth(+)
};

#endif // CAMENUMS_H
