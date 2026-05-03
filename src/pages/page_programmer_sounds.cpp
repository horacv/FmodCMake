// Refer to:
// https://www.fmod.com/docs/2.03/studio/dialogue-and-localization.html#audio-tables
// https://www.fmod.com/docs/2.03/api/studio-guide.html#dialogue-and-localization

/*
 * This page shows how to play audio defined in audio tables through a programmer sound instrument.
 * When an event contains a programmer sound instrument, it generates a callback of type FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND.
 * It is essential to pass the key mapped to the sound in the table.
 * Pass the key to the Event Instance Play function to resolve the actual sound loaded in the bank containing the table.
 * When the sound ends or stops, the FMOD_STUDIO_EVENT_CALLBACK_END_OF_EVENT callback is triggered; do cleanup here.
 */

#include "page_programmer_sounds.h"

#include "raygui.h"
#include "media/media_framework.h"

namespace
{
    const auto TITLE = "Programmer Sounds & Audio Tables";
    constexpr auto FONT_SIZE_TITLE = 46;

    constexpr float WINDOW_WIDTH = 880;
    constexpr float WINDOW_HEIGHT = 480;
    constexpr float PADDING_Y = 10;
    constexpr float PADDING_X = 10;

    constexpr auto LABEL_WINDOW = "Programmer Sounds & Audio Tables";

    constexpr float LIST_ENTRY_HEIGHT = 32;
    constexpr float LIST_RECTANGLE_HEIGHT = LIST_ENTRY_HEIGHT * 8;

    constexpr float BUTTON_HEIGHT = 32;
    constexpr float BUTTON_WIDTH = 64;
    constexpr auto LABEL_BUTTON_PLAY = "Play";
    constexpr auto LABEL_BUTTON_STOP = "Stop";
    constexpr auto LABEL_CHECKBOX_REVERB = "Enable Reverb";

    const std::string& BANK_PROG_BASIC = "ProgrammerSounds_Basic.bank";
    const std::string& EVENT_PROG_SOUNDS = "event:/ProgrammerSound_VO";
    const std::string& PARAM_REVERB = "ReverbSendValue";

    const std::vector<std::string>& LOCALES = {
        "English (en)",
        "Spanish (es)",
        "Portuguese (pt)",
    };

    const std::unordered_map<std::string, std::string>& BANKS_PROG_LOCALIZED = {
        {"English (en)", "ProgrammerSounds_Localized_en.bank"},
        {"Spanish (es)", "ProgrammerSounds_Localized_es.bank"},
        {"Portuguese (pt)", "ProgrammerSounds_Localized_pt.bank"},
    };

    const std::vector<std::string>& PROG_SOUND_KEYS = {
        "Programmer sounds and tables (Non Localized)",
        "Sources and soundbanks (Non Localized)",
        "Try it yourself (Non Localized)",
        "My native language (Localized)",
        "Other languages (Localized)",
        "Nice talking to you! (Localized)",
    };
}

PageProgrammerSounds::PageProgrammerSounds()
: mActiveLocale{LOCALES[0]}
{}

void PageProgrammerSounds::Initialize()
{
    IPage::Initialize();
    MediaFramework::SubscribeToRenderStage(weak_from_this());

    Start();
}

void PageProgrammerSounds::Deinitialize()
{
    IPage::Deinitialize();
    MediaFramework::UnsubscribeFromRenderStage(weak_from_this());

    mCurrentAudioInstance->setCallback(nullptr);
    AudioEngine::InstanceStop(mCurrentAudioInstance, false);
    AudioEngine::UnloadSoundBank(mLoadedBasicBank);
    AudioEngine::UnloadSoundBank(mLoadedLocalizedBank);

    bCanDestroy.store(true, std::memory_order_release);
}

void PageProgrammerSounds::Start()
{
    AudioEngine::LoadSoundBankFile(BANK_PROG_BASIC, mLoadedBasicBank);
    AudioEngine::LoadSoundBankFile(BANKS_PROG_LOCALIZED.find(mActiveLocale)->second, mLoadedLocalizedBank);

    PrepareGuiContent();
}

