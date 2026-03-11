#include "GodMode.hpp"

void Features::GodMode::Tick(SDK::APlayerController* PC) {
    if (!PC || !PC->Pawn) return;

    if (PC->Pawn->IsA(SDK::ACharacter::StaticClass())) {
        SDK::ACharacter* Character = static_cast<SDK::ACharacter*>(PC->Pawn);

        if (Character->CapsuleComponent) {
            Character->CapsuleComponent->SetGenerateOverlapEvents(false);
            Character->CapsuleComponent->SetCollisionResponseToChannel(SDK::ECollisionChannel::ECC_Pawn, SDK::ECollisionResponse::ECR_Ignore);
        }
    }
}

void Features::GodMode::Disable(SDK::APlayerController* PC) {
    if (!PC || !PC->Pawn) return;

    if (PC->Pawn->IsA(SDK::ACharacter::StaticClass())) {
        SDK::ACharacter* Character = static_cast<SDK::ACharacter*>(PC->Pawn);

        if (Character->CapsuleComponent) {
            Character->CapsuleComponent->SetGenerateOverlapEvents(true);
            Character->CapsuleComponent->SetCollisionResponseToChannel(SDK::ECollisionChannel::ECC_Pawn, SDK::ECollisionResponse::ECR_Block);
        }
    }
}