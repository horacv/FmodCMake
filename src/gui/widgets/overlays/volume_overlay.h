#ifndef VOLUME_OVERLAY_H
#define VOLUME_OVERLAY_H

#include "audio/audio_engine.h"
#include "gui/widgets/widget.h"

class VolumeOverlay : public IWidget
{
public:
    VolumeOverlay();
    ~VolumeOverlay() override = default;

    void Initialize() override;
    void Stage(std::vector<InputEvent>& outEvents) override;

private:
    AudioVCA* mMasterVolume_VCA;
    AudioVCA* mMusicVolume_VCA;
    AudioVCA* mSFXVolume_VCA;
    AudioVCA* mVOVolume_VCA;

    float mMasterVolumeCurrent;
    float mMusicVolumeCurrent;
    float mSFXVolumeCurrent;
    float mVOVolumeCurrent;
};
#endif