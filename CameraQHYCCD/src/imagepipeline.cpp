#include "imagepipeline.h"

ImagePipeline::ImagePipeline() {

}

void ImagePipeline::setImage(CamImage *frame) {
    CamImage acquiredImg;

    acquiredImg.time = frame->time;

    acquiredImg.w = frame->w;
    acquiredImg.h = frame->h;
    acquiredImg.bpp = frame->bpp;
    acquiredImg.channels = frame->channels;
    acquiredImg.length = frame->length;



}

std::list <CamImage>::const_iterator ImagePipeline::getFirstFrame() {
    std::list <CamImage>::const_iterator it = mList.cbegin();
    while (it == mList.cend()) {
        std::shared_lock lock(mMutex);
        if (mList.size() != 0) {
            it = mList.begin();
        }
    }
    return it;
}

std::list <CamImage>::const_iterator ImagePipeline::nextFrame(const std::list <CamImage>::const_iterator& it) {
    std::list <CamImage>::const_iterator next = std::next(it, 1);
    while (next == mList.cend()) {
        std::shared_lock lock(mMutex);
        next = std::next(it, 1);
    }
    return next;
}