void PageProgrammerSounds::RenderStage()
{
    /** TITLE */
    const MediaWindowSettings& windowSettings = MediaFramework::GetCurrentWindowSettings();
    const auto textSize = static_cast<float>(MeasureText(TITLE, FONT_SIZE_TITLE));
    DrawText(TITLE,
        static_cast<int>(static_cast<float>(windowSettings.width) * 0.5f - textSize * 0.5f),
        static_cast<int>(static_cast<float>(windowSettings.height) * 0.1f), FONT_SIZE_TITLE, LIGHTGRAY);

    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

    /** PANEL */
    const Vector2 pivot = {static_cast<float>(GetScreenWidth()) * 0.5f - WINDOW_WIDTH * 0.5f
        , static_cast<float>(GetScreenHeight()) * 0.5f - WINDOW_HEIGHT * 0.5f};
    const Rectangle panelRectangle{pivot.x , pivot.y,  WINDOW_WIDTH, WINDOW_HEIGHT};
    GuiPanel(panelRectangle, LABEL_WINDOW);

    /** LIST */
    const Rectangle listRectangle{pivot.x + PADDING_X, pivot.y + LIST_ENTRY_HEIGHT,
        WINDOW_WIDTH - PADDING_X * 2, LIST_RECTANGLE_HEIGHT};
    GuiListView(listRectangle, mSoundListEntries.c_str(), &mActiveListScrollIndex, &mActiveListIndex);

    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    /** CACHE DEFAULT STYLE */
    const int defaultBaseColor = GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL);
    const int defaultFocusedColor = GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED);
    const int defaultPressedColor = GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED);

    GuiUnlock();

    /** PLAY BUTTON: GREEN */
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(ColorFromHSV(120.f, 0.6f, 0.6f)));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(ColorFromHSV(120.f, 0.7f, 0.7f)));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(ColorFromHSV(120.f, 0.8f, 0.8f)));
    const Rectangle playButtonRectangle{pivot.x + PADDING_X,
        listRectangle.y + listRectangle.height + PADDING_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
    if (GuiButton(playButtonRectangle, LABEL_BUTTON_PLAY))
    {
        if (mActiveListIndex >= 0 && mActiveListIndex < PROG_SOUND_KEYS.size())
        {
            PlayProgrammerSound(PROG_SOUND_KEYS[mActiveListIndex]);
        }
    }

    /** STOP BUTTON: RED */
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(ColorFromHSV(0.0f, 0.6f, 0.6f)));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(ColorFromHSV(0.0f, 0.7f, 0.7f)));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(ColorFromHSV(0.0f, 0.8f, 0.8f)));
    const Rectangle stopButtonRectangle{playButtonRectangle.x + playButtonRectangle.width + PADDING_X,
        listRectangle.y + listRectangle.height + PADDING_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
    if (GuiButton(stopButtonRectangle, LABEL_BUTTON_STOP))
    {
        AudioEngine::InstanceStop(mCurrentAudioInstance, false);
        mCurrentAudioInstance->release();
    }

    /** RESTORE DEFAULT STYLE */
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, defaultBaseColor);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, defaultFocusedColor);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, defaultPressedColor);

    /** REVERB CHECKBOX */
    const bool bReverbEnabledCurrent = bReverbEnabled;
    const Rectangle checkboxRectangle{stopButtonRectangle.x + stopButtonRectangle.width + PADDING_X,
        listRectangle.y + listRectangle.height + PADDING_Y, BUTTON_HEIGHT, BUTTON_HEIGHT};
    GuiCheckBox(checkboxRectangle, LABEL_CHECKBOX_REVERB, &bReverbEnabled);
    if (bReverbEnabled != bReverbEnabledCurrent)
    {
        HandleChangeReverbActiveState();
    }

    /** LOCALE COMBO BOX*/
    const int currentActiveLocaleIndex = mActiveLocaleIndex;
    const Rectangle localeComboBoxRectangle{pivot.x + PADDING_X,
        playButtonRectangle.y + playButtonRectangle.height + PADDING_Y, BUTTON_WIDTH * 2.64 + PADDING_Y, BUTTON_HEIGHT};
    GuiComboBox(localeComboBoxRectangle, mComboBoxLocaleEntries.c_str(),  &mActiveLocaleIndex);
    if (currentActiveLocaleIndex != mActiveLocaleIndex)
    {
        HandleLocaleChange();
    }

    const Rectangle statusBarRectangle{pivot.x + PADDING_X,
        localeComboBoxRectangle.y + localeComboBoxRectangle.height + PADDING_Y,
        localeComboBoxRectangle.width * 1.75f, localeComboBoxRectangle.height * 2.f};

    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    /** STATUS BAR*/
    std::string status = "Now Playing:""\n";
    if (!mActiveTableKey.empty() && mCurrentAudioInstance->isValid())
    {
        status = std::format("Now Playing:""\n{}", mActiveTableKey);
    }
    GuiStatusBar(statusBarRectangle, status.c_str());

    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
}

