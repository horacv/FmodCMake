#include "page_cover.h"

#include "media/media_framework.h"

namespace
{
    const auto TITLE = "FMOD is Alive!";
    constexpr auto FONT_SIZE_TITLE = 90;
    constexpr auto FONT_SIZE_MUSIC_TEXT = 24;

    const std::string& BANK_MUSIC = "Music.bank";
    const std::string& EVENT_MUSIC = "event:/MusicTest";

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

    mMusicInstance->setCallback(nullptr);
    AudioEngine::InstanceStop(mMusicInstance);
    AudioEngine::UnloadSoundBank(mMusicBank);

    bCanDestroy.store(true, std::memory_order_release);
}

void PageCover::Start()
{
    AudioEngine::LoadSoundBankFile(BANK_MUSIC, mMusicBank);
    mMusicInstance = AudioEngine::PlayAudioEvent(EVENT_MUSIC,{},
        this, ProgrammerSoundCallback, FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_BEAT);
}

void PageCover::RenderStage()
{
    const MediaWindowSettings& windowSettings = MediaFramework::GetCurrentWindowSettings();

    DrawText(TITLE,
        static_cast<int>(static_cast<float>(windowSettings.width) * 0.2f),
        static_cast<int>(static_cast<float>(windowSettings.height) * 0.45f),
        FONT_SIZE_TITLE, LIGHTGRAY);

    auto [bar, beat] = GetCurrentMusicBarAndBeat();
    const std::string musicBeatText = std::format("Music Bar: {} Beat: {}", bar, beat);
    DrawText(musicBeatText.c_str(),
    	static_cast<int>(static_cast<float>(windowSettings.width) * 0.02f),
    	static_cast<int>(static_cast<float>(windowSettings.height) * 0.95f),
    	FONT_SIZE_MUSIC_TEXT, LIGHTGRAY);
}

FMOD_RESULT PageCover::ProgrammerSoundCallback(const FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
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