#ifndef MAIN_MENU_SETTINGS_H
#define MAIN_MENU_SETTINGS_H

#include "gui/widgets/widget.h"

class MainMenuSettings : public IWidget
{
public:
    MainMenuSettings();
    ~MainMenuSettings() override = default;

    void Initialize() override;
    void Stage(std::vector<InputEvent>& outEvents) override;

private:
    int menuActiveIndex = 0;
    bool bIsMenuOpen = false;
};
#endif