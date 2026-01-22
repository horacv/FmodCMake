# FMOD Plugins

This directory contains third-party and custom FMOD plugins, including DSP effects.

## Setup Instructions

1. Copy plugin binaries from the FMOD installation to platform-specific subdirectories
2. FMOD includes the **Google Resonance Audio** plugin by default
3. Add any custom plugins to the appropriate platform folder

## Directory Structure

```
plugins/
├── apple/                  
├── linux_x64/             
├── win_x64/                
└── ...                # Other platforms as needed
```

## Included Plugins

Common FMOD plugins include:
- **fmod_resonance.dll / fmod_resonance.dylib** - Google Resonance Audio spatial audio
- **fmod_haptics.dll / fmod_haptics.dylib** - Haptics feedback support
- Custom DSP effect plugins

## Location

Plugin files are located in the FMOD installation under `plugins/`:
- **Windows**: `C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\plugins`
- **macOS**: Inside the FMOD Studio API installer `.dmg` file
- **Linux**: Inside the FMOD Studio API `.tar` file

## Custom Plugins

To add custom plugins:
1. Place the compiled plugin binary in the appropriate platform subdirectory
2. Load the plugin at runtime using `FMOD::System::loadPlugin()`