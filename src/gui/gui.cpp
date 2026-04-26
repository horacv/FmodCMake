#include "gui.h"

#include "media/media_framework.h"
#include "widgets/menus/main_menu.h"
#include "widgets/overlays/audio_info_overlay.h"
#include "widgets/overlays/volume_overlay.h"

std::unique_ptr<GUI> GUI::sInstance = nullptr;

GUI::GUI()
: bIsInitialized(false)
, mMainMenu(std::make_unique<MainMenu>())
, bIsAudioInfoOverlayVisible(false)
, bIsVolumeOverlayVisible(false)
, mAudioInfoOverlay(std::make_unique<AudioInfoOverlay>())
, mVolumeOverlay(std::make_unique<VolumeOverlay>())
{}

GUI& GUI::Get()
{
    if (!sInstance)
    {
        sInstance = std::unique_ptr<GUI>(new GUI());
    }
    return *sInstance;
}

bool GUI::Initialize()
{
    GuiInitDarkStyle();

    auto& instance = Get();
    instance.InitializeWidgets();
    instance.bIsInitialized = true;

    return true;
}

void GUI::InitializeWidgets() const
{
    mMainMenu->Initialize();
    mAudioInfoOverlay->Initialize();
    mVolumeOverlay->Initialize();
}

void GUI::StageWidgets(std::vector<InputEvent>& outEvents)
{
    GUI& instance = Get();
    instance.mMainMenu->Stage(outEvents);

    if (instance.bIsAudioInfoOverlayVisible)
    {
        instance.mAudioInfoOverlay->Stage(outEvents);
    }
    if (instance.bIsVolumeOverlayVisible)
    {
        instance.mVolumeOverlay->Stage(outEvents);
    }
    instance.ConsumeInputEvents(outEvents);
}

void GUI::ConsumeInputEvents(std::vector<InputEvent>& ioEvents)
{
    for (auto it = ioEvents.begin(); it != ioEvents.end();)
    {
        if (std::holds_alternative<ToggleAudioInfoOverlayEvent>(*it))
        {
            bIsAudioInfoOverlayVisible = !bIsAudioInfoOverlayVisible;
            it = ioEvents.erase(it);
        }
        else if (std::holds_alternative<ToggleAudioVolumeWindowEvent>(*it))
        {
            bIsVolumeOverlayVisible = !bIsVolumeOverlayVisible;
            it = ioEvents.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void GUI::RenderStage(std::vector<InputEvent>& outEvents)
{
    StageWidgets(outEvents);
}

void GUI::Terminate()
{
    const GUI& instance = Get();
    assert(instance.bIsInitialized && "Trying to terminate uninitialized GUI");
}

bool GUI::IsInitialized()
{
    const GUI& guiInstance = Get();
    return guiInstance.bIsInitialized;
}
