#include "app.h"

#include "audio/audio_engine.h"
#include "gui/gui.h"
#include "media/media_framework.h"
#include "media/media_framework_data.h"
#include "pages/page_cover.h"
#include "pages/page_programmer_sounds.h"

namespace
{
	const MediaWindowSettings INIT_WINDOW_SETTINGS{"FMOD is Alive!", 1024, 768, 60};

	const std::unordered_map<std::string_view, std::function<std::unique_ptr<IPage>()>> pages = {
		{"Cover", []{ return std::make_unique<PageCover>(); }},
		{"ProgrammerSounds", []{ return std::make_unique<PageProgrammerSounds>(); }},
	};
}

Application::Application()
: mIsRunning(false)
, bIsAutoExitEnabled(false)
, currentPageName(pages.begin()->first)
, currentPage(pages.find(currentPageName)->second())
{
	std::cout << "Application Created" << std::endl;
}

void Application::Initialize()
{
	if (!(AudioEngine::Initialize() && MediaFramework::Initialize(INIT_WINDOW_SETTINGS)))
	{
		assert(!"Failed to initialize application");
	}

	GUI::Initialize();
	mIsRunning = true;

	SetupAutoExit();

	std::cout << "Game Initialized" << std::endl;
}

void Application::Run()
{
	Start();
	while (IsRunning())
	{
		ProcessEvents();
		Update();
		Render();
	}
}

void Application::Terminate() const
{
	GUI::Terminate();
	AudioEngine::Terminate();
	MediaFramework::Terminate();

	std::cout << "Game destroyed" << std::endl;
}

bool Application::IsRunning() const
{
	return mIsRunning;
}

void Application::Update()
{
	AudioEngine::Update();
	HandlePagesPendingDestroy();
	HandleAutoExit();
}

void Application::ProcessEvents()
{
	MediaFramework::PollEvents(mInputEventsCurrent);
	for (auto& inputEvent : mInputEventsCurrent)
	{
		if (std::holds_alternative<QuitRequestedEvent>(inputEvent)) { mIsRunning = false; }
		if (std::holds_alternative<OpenPageEvent>(inputEvent))
		{
			std::string& newPageName = std::get<OpenPageEvent>(inputEvent).page_name;
			if (newPageName != currentPageName)
			{
				ChangePage(newPageName);
			}
		}
	}
	mInputEventsCurrent.clear();
}

void Application::Start() const
{
	MediaFramework::Start();
	currentPage->Initialize();
	std::cout << "Game Started" << std::endl;
}

void Application::Render()
{
	MediaFramework::RenderClear(DARKGRAY);

	MediaFramework::RenderStage();
	GUI::RenderStage(mInputEventsCurrent);

	MediaFramework::RenderPresent();
}

void Application::ChangePage(const std::string_view& pageName)
{
	if (const auto it = pages.find(pageName); it != pages.end())
	{
		std::unique_ptr newPage = it->second();

		if (currentPage)
		{
			currentPage->Deinitialize();
			mPagesPendingDestroy.push_back(std::move(currentPage));
		}

		if (currentPage = std::move(newPage); currentPage)
		{
			currentPage->Initialize();
			currentPageName = pageName;
		}
	}
}

void Application::HandlePagesPendingDestroy()
{
	for (auto it = mPagesPendingDestroy.begin(); it != mPagesPendingDestroy.end();)
	{
		IPage* page = it->get();
		if (!page)
		{
			++it; continue;
		}
		if (page->CanDestroy() && !page->IsInitialized())
		{
			it = mPagesPendingDestroy.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Application::SetupAutoExit()
{
	if (const char* env = std::getenv("FMOD_CMAKE_AUTO_EXIT"))
	{
		const std::string_view envString(TextToLower(env));
		bIsAutoExitEnabled = envString == "1" || envString == "true";
	}
}

void Application::HandleAutoExit()
{
	if (bIsAutoExitEnabled)
	{
		if (GetTime() > 10)
		{
			mIsRunning = false;
		}
	}
}
