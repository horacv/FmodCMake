#include "audio_info_overlay.h"

#include "audio/audio_engine.h"
#include "raygui.h"

namespace
{
    constexpr float PADDING = 10;
    constexpr float HEIGHT = 100;
    constexpr float DRIVER_NAME_PADDING = 100;
}

AudioInfoOverlay::AudioInfoOverlay() = default;

void AudioInfoOverlay::Initialize()
{
    IWidget::Initialize();
}

void AudioInfoOverlay::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    int sampleRate, numSpeakers;
    std::string speakerMode;
    std::string driverName;
    if (!AudioEngine::GetCurrentAudioDriverInfo(driverName, sampleRate, numSpeakers, speakerMode))
    {
        return;
    }

    const std::string infoSampleRate = std::format("Sample Rate: {}", sampleRate);
    const std::string infoNumSpeakers = std::format("Number of Speakers: {}", numSpeakers);
    const std::string infoSpeakerMode = std::format("Speaker Mode: {}", speakerMode);
    const std::string infoDriverName = std::format("Driver Name: {}", driverName);
    const std::string infoFull = std::format("{}\n{}\n{}\n{}", infoSampleRate, infoNumSpeakers, infoSpeakerMode, infoDriverName);

    const float width = static_cast<float>(MeasureText(infoDriverName.c_str(), 1)) + DRIVER_NAME_PADDING;
    const Rectangle overlayRectangle{static_cast<float>(GetScreenWidth()) - (width + PADDING),
        static_cast<float>(GetScreenHeight()) - (HEIGHT + PADDING),  width, HEIGHT};

    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiStatusBar(overlayRectangle, infoFull.c_str());
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
}