#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "fmod_studio.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#include "audio_config.h"

using StudioSystem = FMOD::Studio::System;
using CoreSystem = FMOD::System;

using AudioInstance = FMOD::Studio::EventInstance;
using Audio3DAttributes = FMOD_3D_ATTRIBUTES;
using AudioBank = FMOD::Studio::Bank;
using AudioBus = FMOD::Studio::Bus;
using AudioVCA = FMOD::Studio::VCA;
using AudioEventCallback = FMOD_STUDIO_EVENT_CALLBACK;
using AudioCallbackType = FMOD_STUDIO_EVENT_CALLBACK_TYPE;

class AudioEngine
{
	public:
		static AudioEngine& Get();

		static bool Initialize();
		static void Terminate();

		// Call this every frame in the program's update loop
		static void Update();
		static bool IsInitialized();

		// Soundbanks

		static void SetSoundBankRootDirectory(const std::string& directory);
		static bool LoadSoundBankFile(const std::string& filePath);
		static bool LoadSoundBankFile(const std::string& filePath, AudioBank*& outBankPtr);
		static bool UnloadSoundBank(const std::string& studioPath);
		static bool UnloadSoundBank(AudioBank* bank);

		// Events

		static AudioInstance* PlayAudioEvent(const std::string& studioPath,
			const Audio3DAttributes& audio3dAttributes = Audio3DAttributes(),
			void* userData = nullptr,
			AudioEventCallback callback = nullptr,
			AudioCallbackType callbackType = FMOD_STUDIO_EVENT_CALLBACK_ALL,
			bool autoStart = true,
			bool autoRelease = true);

		// Audio Instances

		static bool InstanceStart(AudioInstance* instance);
		static bool InstanceStop(AudioInstance* instance, bool bAllowFadeOut = true);
		static bool InstanceRelease(AudioInstance* instance);
		static bool InstanceSetPaused(AudioInstance* instance, bool bPaused);
		static bool InstanceIsPaused(const AudioInstance* instance, bool& outPaused);

		// Parameters

		static bool SetGlobalParameterByName(const std::string& name,
			float value, bool bIgnoreSeekSpeed = false);
		static bool SetGlobalParameterByNameWithLabel(const std::string& name,
			const std::string& label, bool bIgnoreSeekSpeed = false);
		static bool SetParameterByName(AudioInstance* instance,
			const std::string& name, float value, bool bIgnoreSeekSpeed = false);
		static bool SetParameterByNameWithLabel(AudioInstance* instance,
			const std::string& name, const std::string& label, bool bIgnoreSeekSpeed = false);

		// Buses

		static bool GetBus(const std::string& studioPath, AudioBus*& outBusPtr);
		static bool BusSetVolume(AudioBus* bus, float volume);
		static bool BusGetVolume(const AudioBus* bus, float& outVolume);
		static bool BusGetVolume(const AudioBus* bus, float& outVolume, float& finalVolume);
		static bool BusSetMute(AudioBus* bus, bool bMute);
		static bool BusIsMuted(const AudioBus* bus, bool& outMuted);
		static bool BusSetPaused(AudioBus* bus, bool bPaused);
		static bool BusIsPaused(const AudioBus* bus, bool& outPaused);
		static bool BusStopAllAudioEvents(AudioBus* bus, bool bAllowFadeOut = true);

		// VCAs

		static bool GetVCA(const std::string& studioPath, AudioVCA*& outVCAPtr);
		static bool VCA_GetVolume(const AudioVCA* vca, float& outVolume);
		static bool VCA_GetVolume(const AudioVCA* vca, float& outVolume, float& outFinalVolume);

		// Plugins

		void RegisterAdditionalPlugins(const std::vector<std::string>& pluginNames, const std::string& rootPath);

		// Helpers

		bool GetAudioDriverIndexByName(const std::string& audioDriverName, int& outDriverIndex) const;

	private:
		static std::unique_ptr<AudioEngine> sInstance;
		StudioSystem* mStudioSystem;
		bool bMainBanksLoaded;

		std::string mSoundBankRootDirectory;
		std::unordered_map<std::string, uint32_t> additionalPluginHandles;

		AudioEngine();

		/** Audio Engine (Studio) Callback
		 * Refer to: https://www.fmod.com/docs/2.03/api/core-api-system.html#system_setcallback
		 * "System callbacks can be called by a variety of FMOD threads,
		 *  so make sure any code executed inside the callback is thread safe"
		 */

		static FMOD_RESULT F_CALL StudioSystemCallback(FMOD_STUDIO_SYSTEM* system,
			FMOD_STUDIO_SYSTEM_CALLBACK_TYPE type, void* commandData, void* userdata);

		// Logging and Errors

#ifndef NDEBUG
		static FMOD_RESULT F_CALL AudioEngineLogCallback(FMOD_DEBUG_FLAGS flags,
			const char* file, int line, const char* function, const char* message);
#endif
		static FMOD_RESULT F_CALL AudioEngineErrorCallback(FMOD_SYSTEM* system,
			FMOD_SYSTEM_CALLBACK_TYPE type, void* commandData1, void* commandData2, void* userdata);
};

#endif
