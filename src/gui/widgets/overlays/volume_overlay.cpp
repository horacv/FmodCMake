#include "volume_overlay.h"

#include "raygui.h"

namespace
{
    constexpr float MAIN_MENU_HEIGHT = 26;

    constexpr float WINDOW_WIDTH = 342;
    constexpr float WINDOW_HEIGHT = 164;
    constexpr float WINDOW_PADDING_Y = 10;

    constexpr float SLIDER_PADDING_X = 116;
    constexpr float SLIDER_PADDING_Y = 32;
    constexpr float SLIDER_HEIGHT = 20;

    constexpr int VOLUME_MIN = 0;
    constexpr int VOLUME_MAX = 1;

    constexpr auto LABEL_WINDOW = "Volume";
    constexpr auto LABEL_SLIDER_MASTER = "Master Volume";
    constexpr auto LABEL_SLIDER_MUSIC = "Music Volume";
    constexpr auto LABEL_SLIDER_SFX = "SFX Volume";
    constexpr auto LABEL_SLIDER_VO = "VO Volume";
    
    constexpr auto VCA_MASTER_VOLUME = "vca:/Master_VCA";
    constexpr auto VCA_MUSIC_VOLUME = "vca:/Music_VCA";
    constexpr auto VCA_SFX_VOLUME = "vca:/SFX_VCA";
    constexpr auto VCA_VO_VOLUME = "vca:/VO_VCA";
}

VolumeOverlay::VolumeOverlay()
: mMasterVolume_VCA(nullptr)
, mMusicVolume_VCA(nullptr)
, mSFXVolume_VCA(nullptr)
, mVOVolume_VCA(nullptr)
, mMasterVolumeCurrent(VOLUME_MAX)
, mMusicVolumeCurrent(VOLUME_MAX)
, mSFXVolumeCurrent(VOLUME_MAX)
, mVOVolumeCurrent(VOLUME_MAX)
{}

void VolumeOverlay::Initialize()
{
    IWidget::Initialize();

    AudioEngine::GetVCA(VCA_MASTER_VOLUME, mMasterVolume_VCA);
    AudioEngine::GetVCA(VCA_MUSIC_VOLUME, mMusicVolume_VCA);
    AudioEngine::GetVCA(VCA_SFX_VOLUME, mSFXVolume_VCA);
    AudioEngine::GetVCA(VCA_VO_VOLUME, mVOVolume_VCA);

    AudioEngine::VCA_GetVolume(mMasterVolume_VCA, mMasterVolumeCurrent);
    AudioEngine::VCA_GetVolume(mMusicVolume_VCA, mMusicVolumeCurrent);
    AudioEngine::VCA_GetVolume(mSFXVolume_VCA, mSFXVolumeCurrent);
    AudioEngine::VCA_GetVolume(mVOVolume_VCA, mVOVolumeCurrent);
}

void VolumeOverlay::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    const Vector2 pivot = {static_cast<float>(GetScreenWidth()) - (WINDOW_WIDTH + WINDOW_PADDING_Y), MAIN_MENU_HEIGHT};

    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    const Rectangle windowRectangle{pivot.x , pivot.y,  WINDOW_WIDTH, WINDOW_HEIGHT};
    const bool bShouldCloseWindow = GuiWindowBox(windowRectangle, LABEL_WINDOW);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    const float masterVolumeCached = mMasterVolumeCurrent;
    const Rectangle sliderMasterRectangle{pivot.x + SLIDER_PADDING_X, pivot.y + SLIDER_PADDING_Y,  WINDOW_WIDTH * 0.50, SLIDER_HEIGHT};
    GuiSliderBar(sliderMasterRectangle, LABEL_SLIDER_MASTER,
        TextFormat("%i", static_cast<int>(mMasterVolumeCurrent * 100)), &mMasterVolumeCurrent, VOLUME_MIN, VOLUME_MAX);
    if (masterVolumeCached != mMasterVolumeCurrent)
    {
        AudioEngine::VCA_SetVolume(mMasterVolume_VCA, mMasterVolumeCurrent);
    }

    const float musicVolumeCached = mMusicVolumeCurrent;
    const Rectangle sliderMusicRectangle{pivot.x + SLIDER_PADDING_X, pivot.y + SLIDER_PADDING_Y * 2,  WINDOW_WIDTH * 0.50, SLIDER_HEIGHT};
    GuiSliderBar(sliderMusicRectangle, LABEL_SLIDER_MUSIC,
        TextFormat("%i", static_cast<int>(mMusicVolumeCurrent * 100)), &mMusicVolumeCurrent, VOLUME_MIN, VOLUME_MAX);
    if (musicVolumeCached != mMusicVolumeCurrent)
    {
        AudioEngine::VCA_SetVolume(mMusicVolume_VCA, mMusicVolumeCurrent);
    }

    const float sfxVolumeCached = mSFXVolumeCurrent;
    const Rectangle sliderSFXRectangle{pivot.x + SLIDER_PADDING_X, pivot.y + SLIDER_PADDING_Y * 3,  WINDOW_WIDTH * 0.50, SLIDER_HEIGHT};
    GuiSliderBar(sliderSFXRectangle, LABEL_SLIDER_SFX,
        TextFormat("%i", static_cast<int>(mSFXVolumeCurrent * 100)), &mSFXVolumeCurrent, VOLUME_MIN, VOLUME_MAX);
    if (sfxVolumeCached != mSFXVolumeCurrent)
    {
        AudioEngine::VCA_SetVolume(mSFXVolume_VCA, mSFXVolumeCurrent);
    }

    const float voVolumeCached = mVOVolumeCurrent;
    const Rectangle sliderVORectangle{pivot.x + SLIDER_PADDING_X, pivot.y + SLIDER_PADDING_Y * 4,  WINDOW_WIDTH * 0.50, SLIDER_HEIGHT};
    GuiSliderBar(sliderVORectangle, LABEL_SLIDER_VO,
        TextFormat("%i", static_cast<int>(mVOVolumeCurrent * 100)), &mVOVolumeCurrent, VOLUME_MIN, VOLUME_MAX);
    if (voVolumeCached != mVOVolumeCurrent)
    {
        AudioEngine::VCA_SetVolume(mVOVolume_VCA, mVOVolumeCurrent);
    }

    if (bShouldCloseWindow)
    {
        outEvents.emplace_back(ToggleAudioVolumeWindowEvent());
    }
}