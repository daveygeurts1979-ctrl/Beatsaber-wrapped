#include "WrappedFlowCoordinator.hpp"
#include "bsml/shared/BSML.hpp"

using namespace BeatSaberWrapped::UI;

void WrappedFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (firstActivation) {
        this->SetTitle(il2cpp_utils::newcsstr("Beat Saber Wrapped"), HMUI::ViewController::AnimationType::In);
        this->showBackButton = true;

        auto* viewController = BSML::Helpers::CreateViewController<WrappedViewController*>();
        this->ProvideInitialViewControllers(viewController, nullptr, nullptr, nullptr, nullptr);
    }
}

void WrappedFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* /*topViewController*/) {
    this->parentFlowCoordinator->DismissFlowCoordinator(this, nullptr, false);
}
