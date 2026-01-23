#include "audio_engine.h"

#include <chrono>
#include <iostream>
#include <ostream>
#include <vector>

#if WIN32
#include <combaseapi.h>
#endif

std::unique_ptr<AudioEngine> AudioEngine::sInstance(nullptr);

namespace {
	FMOD_SPEAKERMODE ParseSpeakerMode(const std::string& speakerMode) {
		std::unordered_map<std::string, FMOD_SPEAKERMODE> speakerModes{
				{"Stereo", FMOD_SPEAKERMODE_STEREO},
				{"5.1", FMOD_SPEAKERMODE_5POINT1},
				{"7.1", FMOD_SPEAKERMODE_7POINT1},
				{"7.1.4", FMOD_SPEAKERMODE_7POINT1POINT4}
		};

		if (const auto it = speakerModes.find(speakerMode); it != speakerModes.end())
		{
			return it->second;
		}
		return FMOD_SPEAKERMODE_STEREO;
	}

	FMOD_OUTPUTTYPE ParseOutputType(const std::string& outputMode) {
		std::unordered_map<std::string, FMOD_OUTPUTTYPE> outputTypes{
			{"AutoDetect", FMOD_OUTPUTTYPE_AUTODETECT},
			{"Unknown", FMOD_OUTPUTTYPE_UNKNOWN},
			{"NoSound", FMOD_OUTPUTTYPE_NOSOUND},
			{"WavWriter", FMOD_OUTPUTTYPE_WAVWRITER},
			{"NoSoundNRT", FMOD_OUTPUTTYPE_NOSOUND_NRT},
			{"WavWriterNRT", FMOD_OUTPUTTYPE_WAVWRITER_NRT},
			{"WASAPI", FMOD_OUTPUTTYPE_WASAPI},
			{"ASIO", FMOD_OUTPUTTYPE_ASIO},
			{"PulseAudio", FMOD_OUTPUTTYPE_PULSEAUDIO},
			{"ALSA", FMOD_OUTPUTTYPE_ALSA},
			{"CoreAudio", FMOD_OUTPUTTYPE_COREAUDIO},
			{"AudioTrack", FMOD_OUTPUTTYPE_AUDIOTRACK},
			{"OpenSL", FMOD_OUTPUTTYPE_OPENSL},
			{"AudioOut", FMOD_OUTPUTTYPE_AUDIOOUT},
			{"Audio3D", FMOD_OUTPUTTYPE_AUDIO3D},
			{"WebAudio", FMOD_OUTPUTTYPE_WEBAUDIO},
			{"NNAudio", FMOD_OUTPUTTYPE_NNAUDIO},
			{"WinSonic", FMOD_OUTPUTTYPE_WINSONIC},
			{"AAudio", FMOD_OUTPUTTYPE_AAUDIO},
			{"AudioWorklet", FMOD_OUTPUTTYPE_AUDIOWORKLET},
			{"Phase", FMOD_OUTPUTTYPE_PHASE},
			{"OhAudio", FMOD_OUTPUTTYPE_OHAUDIO},
		};

		if (const auto it = outputTypes.find(outputMode); it != outputTypes.end())
		{
			return it->second;
		}

		return FMOD_OUTPUTTYPE_AUTODETECT;
	}

	FMOD_DEBUG_FLAGS ParseDebugFlags(const std::string& debugFlags) {
		std::unordered_map<std::string, FMOD_DEBUG_FLAGS> loggingLevels{
				{"None", FMOD_DEBUG_LEVEL_NONE},
				{"Log", FMOD_DEBUG_LEVEL_LOG},
				{"Warning", FMOD_DEBUG_LEVEL_WARNING},
				{"Error", FMOD_DEBUG_LEVEL_ERROR}
		};

		if (const auto it = loggingLevels.find(debugFlags); it != loggingLevels.end())
		{
			return it->second;
		}

		return FMOD_DEBUG_LEVEL_NONE;
	}
}

AudioEngine::AudioEngine()
: mStudioSystem(nullptr)
, bMainBanksLoaded(false)
{}

AudioEngine& AudioEngine::Get()
{
	if (!sInstance)
	{
		sInstance = std::unique_ptr<AudioEngine>(new AudioEngine());
#if WIN32 // Refer to: https://www.fmod.com/docs/2.03/api/platforms-win.html#com
		(void)CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
#endif
	}
	return *sInstance;
}

