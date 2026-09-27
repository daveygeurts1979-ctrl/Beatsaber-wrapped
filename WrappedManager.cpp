#include "WrappedManager.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <sys/stat.h>
#include <cstring>

#include "Logging.hpp"

namespace BeatSaberWrapped {

    static constexpr char kDelim = '\x1F'; // unit separator, safe against song titles

    WrappedManager& WrappedManager::Get() {
        static WrappedManager instance;
        return instance;
    }

    std::string WrappedManager::GetDataDirPath() const {
        return "/sdcard/ModData/com.beatgames.beatsaber/Mods/BeatSaberWrapped";
    }

    std::string WrappedManager::GetLogFilePath() const {
        return GetDataDirPath() + "/playlog.csv";
    }

    // Minimal recursive "mkdir -p" — avoids depending on std::filesystem, which
    // has historically been flaky on some Quest NDK toolchains.
    static void MkdirRecursive(const std::string& path) {
        std::string current;
        for (size_t i = 0; i < path.size(); i++) {
            current += path[i];
            if (path[i] == '/' && !current.empty()) {
                mkdir(current.c_str(), 0777);
            }
        }
        mkdir(path.c_str(), 0777);
    }

    void WrappedManager::Init() {
        MkdirRecursive(GetDataDirPath());
        std::ifstream test(GetLogFilePath());
        if (!test.good()) {
            std::ofstream create(GetLogFilePath(), std::ios::app);
        }
    }

    static std::string Sanitize(const std::string& in) {
        std::string out = in;
        std::replace(out.begin(), out.end(), kDelim, ' ');
        std::replace(out.begin(), out.end(), '\n', ' ');
        std::replace(out.begin(), out.end(), '\r', ' ');
        return out;
    }

    void WrappedManager::LogSession(const std::string& levelId,
                                     const std::string& songName,
                                     const std::string& songAuthor,
                                     double playedSeconds) {
        // Ignore accidental near-zero sessions (menu flicker, instant restarts, etc.)
        if (playedSeconds < 3.0) return;

        std::ofstream out(GetLogFilePath(), std::ios::app);
        if (!out.good()) {
            getLogger().error("Failed to open play log for writing");
            return;
        }

        time_t now = time(nullptr);
        out << now << kDelim
            << Sanitize(levelId) << kDelim
            << Sanitize(songName) << kDelim
            << Sanitize(songAuthor) << kDelim
            << playedSeconds << "\n";

        getLogger().info("Logged %.1f min of \"%s\"", playedSeconds / 60.0, songName.c_str());
    }

    const std::vector<PlaySession>& WrappedManager::GetAllSessions() {
        struct stat st{};
        time_t mtime = 0;
        if (stat(GetLogFilePath().c_str(), &st) == 0) {
            mtime = st.st_mtime;
        }

        if (cacheLoaded && mtime == cachedMTime) {
            return cachedSessions;
        }

        cachedSessions.clear();
        std::ifstream in(GetLogFilePath());
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;

            std::vector<std::string> fields;
            std::stringstream ss(line);
            std::string field;
            while (std::getline(ss, field, kDelim)) {
                fields.push_back(field);
            }
            if (fields.size() != 5) continue; // malformed/old-format line, skip

            PlaySession session;
            try {
                session.timestamp = static_cast<time_t>(std::stoll(fields[0]));
                session.levelId = fields[1];
                session.songName = fields[2];
                session.songAuthor = fields[3];
                session.playedSeconds = std::stod(fields[4]);
            } catch (...) {
                continue;
            }
            cachedSessions.push_back(session);
        }

        cachedMTime = mtime;
        cacheLoaded = true;
        return cachedSessions;
    }

    PeriodStats WrappedManager::Aggregate(const std::vector<PlaySession>& sessions) {
        PeriodStats stats;
        stats.sessionCount = static_cast<int>(sessions.size());

        std::unordered_map<std::string, SongMinutes> bySong;
        for (const auto& s : sessions) {
            stats.totalMinutes += s.playedSeconds / 60.0;

            std::string key = s.songName + "\x01" + s.songAuthor;
            auto it = bySong.find(key);
            if (it == bySong.end()) {
                bySong[key] = SongMinutes{s.songName, s.songAuthor, s.playedSeconds / 60.0, 1};
            } else {
                it->second.minutes += s.playedSeconds / 60.0;
                it->second.playCount += 1;
            }
        }

        stats.uniqueSongCount = static_cast<int>(bySong.size());

        std::vector<SongMinutes> all;
        all.reserve(bySong.size());
        for (auto& [key, val] : bySong) all.push_back(val);

        std::sort(all.begin(), all.end(), [](const SongMinutes& a, const SongMinutes& b) {
            return a.minutes > b.minutes;
        });

        if (all.size() > static_cast<size_t>(kTopSongCount)) {
            all.resize(kTopSongCount);
        }
        stats.topSongs = std::move(all);

        return stats;
    }

    PeriodStats WrappedManager::GetMonthStats(int year, int month) {
        std::vector<PlaySession> filtered;
        for (const auto& s : GetAllSessions()) {
            std::tm* t = std::localtime(&s.timestamp);
            if (t->tm_year + 1900 == year && t->tm_mon + 1 == month) {
                filtered.push_back(s);
            }
        }
        return Aggregate(filtered);
    }

    PeriodStats WrappedManager::GetYearStats(int year) {
        std::vector<PlaySession> filtered;
        for (const auto& s : GetAllSessions()) {
            std::tm* t = std::localtime(&s.timestamp);
            if (t->tm_year + 1900 == year) {
                filtered.push_back(s);
            }
        }
        return Aggregate(filtered);
    }

    std::vector<int> WrappedManager::GetAvailableYears() {
        std::set<int> years;
        for (const auto& s : GetAllSessions()) {
            std::tm* t = std::localtime(&s.timestamp);
            years.insert(t->tm_year + 1900);
        }
        return std::vector<int>(years.begin(), years.end());
    }

    std::vector<int> WrappedManager::GetAvailableMonthsInYear(int year) {
        std::set<int> months;
        for (const auto& s : GetAllSessions()) {
            std::tm* t = std::localtime(&s.timestamp);
            if (t->tm_year + 1900 == year) {
                months.insert(t->tm_mon + 1);
            }
        }
        return std::vector<int>(months.begin(), months.end());
    }

    void WrappedManager::GetCurrentYearMonth(int& year, int& month) {
        time_t now = time(nullptr);
        std::tm* t = std::localtime(&now);
        year = t->tm_year + 1900;
        month = t->tm_mon + 1;
    }

}