void PageProgrammerSounds::PrepareGuiContent()
{
    for (size_t i = 0; i < PROG_SOUND_KEYS.size(); ++i)
    {
        if (i == PROG_SOUND_KEYS.size() - 1)
        {
            mSoundListEntries += std::format(" {}", PROG_SOUND_KEYS[i]);
            continue;
        }
        mSoundListEntries += std::format(" {};", PROG_SOUND_KEYS[i]);
    }

    for (size_t y = 0; y < LOCALES.size(); ++y)
    {
        if (y == LOCALES.size() - 1)
        {
            mComboBoxLocaleEntries += std::format(" {}", LOCALES[y]);
            continue;
        }
        mComboBoxLocaleEntries += std::format(" {};", LOCALES[y]);
    }
}

void PageProgrammerSounds::PlayProgrammerSound(const std::string& audioTableKey)
{
    bool bIsPlaying;
    AudioEngine::InstanceIsPlaying(mCurrentAudioInstance, bIsPlaying);

    if (bIsPlaying)
    {
        AudioEngine::InstanceStop(mCurrentAudioInstance, false);
        mCurrentAudioInstance->release();
    }

    mCurrentContext.key = audioTableKey.c_str();
    mCurrentContext.userData = this;

    mCurrentAudioInstance = AudioEngine::PlayAudioEvent(EVENT_PROG_SOUNDS,{}, &mCurrentContext, AudioEventCallback,
        FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND | FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND);

    if (mCurrentAudioInstance->isValid())
    {
        mActiveTableKey = audioTableKey;
    }
    HandleChangeReverbActiveState();
}

void PageProgrammerSounds::HandleChangeReverbActiveState() const
{
    if (mCurrentAudioInstance->isValid())
    {
        mCurrentAudioInstance->setParameterByName(PARAM_REVERB.c_str(), bReverbEnabled ? 1.0f : 0.0f);
    }
}

void PageProgrammerSounds::HandleLocaleChange()
{
    assert(mActiveLocaleIndex >= 0 && mActiveLocaleIndex < LOCALES.size());

    if (!mActiveTableKey.ends_with("(Non Localized)"))
    {
        AudioEngine::InstanceStop(mCurrentAudioInstance, false);
    }

    AudioEngine::UnloadSoundBank(mLoadedLocalizedBank);
    mActiveLocale = LOCALES[mActiveLocaleIndex];
    const auto it = BANKS_PROG_LOCALIZED.find(mActiveLocale);
    assert(it != BANKS_PROG_LOCALIZED.end());
    AudioEngine::LoadSoundBankFile(it->second, mLoadedLocalizedBank);
}

FMOD_RESULT PageProgrammerSounds::AudioEventCallback(const FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* eventInstance, void* properties)
{
    if (const auto audioInstance = reinterpret_cast<AudioInstance*>(eventInstance))
    {
        if (type == FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND)
        {
            ProgrammerSoundContext* context = nullptr;
            if (audioInstance->getUserData(reinterpret_cast<void**>(&context)) != FMOD_OK)
                return FMOD_ERR_BADCOMMAND;

            StudioSystem* studioSystem;
            if (audioInstance->getSystem(&studioSystem) != FMOD_OK)
                return FMOD_ERR_BADCOMMAND;

            AudioStudioSystemSoundInfo soundInfo;
            if (studioSystem->getSoundInfo(context->key, &soundInfo) != FMOD_OK)
                return FMOD_ERR_BADCOMMAND;

            CoreSystem* coreSystem;
            if (studioSystem->getCoreSystem(&coreSystem) != FMOD_OK)
                return FMOD_ERR_BADCOMMAND;

            AudioCoreSound* coreSound;
            if (coreSystem->createSound(soundInfo.name_or_data, soundInfo.mode, &soundInfo.exinfo, &coreSound) != FMOD_OK)
                return FMOD_ERR_BADCOMMAND;

            const auto programmerSoundProperties = static_cast<AudioProgrammerSoundProperties*>(properties);
            programmerSoundProperties->sound = reinterpret_cast<FMOD_SOUND*>(coreSound);
            programmerSoundProperties->subsoundIndex = soundInfo.subsoundindex;
            return FMOD_OK;
        }
        if (type == FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND)
        {
            const auto programmerSoundProperties = static_cast<AudioProgrammerSoundProperties*>(properties);
            const auto coreSound = reinterpret_cast<AudioCoreSound*>(programmerSoundProperties->sound);
            return coreSound->release();
        }
    }

    return FMOD_ERR_BADCOMMAND;
}