bool AudioEngine::Initialize()
{
	if (IsInitialized()) { return true; } // Already Initialized

	AudioEngine& audioEngine = Get();
	if (StudioSystem::create(&audioEngine.mStudioSystem) != FMOD_OK) { return false; }

	AudioConfig config;
	if (!config.LoadConfigFile("config/audio_engine.ini")) { return false; }

	CoreSystem* coreSystem = nullptr;
	if (audioEngine.mStudioSystem->getCoreSystem(&coreSystem) != FMOD_OK) { return false; }

	FMOD_SPEAKERMODE outputFormat = ParseSpeakerMode(config.GetString("System", "OutputFormat", "Stereo"));
	FMOD_OUTPUTTYPE outputType = ParseOutputType(config.GetString("System", "OutputType", "AutoDetect"));

	const int maxChannelCount = config.GetInt("System", "MaxChannelCount", 128);
	const int realChannelCount = config.GetInt("Advanced", "RealChannelCount", 64);
	const int sampleRate = config.GetInt("System", "SampleRate");
	const int dspBufferLength = config.GetInt("System", "DSPBufferLength");
	const int dspBufferCount = config.GetInt("System", "DSPBufferCount");

	int audioDriverIndex = 0;
	audioEngine.GetAudioDriverIndexByName(config.GetString("System", "InitialOutputDriverName", ""), audioDriverIndex);

	coreSystem->setSoftwareChannels(realChannelCount);
	coreSystem->setDSPBufferSize(dspBufferLength, dspBufferCount);
	coreSystem->setSoftwareFormat(sampleRate, outputFormat, 0);
	coreSystem->setOutput(outputType);
	coreSystem->setDriver(audioDriverIndex);

	void* initDriverData = nullptr;
	const std::string wavWriterPath = config.GetString("System", "WavWriterPath", "");
	if ((outputType == FMOD_OUTPUTTYPE_WAVWRITER || outputType == FMOD_OUTPUTTYPE_WAVWRITER_NRT) && !wavWriterPath.empty())
	{
		initDriverData = static_cast<void*>(const_cast<char*>(wavWriterPath.c_str()));
	}

	if (config.GetBool("System", "EnableAPIErrorLogging"))
	{
		coreSystem->setCallback(AudioEngineErrorCallback, FMOD_SYSTEM_CALLBACK_ERROR);
	}

	std::string bankKey = config.GetString("Advanced", "StudioBankKey");

	FMOD_STUDIO_ADVANCEDSETTINGS studioAdvancedSettings = {};
	studioAdvancedSettings.cbsize = sizeof(FMOD_STUDIO_ADVANCEDSETTINGS);
	studioAdvancedSettings.studioupdateperiod = config.GetInt("Advanced", "StudioUpdatePeriodMs");
	if (!bankKey.empty()) { studioAdvancedSettings.encryptionkey = bankKey.c_str(); }

	FMOD_ADVANCEDSETTINGS coreAdvancedSettings = {};

	coreAdvancedSettings.cbSize = sizeof(FMOD_ADVANCEDSETTINGS);
	coreAdvancedSettings.vol0virtualvol = config.GetFloat("Advanced", "Vol0VirtualLevel");
	coreAdvancedSettings.profilePort = config.GetInt("Advanced", "LiveUpdatePort");
	if (coreSystem->setAdvancedSettings(&coreAdvancedSettings) != FMOD_OK) { return false; }

	FMOD_STUDIO_INITFLAGS studio_init_flags = FMOD_STUDIO_INIT_NORMAL;
	FMOD_INITFLAGS init_flags = FMOD_INIT_NORMAL;

#ifndef NDEBUG
	if (config.GetBool("System", "EnableLiveUpdate")) { studio_init_flags |= FMOD_STUDIO_INIT_LIVEUPDATE; }
	if (config.GetBool("System", "EnableMemoryTracking")) { studio_init_flags |= FMOD_STUDIO_INIT_MEMORY_TRACKING; }

	// Logging only available in the Debug config (fmodstudioL and fmodL dynamic libs)
	FMOD_DEBUG_FLAGS loggingLevel = ParseDebugFlags(config.GetString("System", "DebugFlags", "None"));
	FMOD::Debug_Initialize(loggingLevel, FMOD_DEBUG_MODE_CALLBACK, AudioEngineLogCallback);
#endif

	if (audioEngine.mStudioSystem->initialize(maxChannelCount,studio_init_flags, init_flags, initDriverData) != FMOD_OK) { return false; }

	// AUDIO ENGINE CALLBACK

	audioEngine.mStudioSystem->setUserData(&audioEngine);
	audioEngine.mStudioSystem->setCallback(StudioSystemCallback, FMOD_STUDIO_SYSTEM_CALLBACK_ALL);

	// ADDITIONAL PLUGINS
	// Registering the resonance dynamic library as an additional plugin
	// Add additional third-party libraries here
	const std::vector<std::string> additionalPlugins = config.GetStringArray("Plugins", "AdditionalPlugins");
	const std::string additionalPluginsRootPath = config.GetString("Plugins", "AdditionalPluginsRootPath");
	audioEngine.RegisterAdditionalPlugins(additionalPlugins, additionalPluginsRootPath);

	// MASTER AND STRINGS BANK
	const std::string bankOutputDirectory = config.GetString("Banks", "BankOutputDirectory") + "/" + AUDIO_PLATFORM + "/";
	SetSoundBankRootDirectory(bankOutputDirectory);

	const bool bIsMainBankLoaded = LoadSoundBankFile(config.GetString("Banks", "MasterBank"));
	const bool bIsStringsBankLoaded = LoadSoundBankFile(config.GetString("Banks", "MasterStringsBank"));

	audioEngine.bMainBanksLoaded = bIsMainBankLoaded && bIsStringsBankLoaded;

	return audioEngine.mStudioSystem->isValid() && audioEngine.bMainBanksLoaded;
}

