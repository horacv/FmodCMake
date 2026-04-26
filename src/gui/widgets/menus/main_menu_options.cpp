#include "main_menu_options.h"

#include "raygui.h"
#include "raylib.h"

namespace
{
    constexpr float menuWidth = 96;
    constexpr float menuHeight = 26;
    constexpr float posX = menuWidth * 2;
    constexpr float posY = 0;
    constexpr Rectangle menuRectangle(posX, posY, menuWidth, menuHeight);

    const std::string& menuLabelRoot = "Options";
    const std::string& menuLabelToggleOverlay = GuiIconText(ICON_INFO, "Audio Info");
    const std::string& menuMembers = std::format("{};{}", menuLabelRoot, menuLabelToggleOverlay);
}

MainMenuOptions::MainMenuOptions() = default;

void MainMenuOptions::Initialize()
{
    IWidget::Initialize();
}

void MainMenuOptions::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    if (GuiDropdownBox(menuRectangle, menuMembers.c_str(), &menuActiveIndex, bIsMenuOpen))
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
}

