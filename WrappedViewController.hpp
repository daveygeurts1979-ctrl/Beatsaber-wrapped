#pragma once

#include "custom-types/shared/macros.hpp"
#include "bsml/shared/BSML.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(BeatSaberWrapped::UI, WrappedViewController, HMUI::ViewController,

    DECLARE_INSTANCE_FIELD(UnityEngine::UI::VerticalLayoutGroup*, listContainer);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, headerText);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, statsText);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, songListText);

    // Which period is currently displayed. When showingYear is true we ignore
    // currentMonth and show the whole currentYear instead.
    DECLARE_INSTANCE_FIELD(int, currentYear);
    DECLARE_INSTANCE_FIELD(int, currentMonth);
    DECLARE_INSTANCE_FIELD(bool, showingYear);

    DECLARE_METHOD(void, DidActivate, (bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling));

    DECLARE_METHOD(void, OnPrevPressed, ());
    DECLARE_METHOD(void, OnNextPressed, ());
    DECLARE_METHOD(void, OnToggleYearPressed, ());

public:
    void RefreshDisplay();
)
