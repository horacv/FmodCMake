# FMOD Studio API

This directory contains the FMOD Studio API files (high-level event-based audio system).

## Setup Instructions

1. Copy header files from the FMOD installation to the `inc/` folder
2. Copy platform-specific binaries to the appropriate subdirectory in `lib/`

## Directory Structure

```
studio/
├── inc/                    # FMOD Studio header files
│   ├── fmod_studio.h
│   ├── fmod_studio.hpp
│   └── fmod_studio_common.h
└── lib/                    # Platform-specific binaries
    ├── apple/                  
    ├── linux_x64/             
    ├── win_x64/ 
    └── ...                 # Other platforms as needed
```

## Location

Files are located in the FMOD installation under `api/studio/`:
- **Windows**: `C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api\studio`
- **macOS**: Inside the FMOD Studio API installer `.dmg` file
- **Linux**: Inside the FMOD Studio API `.tar` file
