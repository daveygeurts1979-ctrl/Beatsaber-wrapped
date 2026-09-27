#pragma once

// Central logger for the whole mod. Uses paper2_scotland2's context logger.
// If your restored dependencies expose logging differently, this is the one
// place to change it.
#include "paper2_scotland2/shared/logger.hpp"

namespace BeatSaberWrapped {
    inline auto& getLogger() {
        static auto logger = Paper::ConstLoggerContext<"BeatSaberWrapped">();
        return logger;
    }
}
