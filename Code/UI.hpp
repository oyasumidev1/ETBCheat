#pragma once
#include <Windows.h>
#include <string>
#include "SDK.hpp"

namespace UI {
    inline bool bShowMenu = true;
    inline bool bShouldExit = false;
    inline bool bESP = true;
    inline bool bAimbot = false;
    inline bool bNoRecoil = false;
    inline bool bNames = true;
    inline bool bSkeleton = false;
    inline bool bGodMode = false;
    inline bool bInfiniteStamina = false;
    inline bool bInfiniteSanity = false;
    inline bool bNoFall = false;
    inline bool bDisableAC = false;
    inline bool bTracers = false;
    inline bool bItemESP = true;
    inline int MenuIndex = 0;
    inline SDK::FVector SavedPos = { 0, 0, 0 };

    void Render(SDK::UCanvas* Canvas);

    void DrawTextWithShadow(SDK::UCanvas* Canvas, const wchar_t* Text, SDK::FVector2D Pos, SDK::FLinearColor Col, bool Center);
    void FillRect(SDK::UCanvas* Canvas, SDK::FVector2D Pos, float W, float H, SDK::FLinearColor Col);
}