void AudioEngine::Terminate()
{
	if (AudioEngine& audioEngine = Get(); audioEngine.mStudioSystem->isValid())
	{
		audioEngine.mStudioSystem->release();
		audioEngine.mStudioSystem = nullptr;
#if WIN32 // Refer to: https://www.fmod.com/docs/2.03/api/platforms-win.html#com
		CoUninitialize();
#endif
	}
}

void AudioEngine::Update()
{
	if (IsInitialized()) { Get().mStudioSystem->update(); }
}

bool AudioEngine::IsInitialized()
{
	const AudioEngine& audioEngine = Get();
	return audioEngine.mStudioSystem && audioEngine.mStudioSystem->isValid() && audioEngine.bMainBanksLoaded;
}

// Soundbanks

void AudioEngine::SetSoundBankRootDirectory(const std::string& directory)
{
	Get().mSoundBankRootDirectory = directory;
}

bool AudioEngine::LoadSoundBankFile(const std::string& filePath)
{
	AudioBank* bank;
	return LoadSoundBankFile(filePath, bank);
}

bool AudioEngine::LoadSoundBankFile(const std::string& filePath, AudioBank*& outBankPtr)
{
	const AudioEngine& audioEngine = Get();
	if (!audioEngine.mStudioSystem->isValid()) { return false; }

	const std::string fullBankPath = audioEngine.mSoundBankRootDirectory + filePath;
	const FMOD_RESULT result = audioEngine.mStudioSystem->loadBankFile(fullBankPath.c_str(),
		FMOD_STUDIO_LOAD_BANK_NORMAL, &outBankPtr);

	return result == FMOD_OK;
}

bool AudioEngine::UnloadSoundBank(const std::string& studioPath)
{
	const AudioEngine& audioEngine = Get();
	if (!audioEngine.mStudioSystem->isValid()) { return false; }

	FMOD::Studio::Bank* bank = nullptr;
	if (audioEngine.mStudioSystem->getBank(studioPath.c_str(), &bank) == FMOD_OK)
	{
		return UnloadSoundBank(bank);
	}

	return false;
}

bool AudioEngine::UnloadSoundBank(AudioBank* bank)
{
	if (!(Get().mStudioSystem->isValid() && bank)) { return false; }
	const FMOD_RESULT result = bank->unload();
	return result == FMOD_OK;
}

// Events

