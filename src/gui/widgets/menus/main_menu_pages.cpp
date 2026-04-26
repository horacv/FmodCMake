#include "main_menu_pages.h"

#include "raygui.h"
#include "raylib.h"

namespace
{
    constexpr float menuWidth = 96;
    constexpr float menuHeight = 26;
    constexpr float posX = menuWidth * 1;
    constexpr float posY = 0;
    constexpr Rectangle menuRectangle(posX, posY, menuWidth, menuHeight);

    const std::string& menuLabelRoot = "Pages";
    const std::string& menuMembers = menuLabelRoot;
}

MainMenuPages::MainMenuPages() = default;

void MainMenuPages::Initialize()
{
    IWidget::Initialize();
}

void MainMenuPages::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    if (GuiDropdownBox(menuRectangle, menuMembers.c_str(), &menuActiveIndex, bIsMenuOpen))
    {
        if (bIsMenuOpen)
        {

        }
        bIsMenuOpen = !bIsMenuOpen;
        menuActiveIndex = 0;
    }
}


