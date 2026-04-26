#include "media_framework.h"

#include "pages/page.h"

std::unique_ptr<MediaFramework> MediaFramework::sInstance = nullptr;

MediaFramework::MediaFramework()
: bIsInitialized(false)
{}

MediaFramework& MediaFramework::Get()
{
    if (!sInstance)
    {
        sInstance = std::unique_ptr<MediaFramework>(new MediaFramework());
    }
    return *sInstance;
}

bool MediaFramework::Initialize(const MediaWindowSettings& settings)
{
    MediaFramework& instance = Get();

    SetTargetFPS(settings.fps);
    InitWindow(settings.width, settings.height, settings.title.c_str());

    instance.mCurrentWindowSettings = settings;
    instance.bIsInitialized = true;

    return instance.bIsInitialized;
}

void MediaFramework::Start()
{
}

void MediaFramework::Terminate()
{
    const MediaFramework& instance = Get();
    assert(instance.bIsInitialized && "Trying to terminate uninitialized MediaFramework");

    CloseWindow();
}

void MediaFramework::PollEvents(std::vector<InputEvent>& outEvents)
{
    if (WindowShouldClose()) { outEvents.emplace_back(QuitRequestedEvent()); }
}

void MediaFramework::RenderClear(const Color& backgroundColor)
{
    BeginDrawing();
    ClearBackground(backgroundColor);
}

void MediaFramework::RenderStage()
{
    MediaFramework& instance = Get();
    for (auto it = instance.mRenderablePages.begin(); it != instance.mRenderablePages.end();)
    {
        if (auto ptr = it->lock(); ptr == nullptr)
        {
            it = instance.mRenderablePages.erase(it);
        }
        else
        {
            ptr->RenderStage();
            ++it;
        }
    }
}

void MediaFramework::RenderPresent()
{
    EndDrawing();
}

const MediaWindowSettings& MediaFramework::GetCurrentWindowSettings()
{
    const MediaFramework& instance = Get();
    return instance.mCurrentWindowSettings;
}

void MediaFramework::SubscribeToRenderStage(const std::weak_ptr<IPage>& rendereable)
{
    MediaFramework& instance = Get();
    instance.mRenderablePages.insert(rendereable);
}

void MediaFramework::UnsubscribeFromRenderStage(const std::weak_ptr<IPage>& rendereable)
{
    MediaFramework& instance = Get();
    if (const auto page = instance.mRenderablePages.find(rendereable); page != instance.mRenderablePages.end())
    {
        instance.mRenderablePages.erase(page);
    }
}

bool MediaFramework::IsInitialized()
{
    const MediaFramework& instance = Get();
    return instance.bIsInitialized;
}

