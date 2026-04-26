#include "page_cover.h"

#include "media/media_framework.h"

namespace
{
    // UI
    const std::string& title = "FMOD is Alive!";

    // AUDIO
    const std::string& soundbankName = "Music.bank";
    const std::string& audioEventPath = "event:/MusicTest";

}


PageCover::PageCover()
: mCurrentMusicBar(0)
, mCurrentMusicBeat(0)
, musicInstanceID(0)
, audioObjectID(0)
{}

void PageCover::Initialize()
{
    IPage::Initialize();
    MediaFramework::SubscribeToRenderStage(weak_from_this());

    Start();
}

void PageCover::Deinitialize()
{
    IPage::Deinitialize();
    MediaFramework::UnsubscribeFromRenderStage(weak_from_this());
}

void PageCover::Start()
{
    AudioEngine::LoadSoundBankFile(soundbankName);
    AudioEngine::PlayAudioEvent(audioEventPath,{},
        this, GlobalAudioEventCallback, FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_BEAT);
}

void PageCover::RenderStage()
{
    const MediaWindowSettings& windowSettings = MediaFramework::GetCurrentWindowSettings();

    DrawText(title.c_str(),
        static_cast<int>(static_cast<float>(windowSettings.width) * 0.2f),
        static_cast<int>(static_cast<float>(windowSettings.height) * 0.45f),
        90, LIGHTGRAY);

    auto [bar, beat] = GetCurrentMusicBarAndBeat();
    const std::string musicBeatText = std::format("Music Bar: {} Beat: {}", bar, beat);
    DrawText(musicBeatText.c_str(),
    	static_cast<int>(static_cast<float>(windowSettings.width) * 0.02f),
    	static_cast<int>(static_cast<float>(windowSettings.height) * 0.95f),
    	24, LIGHTGRAY);
}

FMOD_RESULT PageCover::GlobalAudioEventCallback(const FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
    FMOD_STUDIO_EVENTINSTANCE* eventInstance, void* properties)
{
    if (type == FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_BEAT)
    {
        const auto* musicProperties = static_cast<FMOD_STUDIO_TIMELINE_BEAT_PROPERTIES*>(properties);
        const auto* audioInstance = reinterpret_cast<AudioInstance*>(eventInstance);
        void* userData;
        if (audioInstance->getUserData(&userData) == FMOD_OK)
        {
            if (const auto game = static_cast<PageCover*>(userData))
            {
                game->SetCurrentMusicBarAndBeat(musicProperties->bar, musicProperties->beat);
                return FMOD_OK;
            }
        }
    }

    // Add more callback types here
    //...

    //...

    return FMOD_ERR_BADCOMMAND;
}

void PageCover::SetCurrentMusicBarAndBeat(const int bar, const int beat)
{
    std::lock_guard lock(mMusicDataMutex);
    mCurrentMusicBar = bar;
    mCurrentMusicBeat = beat;
}

std::pair<int, int> PageCover::GetCurrentMusicBarAndBeat() const
{
    std::lock_guard lock(mMusicDataMutex);
    return {mCurrentMusicBar, mCurrentMusicBeat};
}

#undef FONT_PATH