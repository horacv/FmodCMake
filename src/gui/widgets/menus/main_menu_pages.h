#ifndef FMODCMAKE_MAIN_MENU_PAGES_H
#define FMODCMAKE_MAIN_MENU_PAGES_H

#include "gui/widgets/widget.h"

class MainMenuPages final : public IWidget
{
public:
    MainMenuPages();
    ~MainMenuPages() override = default;

    void Initialize() override;
    void Stage(std::vector<InputEvent>& outEvents) override;

private:
    int menuActiveIndex = 0;
    bool bIsMenuOpen = false;
};
#endif
