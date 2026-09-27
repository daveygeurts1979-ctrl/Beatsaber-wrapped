#include "scotland2/shared/modloader.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"

#include "WrappedManager.hpp"
#include "GameplayHooks.hpp"
#include "WrappedFlowCoordinator.hpp"
#include "WrappedViewController.hpp"
#include "Logging.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

using namespace GlobalNamespace;

// VERIFY AGAINST YOUR MODLOADER VERSION: this is the standard Scotland2
// entry-point pattern (setup() + late_load(), a ModInfo struct converted with
// .to_c()). If your restored `paper2_scotland2`/loader version names this
// differently, match whatever your other working Scotland2 mods use here —
// it's a few lines and doesn't touch any of the tracking/UI logic below.
static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

static HMUI::FlowCoordinator* GetOrCreateWrappedFlowCoordinator() {
    static SafePtrUnity<BeatSaberWrapped::UI::WrappedFlowCoordinator> flowCoordinator;
    if (!flowCoordinator) {
        flowCoordinator = BSML::Helpers::CreateFlowCoordinator<BeatSaberWrapped::UI::WrappedFlowCoordinator*>();
    }
    return flowCoordinator.ptr();
}

// ---- Add a "Wrapped" button to the main menu ------------------------------
// VERIFY AGAINST YOUR CODEGEN / BSML VERSION: `MainMenuViewController::DidActivate`'s
// parameter list and `BSML::Lite::CreateUIButton`'s exact overload have both
// shifted slightly across Beat Saber / BSML releases. The logic (hook menu
// activation, drop a button next to the existing ones, open the flow
// coordinator on click) is the stable part; only the two signatures below
// might need adjusting to match what `qpm restore` gives you.
MAKE_HOOK_MATCH(MainMenuViewController_DidActivate,
                &MainMenuViewController::DidActivate,
                void,
                MainMenuViewController* self,
                bool firstActivation,
                bool addedToHierarchy,
                bool screenSystemEnabling) {

    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    if (!firstActivation) return;

    BSML::Lite::CreateUIButton(
        self->get_transform(),
        "Wrapped",
        UnityEngine::Vector2(0.0f, -64.0f),   // anchored position; nudge to taste
        UnityEngine::Vector2(30.0f, 10.0f),   // size
        []() {
            auto* mainFlow = BSML::Helpers::GetMainFlowCoordinator();
            mainFlow->PresentFlowCoordinator(
                GetOrCreateWrappedFlowCoordinator(), nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false, false);
        });
}

extern "C" void setup(CModInfo* info) {
    *info = modInfo.to_c();
    BeatSaberWrapped::getLogger().info("Beat Saber Wrapped setup complete");
}

extern "C" void late_load() {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();

    BeatSaberWrapped::WrappedManager::Get().Init();
    BeatSaberWrapped::Hooks::InstallGameplayHooks();

    INSTALL_HOOK(BeatSaberWrapped::getLogger(), MainMenuViewController_DidActivate);

    BeatSaberWrapped::getLogger().info("Beat Saber Wrapped loaded");
}
