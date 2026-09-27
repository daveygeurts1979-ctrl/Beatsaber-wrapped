#pragma once

namespace BeatSaberWrapped::Hooks {
    // Installs the hooks that detect when a song starts and ends so play time
    // can be logged. Call once from main.cpp's load().
    void InstallGameplayHooks();
}