AudioInstance* AudioEngine::PlayAudioEvent(const std::string& studioPath, const Audio3DAttributes& audio3dAttributes,
	void* userData, const AudioEventCallback callback, const AudioCallbackType callbackType, const bool autoStart, const bool autoRelease)
{
	if (!IsInitialized()) { return nullptr; }

	FMOD::Studio::EventDescription* description = nullptr;
	AudioInstance* instance = nullptr;

	FMOD_RESULT result = Get().mStudioSystem->getEvent(studioPath.c_str(), &description);
	if (result != FMOD_OK) { return nullptr; }

	result = description->createInstance(&instance);
	if (result != FMOD_OK) { return nullptr; }

	instance->set3DAttributes(&audio3dAttributes);

	if (callback)
	{
		instance->setCallback(callback, callbackType);
	}

	if (userData)
	{
		instance->setUserData(userData);
	}

	if (autoStart)
	{
		instance->start();
		if (autoRelease)
		{
			instance->release();
		}
	}

	return instance;
}

// Audio Instances

bool AudioEngine::InstanceStart(AudioInstance* instance)
{
	if (!(IsInitialized() && instance && instance->isValid())) { return false; }
	const FMOD_RESULT result = instance->start();
	return result == FMOD_OK;
}

bool AudioEngine::InstanceStop(AudioInstance* instance, const bool bAllowFadeOut)
{
	if (!(IsInitialized() && instance && instance->isValid())) { return false; }
	const FMOD_RESULT result = instance->stop(bAllowFadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
	return result == FMOD_OK;
}

bool AudioEngine::InstanceRelease(AudioInstance* instance)
{
	if (!(IsInitialized() && instance && instance->isValid())) { return false; }
	const FMOD_RESULT result = instance->release();
	return result == FMOD_OK;
}

bool AudioEngine::InstanceSetPaused(AudioInstance* instance, const bool bPaused)
{
	if (!(IsInitialized() && instance && instance->isValid())) { return false; }
	const FMOD_RESULT result = instance->setPaused(bPaused);
	return result == FMOD_OK;
}

// Parameters

bool AudioEngine::InstanceIsPaused(const AudioInstance* instance, bool& outPaused)
{
	if (!(IsInitialized() && instance && instance->isValid())) { return false; }
	const FMOD_RESULT result = instance->getPaused(&outPaused);
	return result == FMOD_OK;
}

bool AudioEngine::SetGlobalParameterByName(const std::string& name,
			const float value, const bool bIgnoreSeekSpeed)
{
	if (!IsInitialized()) { return false; }
	const FMOD_RESULT result = Get().mStudioSystem->setParameterByName(name.c_str(), value, bIgnoreSeekSpeed);
	return result == FMOD_OK;
}

bool AudioEngine::SetGlobalParameterByNameWithLabel(const std::string& name,
	const std::string& label, const bool bIgnoreSeekSpeed)
{
	if (!IsInitialized()) { return false; }
	const FMOD_RESULT result = Get().mStudioSystem->setParameterByNameWithLabel(name.c_str(), label.c_str(), bIgnoreSeekSpeed);
	return result == FMOD_OK;
}

bool AudioEngine::SetParameterByName(AudioInstance* instance, const std::string& name,
	const float value, const bool bIgnoreSeekSpeed)
{
	if (!(instance && instance->isValid() && IsInitialized())) { return false; }

	const FMOD_RESULT result = instance->setParameterByName(name.c_str(), value, bIgnoreSeekSpeed);
	return result == FMOD_OK;
}

bool AudioEngine::SetParameterByNameWithLabel(AudioInstance* instance,
			const std::string& name, const std::string& label, const bool bIgnoreSeekSpeed)
{
	if (!(instance && instance->isValid() && IsInitialized())) { return false; }
	const FMOD_RESULT result = instance->setParameterByNameWithLabel(name.c_str(), label.c_str(), bIgnoreSeekSpeed);
	return result == FMOD_OK;
}

// Buses

bool AudioEngine::GetBus(const std::string& studioPath, AudioBus*& outBusPtr)
{
	if (!IsInitialized()) { return false; }
	const FMOD_RESULT result = Get().mStudioSystem->getBus(studioPath.c_str(), &outBusPtr);
	return result == FMOD_OK;
}

bool AudioEngine::BusSetVolume(AudioBus* bus, const float volume)
{
	if (!(IsInitialized() && bus)) { return false; }
	const FMOD_RESULT result = bus->setVolume(volume);
	return result == FMOD_OK;
}

bool AudioEngine::BusGetVolume(const AudioBus* bus, float& outVolume)
{
	float finalVolume;
	return BusGetVolume(bus, outVolume, finalVolume);
}

bool AudioEngine::BusGetVolume(const AudioBus* bus, float& outVolume, float& finalVolume)
{
	if (!(IsInitialized() && bus)) { return false; }
	const FMOD_RESULT result = bus->getVolume(&outVolume, &finalVolume);
	return result == FMOD_OK;
}

bool AudioEngine::BusSetMute(AudioBus* bus, const bool bMute)
{
	if (!(IsInitialized() && bus)) { return false; }
	const FMOD_RESULT result = bus->setMute(bMute);
	return result == FMOD_OK;
}

bool AudioEngine::BusIsMuted(const AudioBus* bus, bool& outMuted)
{
	if (!(IsInitialized() && bus)) { return false; }
	const FMOD_RESULT result = bus->getMute(&outMuted);
	return result == FMOD_OK;
}

bool AudioEngine::BusSetPaused(AudioBus* bus, const bool bPaused)
{
	if (!(IsInitialized() && bus)) { return false; }
	const FMOD_RESULT result = bus->setPaused(bPaused);
	return result == FMOD_OK;
}

bool AudioEngine::BusIsPaused(const AudioBus* bus, bool& outPaused)
{
	if (!(IsInitialized() && bus)) { return false; }
	const FMOD_RESULT result = bus->getPaused(&outPaused);
	return result == FMOD_OK;
}

bool AudioEngine::BusStopAllAudioEvents(AudioBus* bus, const bool bAllowFadeOut)
{
	if (!(IsInitialized() && bus)) { return false; }
	const FMOD_RESULT result = bus->stopAllEvents(bAllowFadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
	return result == FMOD_OK;
}

// VCAs

bool AudioEngine::GetVCA(const std::string& studioPath, AudioVCA*& outVCAPtr)
{
	if (!IsInitialized()) { return false; }
	const FMOD_RESULT result = Get().mStudioSystem->getVCA(studioPath.c_str(), &outVCAPtr);
	return result == FMOD_OK;
}

bool AudioEngine::VCA_GetVolume(const AudioVCA* vca, float& outVolume)
{
	float finalVolume;
	return VCA_GetVolume(vca, outVolume, finalVolume);
}

bool AudioEngine::VCA_GetVolume(const AudioVCA* vca, float& outVolume, float& outFinalVolume)
{
	if (!(IsInitialized() && vca)) { return false; }
	const FMOD_RESULT result = vca->getVolume(&outVolume, &outFinalVolume);
	return result == FMOD_OK;
}

// Plugins

void AudioEngine::RegisterAdditionalPlugins(const std::vector<std::string>& pluginNames, const std::string& rootPath)
{
	if (!mStudioSystem->isValid()) { return; }

	FMOD::System* coreSystem = nullptr;
	mStudioSystem->getCoreSystem(&coreSystem);
	coreSystem->setPluginPath(rootPath.c_str());

	for (const auto& additionalPluginName : pluginNames)
	{
		uint32_t additionalPluginHandle = 0;
		if (const FMOD_RESULT result = coreSystem->loadPlugin(additionalPluginName.c_str(), &additionalPluginHandle);
			result == FMOD_OK)
		{
			additionalPluginHandles.emplace(std::pair(additionalPluginName, additionalPluginHandle));
		}
	}
}

// Helpers

bool AudioEngine::GetAudioDriverIndexByName(const std::string& audioDriverName, int& outDriverIndex) const
{
	if (!mStudioSystem->isValid()) { return false; }

	FMOD::System* coreSystem = nullptr;
	mStudioSystem->getCoreSystem(&coreSystem);

	if (!coreSystem) { return false; }

	if (!audioDriverName.empty())
	{
		int driverCount;
		coreSystem->getNumDrivers(&driverCount);

		for (int i = 0; i < driverCount; i++)
		{
			char name[256] = {};
			coreSystem->getDriverInfo(i, name, sizeof(name),
				nullptr, nullptr, nullptr, nullptr);
			if (std::string(name) == audioDriverName)
			{
				outDriverIndex = i;
				return true;
			}
		}
	}

	return false;
}

// Audio Engine (Studio) Callback

FMOD_RESULT AudioEngine::StudioSystemCallback(FMOD_STUDIO_SYSTEM* system, FMOD_STUDIO_SYSTEM_CALLBACK_TYPE type,
	void* commandData, void* userdata)
{
	const auto* audioEngine =  static_cast<AudioEngine*>(userdata);
	if (!audioEngine) { return FMOD_ERR_BADCOMMAND; }

	switch (type)
	{
		case FMOD_STUDIO_SYSTEM_CALLBACK_PREUPDATE:
			// std::cout << "FMOD UPDATE PRE" << std::endl; // Commented out to not spam the console
			break;
		case FMOD_STUDIO_SYSTEM_CALLBACK_POSTUPDATE:
			// std::cout << "FMOD UPDATE POST" << std::endl; // Commented out to not spam the console
			break;
		case FMOD_STUDIO_SYSTEM_CALLBACK_BANK_UNLOAD:
			std::cout << "FMOD BANK UNLOADED " << std::endl;
			break;
		case FMOD_STUDIO_SYSTEM_CALLBACK_LIVEUPDATE_CONNECTED:
			std::cout << "FMOD LIVE UPDATE CONNECTED" << std::endl;
			break;
		case FMOD_STUDIO_SYSTEM_CALLBACK_LIVEUPDATE_DISCONNECTED:
			std::cout << "FMOD LIVE UPDATE DISCONNECTED" << std::endl;
			break;
		default:
			break;
	}

	return FMOD_OK;
}

// Logging and Errors

#ifndef NDEBUG // Logging only available in the Debug config (fmodstudioL and fmodL dynamic libs)
FMOD_RESULT AudioEngine::AudioEngineLogCallback(const FMOD_DEBUG_FLAGS flags,
	const char* file, int line, const char* function, const char* message)
{
	const auto now = std::chrono::system_clock::now();
	const auto time64 = std::chrono::system_clock::to_time_t(now);

	std::tm localTime {};
#ifdef WIN32
	localtime_s(&localTime, &time64);   // Windows (safe version)
#else
	localtime_r(&time64, &localTime);   // POSIX (safe version on Linux/macOS)
#endif

	std::ostringstream stringStreamTime;
	stringStreamTime << std::put_time(&localTime, "%d-%b-%Y %H:%M:%S");

	std::unordered_map<FMOD_DEBUG_FLAGS, std::string> loggingLevels{
			{FMOD_DEBUG_LEVEL_LOG, "Log"},
			{FMOD_DEBUG_LEVEL_WARNING, "Warning"},
			{FMOD_DEBUG_LEVEL_ERROR, "Error"}
	};

	std::string loggingLevel;
	if (const auto it = loggingLevels.find(flags); it != loggingLevels.end())
	{
		loggingLevel = it->second;
	}

	std::cout << "FMOD " << loggingLevel << " [" + stringStreamTime.str() + "] " + message << std::endl;
	return FMOD_OK;
}
#endif

FMOD_RESULT AudioEngine::AudioEngineErrorCallback(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type,
	void* commandData1, void* commandData2, void* userdata)
{
	const auto callbackInfo = static_cast<FMOD_ERRORCALLBACK_INFO*>(commandData1);

	const auto now = std::chrono::system_clock::now();
	const auto time64 = std::chrono::system_clock::to_time_t(now);

	std::tm localTime {};
#ifdef WIN32
	localtime_s(&localTime, &time64);   // Windows (safe version)
#else
	localtime_r(&time64, &localTime);   // POSIX (safe version on Linux/macOS)
#endif

	std::ostringstream stringStreamTime;
	stringStreamTime << std::put_time(&localTime, "%d-%b-%Y %H:%M:%S");

	const std::string message = std::format("{}({}) returned error for instance type: {} (0x{})\n",
		callbackInfo->functionname,
		callbackInfo->functionparams,
		static_cast<int>(callbackInfo->result),
		callbackInfo->instance);

	std::cout << "FMOD Error [" + stringStreamTime.str() + "] " + message << std::endl;
	return FMOD_OK;
}