#ifndef FMODCMAKE_MAIN_MENU_FILE_H
#define FMODCMAKE_MAIN_MENU_FILE_H

#include "gui/widgets/widget.h"

class MainMenuFile final : public IWidget
{
public:
    MainMenuFile();
    ~MainMenuFile() override = default;

    void Initialize() override;
    void Stage(std::vector<InputEvent>& outEvents) override;

private:
    int menuActiveIndex = 0;
    bool bIsMenuOpen = false;
};
#endif
