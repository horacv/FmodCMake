#include "main_menu_options.h"

#include "raygui.h"
#include "raylib.h"

namespace
{
    constexpr float MENU_WIDTH = 96;
    constexpr float MENU_HEIGHT = 26;
    constexpr float POS_X = MENU_WIDTH * 2;
    constexpr float POS_Y = 0;
    constexpr Rectangle MENU_RECTANGLE(POS_X, POS_Y, MENU_WIDTH, MENU_HEIGHT);

    const auto LABEL_MENU_ROOT = "Options";
    const auto LABEL_AUDIO_INFO_OVERLAY = GuiIconText(ICON_INFO, "Audio Info");
    const auto MENU_ENTRIES = std::format("{};{}", LABEL_MENU_ROOT, LABEL_AUDIO_INFO_OVERLAY);
}

MainMenuOptions::MainMenuOptions() = default;

void MainMenuOptions::Initialize()
{
    IWidget::Initialize();
}

void MainMenuOptions::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    GuiUnlock();
    if (GuiDropdownBox(MENU_RECTANGLE, MENU_ENTRIES.c_str(), &menuActiveIndex, bIsMenuOpen))
    {
        if (bIsMenuOpen)
        {
            if (menuActiveIndex == 1)
            {
                outEvents.emplace_back(ToggleAudioInfoOverlayEvent());
            }
        }
        bIsMenuOpen = !bIsMenuOpen;
        menuActiveIndex = 0;
    }

    if (bIsMenuOpen)
        GuiLock();
    else
        GuiUnlock();
}

