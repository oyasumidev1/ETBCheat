#include "Misc.hpp"
#include "UI.hpp"

void Features::Misc::Tick(SDK::APlayerController* PC) {
    if (!PC) return;
    auto World = SDK::UWorld::GetWorld();
    if (!World) return;

    if (PC->Pawn && PC->Pawn->IsA(SDK::ABPCharacter_Demo_C::StaticClass())) {
        auto MyPlayer = static_cast<SDK::ABPCharacter_Demo_C*>(PC->Pawn);
        if (UI::bInfiniteStamina) {
            MyPlayer->ShouldUseStamina = false;
        }
        else {
            MyPlayer->ShouldUseStamina = true;
        }
    }

    if (PC->PlayerState && PC->PlayerState->IsA(SDK::AFancyPlayerState::StaticClass())) {
        auto MyState = static_cast<SDK::AFancyPlayerState*>(PC->PlayerState);

        if (UI::bInfiniteSanity) {
            MyState->Sanity = MyState->MaxSanity;
            MyState->ShouldLowerSanity = false;
        }
        else {
            MyState->ShouldLowerSanity = true;
        }
    }

    if (PC->Pawn && PC->Pawn->IsA(SDK::ABPCharacter_Demo_C::StaticClass())) {
        auto MyPlayer = static_cast<SDK::ABPCharacter_Demo_C*>(PC->Pawn);
        if (UI::bNoFall) {
            MyPlayer->HasFallDamage = false;
        }
        else {
            MyPlayer->HasFallDamage = true;
        }
    }
    
}
