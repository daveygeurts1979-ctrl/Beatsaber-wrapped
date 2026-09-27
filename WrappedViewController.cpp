#include "WrappedViewController.hpp"
#include "WrappedManager.hpp"
#include "Logging.hpp"

#include <sstream>
#include <iomanip>

using namespace BeatSaberWrapped;
using namespace BeatSaberWrapped::UI;

// Simple BSML layout: a header, a prev/next/toggle button row, a stats line,
// and a scrollable text block for the top-10 list. Tweak freely — colors and
// spacing here have no effect on the tracking logic.
static constexpr auto kLayoutXml = R"(
<vertical horizontal-fit="PreferredSize" vertical-fit="PreferredSize" pad="4" spacing="2">
    <text id="headerText" text="Beat Saber Wrapped" align="Center" font-size="6" italics="false"/>

    <horizontal horizontal-fit="PreferredSize" spacing="3" pad="1">
        <button text="◀" on-click="OnPrevPressed" min-width="14"/>
        <button text="This Month / Year" on-click="OnToggleYearPressed" min-width="40"/>
        <button text="▶" on-click="OnNextPressed" min-width="14"/>
    </horizontal>

    <text id="statsText" text="" align="Center" font-size="3.6"/>

    <scroll-view vertical-fit="PreferredSize" size-delta-x="80" size-delta-y="45">
        <text id="songListText" text="" font-size="3.2" rich-text="true"/>
    </scroll-view>
</vertical>
)";

static const char* kMonthNames[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

void WrappedViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    HMUI::ViewController::DidActivate(firstActivation, addedToHierarchy, screenSystemEnabling);

    if (firstActivation) {
        BSML::parse_and_construct(kLayoutXml, this->get_transform(), this);

        int year, month;
        WrappedManager::GetCurrentYearMonth(year, month);
        currentYear = year;
        currentMonth = month;
        showingYear = false;
    }

    RefreshDisplay();
}

static std::string FormatPeriodStats(const PeriodStats& stats) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(0);
    ss << (int)stats.totalMinutes << " minutes played  |  "
       << stats.sessionCount << " sessions  |  "
       << stats.uniqueSongCount << " different songs";
    return ss.str();
}

static std::string FormatTopSongs(const PeriodStats& stats) {
    if (stats.topSongs.empty()) {
        return "<i>No plays logged for this period yet.</i>";
    }
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1);
    int rank = 1;
    for (const auto& song : stats.topSongs) {
        ss << "<b>#" << rank << "</b>  " << song.songName
           << " <color=#AAAAAA>- " << song.songAuthor << "</color>\n"
           << "      " << song.minutes << " min"
           << " (" << song.playCount << (song.playCount == 1 ? " play" : " plays") << ")\n\n";
        rank++;
    }
    return ss.str();
}

void WrappedViewController::RefreshDisplay() {
    if (!headerText || !statsText || !songListText) return;

    if (showingYear) {
        PeriodStats stats = WrappedManager::Get().GetYearStats(currentYear);
        headerText->set_text(il2cpp_utils::newcsstr(
            std::to_string(currentYear) + " Wrapped"));
        statsText->set_text(il2cpp_utils::newcsstr(FormatPeriodStats(stats)));
        songListText->set_text(il2cpp_utils::newcsstr(FormatTopSongs(stats)));
    } else {
        PeriodStats stats = WrappedManager::Get().GetMonthStats(currentYear, currentMonth);
        std::string title = std::string(kMonthNames[currentMonth - 1]) + " " + std::to_string(currentYear);
        headerText->set_text(il2cpp_utils::newcsstr(title));
        statsText->set_text(il2cpp_utils::newcsstr(FormatPeriodStats(stats)));
        songListText->set_text(il2cpp_utils::newcsstr(FormatTopSongs(stats)));
    }
}

void WrappedViewController::OnPrevPressed() {
    if (showingYear) {
        currentYear -= 1;
    } else {
        currentMonth -= 1;
        if (currentMonth < 1) {
            currentMonth = 12;
            currentYear -= 1;
        }
    }
    RefreshDisplay();
}

void WrappedViewController::OnNextPressed() {
    if (showingYear) {
        currentYear += 1;
    } else {
        currentMonth += 1;
        if (currentMonth > 12) {
            currentMonth = 1;
            currentYear += 1;
        }
    }
    RefreshDisplay();
}

void WrappedViewController::OnToggleYearPressed() {
    showingYear = !showingYear;
    RefreshDisplay();
}
