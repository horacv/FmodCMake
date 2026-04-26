#include "main_menu_settings.h"

#include "raygui.h"
#include "raylib.h"

namespace
{
	constexpr float menuWidth = 96;
	constexpr float menuHeight = 26;
	constexpr float posX = menuWidth * 3;
	constexpr float posY = 0;
	Rectangle menuRectangle(posX, posY, menuWidth, menuHeight);

	std::string menuLabelRoot = "Settings";
	std::string menuLabelToggleVolumeSettings = GuiIconText(ICON_AUDIO, "Volume");
	std::string menuMembers = std::format("{};{}", menuLabelRoot, menuLabelToggleVolumeSettings);
}

MainMenuSettings::MainMenuSettings() = default;

void MainMenuSettings::Initialize()
{
	IWidget::Initialize();
}

void MainMenuSettings::Stage(std::vector<InputEvent>& outEvents)
{
	IWidget::Stage(outEvents);

	if (GuiDropdownBox(menuRectangle, menuMembers.c_str(), &menuActiveIndex, bIsMenuOpen))
	{
		if (bIsMenuOpen)
		{
			if (menuActiveIndex == 1)
			{
				outEvents.emplace_back(ToggleAudioVolumeWindowEvent());
			}
		}
		bIsMenuOpen = !bIsMenuOpen;
		menuActiveIndex = 0;
	}
}
