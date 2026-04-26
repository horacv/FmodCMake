#include "main_menu_about.h"

#include "fmod_common.h"
#include "raygui.h"
#include "raylib.h"

namespace
{
    constexpr float menuWidth = 96;
    constexpr float menuHeight = 26;
    constexpr float posX = menuWidth * 4;
    constexpr float posY = 0;
    Rectangle menuRectangleClosed(posX, posY, menuWidth, menuHeight);
    Rectangle menuRectangleOpen(posX, posY, menuWidth * 2.8, menuHeight);

    const std::string& menuLabelRoot = "About";

    constexpr auto LABEL_SEPARATOR_LIBRARIES = " LIBRARIES --------------------------------";
    const std::string TEXT_RAYLIB_VERSION = std::format(" Raylib: v{}", RAYLIB_VERSION);
    const std::string TEXT_RAYGUI_VERSION = std::format(" Raygui: v{}", RAYGUI_VERSION);
    const std::string TEXT_FMOD_VERSION = std::format( " FMOD API: v{}.{}.{}",
        FMOD_VERSION >> 16 & 0xFF, FMOD_VERSION >> 8 & 0xFF, FMOD_VERSION & 0xFF);

    constexpr auto LABEL_SEPARATOR_CREDITS = " CREDITS ----------------------------------";
    constexpr auto TEXT_CREATED_BY = " Created by: Horacio Valdivieso Sotomayor";
    constexpr auto TEXT_COMPANY = " ABOVE NOISE STUDIOS";
    constexpr auto TEXT_EMAIL = " horacio@abovenoisestudios.com";

    const std::string infoFull = std::format("{}\n{}\n{}\n{}\n{}\n{}\n{}\n{}",
        LABEL_SEPARATOR_LIBRARIES, TEXT_RAYLIB_VERSION, TEXT_RAYGUI_VERSION, TEXT_FMOD_VERSION,
        LABEL_SEPARATOR_CREDITS, TEXT_CREATED_BY, TEXT_COMPANY, TEXT_EMAIL);

    const std::string menuMembers = std::format("{};{}", menuLabelRoot, infoFull);
}

MainMenuAbout::MainMenuAbout() = default;

void MainMenuAbout::Initialize()
{
    IWidget::Initialize();
}

void MainMenuAbout::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    const Rectangle menuRectangle = bIsMenuOpen ? menuRectangleOpen : menuRectangleClosed;
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, bIsMenuOpen ? TEXT_ALIGN_LEFT : TEXT_ALIGN_CENTER);
    if (GuiDropdownBox(menuRectangle, menuMembers.c_str(), &menuActiveIndex, bIsMenuOpen))
    {
        bIsMenuOpen = !bIsMenuOpen;
        menuActiveIndex = 0;
    }
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
}
