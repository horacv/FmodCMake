#ifndef PROGRAMMER_SOUNDS_H
#define PROGRAMMER_SOUNDS_H

#include "page.h"

#include "audio/audio_engine.h"

class PageProgrammerSounds : public IPage
{
public:
    PageProgrammerSounds();
    void Initialize() override;
    void Deinitialize() override;

protected:
    void Start() override;
    void RenderStage() override;

private:
    ProgrammerSoundContext mCurrentContext{};

    AudioInstance* mCurrentAudioInstance = nullptr;
    AudioBank* mLoadedBasicBank = nullptr;
    AudioBank* mLoadedLocalizedBank = nullptr;

    std::string mSoundListEntries;
    std::string mComboBoxLocaleEntries;
    std::string mActiveLocale;
    std::string mActiveTableKey;

    int mActiveListScrollIndex = -1;
    int mActiveListIndex = 0;
    int mActiveLocaleIndex = 0;
    bool bReverbEnabled = false;

    void PrepareGuiContent();
    void PlayProgrammerSound(const std::string& audioTableKey);
    void HandleChangeReverbActiveState() const;
    void HandleLocaleChange();

    /**
     * Audio Event Callback.
     * Refer to: https://www.fmod.com/docs/2.03/api/core-api-system.html#system_setcallback
     * "System callbacks can be called by a variety of FMOD threads,
     *  so make sure any code executed inside the callback is thread safe"
     */
    static FMOD_RESULT F_CALL AudioEventCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
        FMOD_STUDIO_EVENTINSTANCE* eventInstance, void* properties);
};
#endif