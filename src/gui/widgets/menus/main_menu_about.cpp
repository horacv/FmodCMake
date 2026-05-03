#include "main_menu_about.h"

#include "fmod_common.h"
#include "raygui.h"
#include "raylib.h"

namespace
{
    constexpr float MENU_WIDTH = 96;
    constexpr float MENU_HEIGHT = 26;
    constexpr float POS_X = MENU_WIDTH * 4;
    constexpr float POS_Y = 0;
    constexpr Rectangle MENU_RECTANGLE_CLOSED(POS_X, POS_Y, MENU_WIDTH, MENU_HEIGHT);
    constexpr Rectangle MENU_RECTANGLE_OPEN(POS_X, POS_Y, MENU_WIDTH * 2.8, MENU_HEIGHT);

    const auto LABEL_MENU_ROOT = "About";

    constexpr auto LABEL_SEPARATOR_LIBRARIES = " LIBRARIES --------------------------------";
    const auto TEXT_RAYLIB_VERSION = std::format(" Raylib: v{}", RAYLIB_VERSION);
    const auto TEXT_RAYGUI_VERSION = std::format(" Raygui: v{}", RAYGUI_VERSION);
    const auto TEXT_FMOD_VERSION = std::format( " FMOD API: v{:x}.{:02x}.{:02x}",
        FMOD_VERSION >> 16 & 0xFF, FMOD_VERSION >> 8 & 0xFF, FMOD_VERSION & 0xFF);

    constexpr auto LABEL_SEPARATOR_CREDITS = " CREDITS ----------------------------------";
    constexpr auto TEXT_CREATED_BY = " Created by: Horacio Valdivieso Sotomayor";
    constexpr auto TEXT_COMPANY = " ABOVE NOISE STUDIOS";
    constexpr auto TEXT_EMAIL = " horacio@abovenoisestudios.com";

    const auto INFO = std::format("{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}",
        LABEL_SEPARATOR_LIBRARIES, TEXT_RAYLIB_VERSION, TEXT_RAYGUI_VERSION, TEXT_FMOD_VERSION,
        LABEL_SEPARATOR_CREDITS, TEXT_CREATED_BY, TEXT_COMPANY, TEXT_EMAIL);

    const std::string menuMembers = std::format("{};{}", LABEL_MENU_ROOT, INFO);
}

MainMenuAbout::MainMenuAbout() = default;

void MainMenuAbout::Initialize()
{
    IWidget::Initialize();
}

void MainMenuAbout::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    GuiUnlock();
    const Rectangle menuRectangle = bIsMenuOpen ? MENU_RECTANGLE_OPEN : MENU_RECTANGLE_CLOSED;
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, bIsMenuOpen ? TEXT_ALIGN_LEFT : TEXT_ALIGN_CENTER);
    if (GuiDropdownBox(menuRectangle, menuMembers.c_str(), &menuActiveIndex, bIsMenuOpen))
    {
        bIsMenuOpen = !bIsMenuOpen;
        menuActiveIndex = 0;
    }
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    if (bIsMenuOpen)
        GuiLock();
    else
        GuiUnlock();
}
