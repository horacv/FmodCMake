#ifndef MEDIA_FRAMEWORK_H
#define MEDIA_FRAMEWORK_H

#include "input/input_events.h"
#include "media_framework.h"
#include "media_framework_data.h"
#include <raylib.h>

class IPage;

using RendereablePageSet = std::set<std::weak_ptr<IPage>, std::owner_less<std::weak_ptr<IPage>>>;

class MediaFramework
{
public:
    static MediaFramework& Get();

    static bool Initialize(const MediaWindowSettings& settings);
    static void Start();
    static void Terminate();

    static void PollEvents(std::vector<InputEvent>& outEvents);
    static void RenderClear(const Color& backgroundColor);
    static void RenderStage();
    static void RenderPresent();

    static const MediaWindowSettings& GetCurrentWindowSettings();

    static void SubscribeToRenderStage(const std::weak_ptr<IPage>& rendereable);
    static void UnsubscribeFromRenderStage(const std::weak_ptr<IPage>& rendereable);

    static bool IsInitialized();

private:
    static std::unique_ptr<MediaFramework> sInstance;
    bool bIsInitialized;

    MediaWindowSettings mCurrentWindowSettings;
    RendereablePageSet mRenderablePages;

    MediaFramework();
};
#endif