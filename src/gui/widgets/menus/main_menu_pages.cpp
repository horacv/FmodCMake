#include "main_menu_pages.h"

#include "raygui.h"
#include "raylib.h"
#include "pages/page_programmer_sounds.h"

namespace
{
    constexpr float MENU_WIDTH = 96;
    constexpr float MENU_HEIGHT = 26;
    constexpr float POS_X = MENU_WIDTH * 1;
    constexpr float POS_Y = 0;
    constexpr Rectangle MENU_RECTANGLE(POS_X, POS_Y, MENU_WIDTH, MENU_HEIGHT);

    const auto LABEL_MENU_ROOT = "Pages";
    const auto LABEL_MENU_COVER = "Cover";
    const auto LABEL_MENU_PROGRAMMER_SOUNDS = "Prog Sounds";
    const auto MENU_ENTRIES = std::format("{};{};{}",
        LABEL_MENU_ROOT, LABEL_MENU_COVER, LABEL_MENU_PROGRAMMER_SOUNDS);
}

MainMenuPages::MainMenuPages() = default;

void MainMenuPages::Initialize()
{
    IWidget::Initialize();
}

void MainMenuPages::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    GuiUnlock();
    if (GuiDropdownBox(MENU_RECTANGLE, MENU_ENTRIES.c_str(), &menuActiveIndex, bIsMenuOpen))
    {
        if (bIsMenuOpen)
        {
            if (menuActiveIndex == 1)
            {
                outEvents.emplace_back(OpenPageEvent("Cover"));
            }
            if (menuActiveIndex == 2)
            {
                outEvents.emplace_back(OpenPageEvent("ProgrammerSounds"));
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


