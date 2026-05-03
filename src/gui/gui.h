#ifndef GUI_H
#define GUI_H

#include "widgets/widget.h"

extern "C" {
    void GuiInitDarkStyle();
}

struct MediaWindowSettings;

class GUI
{
    public:
        static GUI& Get();

        static bool Initialize();
        static void RenderStage(std::vector<InputEvent>& outEvents);
        static void Terminate();

        static bool IsInitialized();

    private:
        bool bIsInitialized;

        static std::unique_ptr<GUI> sInstance;

        bool bIsAudioInfoOverlayVisible;
        bool bIsVolumeOverlayVisible;

        std::unique_ptr<IWidget> mMainMenu;
        std::unique_ptr<IWidget> mAudioInfoOverlay;
        std::unique_ptr<IWidget> mVolumeOverlay;

        GUI();
        void InitializeWidgets() const;
        static void StageWidgets(std::vector<InputEvent>& outEvents);
        void ConsumeInputEvents(std::vector<InputEvent>& ioEvents);
};
#endif