#pragma once

#include <string>
#include <vector>
#include <ctime>

namespace BeatSaberWrapped {

    // How many top songs to keep per period. Change freely.
    constexpr int kTopSongCount = 10;

    struct PlaySession {
        time_t timestamp;        // when the session ended (unix epoch, seconds)
        std::string levelId;     // Beat Saber level ID (unique per song)
        std::string songName;
        std::string songAuthor;
        double playedSeconds;    // real wall-clock time spent in that play
    };

    struct SongMinutes {
        std::string songName;
        std::string songAuthor;
        double minutes;
        int playCount;
    };

    struct PeriodStats {
        double totalMinutes = 0.0;
        int sessionCount = 0;
        int uniqueSongCount = 0;
        std::vector<SongMinutes> topSongs; // sorted desc by minutes, up to kTopSongCount
    };

    // Singleton responsible for persisting play sessions to disk and turning
    // the raw log into monthly / yearly "wrapped" style summaries.
    class WrappedManager {
    public:
        static WrappedManager& Get();

        // Call once on mod load. Makes sure the storage directory/file exist.
        void Init();

        // Call whenever a song-playing session ends (finished or quit early).
        void LogSession(const std::string& levelId,
                         const std::string& songName,
                         const std::string& songAuthor,
                         double playedSeconds);

        // month is 1-12.
        PeriodStats GetMonthStats(int year, int month);
        PeriodStats GetYearStats(int year);

        // Sorted ascending. Used to drive the "prev/next month" navigation.
        std::vector<int> GetAvailableYears();
        std::vector<int> GetAvailableMonthsInYear(int year);

        // Convenience for the UI: today's year/month in the device's local time.
        static void GetCurrentYearMonth(int& year, int& month);

    private:
        WrappedManager() = default;

        std::string GetLogFilePath() const;
        std::string GetDataDirPath() const;

        // Reads the whole CSV log from disk. Cached and re-read only if the
        // file's mtime changed since the last read, so repeated UI navigation
        // (prev/next month) doesn't re-parse the file every frame.
        const std::vector<PlaySession>& GetAllSessions();

        PeriodStats Aggregate(const std::vector<PlaySession>& sessions);

        std::vector<PlaySession> cachedSessions;
        time_t cachedMTime = 0;
        bool cacheLoaded = false;
    };

}
