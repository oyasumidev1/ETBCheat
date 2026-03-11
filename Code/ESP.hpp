#pragma once
#include "SDK.hpp"

namespace Features {
    namespace ESP {
        void Render(SDK::UCanvas* Canvas, SDK::APlayerController* PC);
        void DrawBox(SDK::UCanvas* Canvas, SDK::FVector2D Top, SDK::FVector2D Bottom, SDK::FLinearColor Color);
        SDK::FVector GetBonePos(SDK::USkeletalMeshComponent* Mesh, const wchar_t* BoneName);
        void DrawSkeleton(SDK::UCanvas* Canvas, SDK::APlayerController* PC, SDK::USkeletalMeshComponent* Mesh, SDK::FLinearColor Color);
    }
}