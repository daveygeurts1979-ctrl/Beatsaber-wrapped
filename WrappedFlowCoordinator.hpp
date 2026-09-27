#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "WrappedViewController.hpp"

DECLARE_CLASS_CODEGEN(BeatSaberWrapped::UI, WrappedFlowCoordinator, HMUI::FlowCoordinator,

    DECLARE_METHOD(void, DidActivate, (bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling));
    DECLARE_METHOD(void, BackButtonWasPressed, (HMUI::ViewController* topViewController));
)
