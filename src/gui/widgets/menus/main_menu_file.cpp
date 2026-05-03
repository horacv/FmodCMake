#include "main_menu_file.h"

#include "raygui.h"
#include "raylib.h"

namespace
{
    constexpr float MENU_WIDTH = 96;
    constexpr float MENU_HEIGHT = 26;
    constexpr float POS_X = 0;
    constexpr float POS_Y = 0;
    constexpr Rectangle menuRectangle(POS_X, POS_Y, MENU_WIDTH, MENU_HEIGHT);

    const auto LABEL_MENU_ROOT = "File";
    const auto LABEL_MENU_VOLUME_SETTINGS = GuiIconText(ICON_EXIT , "Quit");
    const auto MENU_ENTRIES = std::format("{};{}", LABEL_MENU_ROOT, LABEL_MENU_VOLUME_SETTINGS);
}

MainMenuFile::MainMenuFile() = default;

void MainMenuFile::Initialize()
{
    IWidget::Initialize();
}

void MainMenuFile::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    GuiUnlock();
    if (GuiDropdownBox(menuRectangle, MENU_ENTRIES.c_str(), &menuActiveIndex, bIsMenuOpen))
    {
        if (bIsMenuOpen)
        {
            if (menuActiveIndex == 1)
            {
                outEvents.emplace_back(QuitRequestedEvent());
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


