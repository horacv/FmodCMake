#include "main_menu_file.h"

#include "raygui.h"
#include "raylib.h"

namespace
{
    constexpr float menuWidth = 96;
    constexpr float menuHeight = 26;
    constexpr float posX = 0;
    constexpr float posY = 0;
    constexpr Rectangle menuRectangle(posX, posY, menuWidth, menuHeight);

    const std::string& menuLabelRoot = "File";
    const std::string& menuLabelQuit = GuiIconText(ICON_EXIT , "Quit");
    const std::string& menuMembers = menuLabelRoot + ";" + menuLabelQuit;
}

MainMenuFile::MainMenuFile() = default;

void MainMenuFile::Initialize()
{
    IWidget::Initialize();
}

void MainMenuFile::Stage(std::vector<InputEvent>& outEvents)
{
    IWidget::Stage(outEvents);

    if (GuiDropdownBox(menuRectangle, menuMembers.c_str(), &menuActiveIndex, bIsMenuOpen))
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
}


