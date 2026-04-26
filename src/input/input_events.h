#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

struct QuitRequestedEvent {};
struct ToggleAudioInfoOverlayEvent {};
struct ToggleAudioVolumeWindowEvent {};
struct OpenPageEvent { std::string page_name = std::string(); };

using InputEvent = std::variant<
	OpenPageEvent,
	QuitRequestedEvent,
	ToggleAudioInfoOverlayEvent,
	ToggleAudioVolumeWindowEvent
>;
#endif