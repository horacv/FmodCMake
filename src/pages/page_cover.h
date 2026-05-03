#ifndef PAGE_COVER_H
#define PAGE_COVER_H

#include "page.h"

#include "audio/audio_engine.h"

class PageCover : public IPage
{
public:
    PageCover();
    void Initialize() override;
    void Deinitialize() override;

protected:
    void Start() override;
    void RenderStage() override;

private:

    AudioBank* mMusicBank = nullptr;
    AudioInstance* mMusicInstance = nullptr;

    mutable std::mutex mMusicDataMutex;
    int mCurrentMusicBar;
    int mCurrentMusicBeat;

    uint32_t musicInstanceID;
    uint64_t audioObjectID;

    /**
     * Audio Event Callback.
     * Refer to: https://www.fmod.com/docs/2.03/api/core-api-system.html#system_setcallback
     * "System callbacks can be called by a variety of FMOD threads,
     *  so make sure any code executed inside the callback is thread safe"
     */
    static FMOD_RESULT F_CALL ProgrammerSoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
        FMOD_STUDIO_EVENTINSTANCE* eventInstance, void* properties);

    void SetCurrentMusicBarAndBeat(int bar, int beat);
    std::pair<int, int> GetCurrentMusicBarAndBeat() const;
};
#endif