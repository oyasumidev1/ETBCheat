#pragma execution_character_set("utf-8") 

#include "ESP.hpp"
#include "UI.hpp"
#include <string>
#include <windows.h> 

std::wstring Utf8ToWstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string CleanItemName(const std::string& rawName, bool& outIsItem) {
    outIsItem = true;
    // 1. 物品类
    if (rawName.find("KeyPiece") != std::string::npos) return "钥匙";
    if (rawName.find("Almond") != std::string::npos) return "杏仁水";
    if (rawName.find("Tape") != std::string::npos) return "录像带";
    if (rawName.find("Flashlight") != std::string::npos) return "手电筒";
    if (rawName.find("Battery") != std::string::npos) return "电池";
    if (rawName.find("Door") != std::string::npos) return "门";
    if (rawName.find("Valve") != std::string::npos) return "阀门";
    if (rawName.find("Button") != std::string::npos) return "按钮";
    //if (rawName.find("Item") != std::string::npos) return "未知物品";

    outIsItem = false;

    // 2. 怪物类
    if (rawName.find("Bacteria") != std::string::npos) return "细菌";
    if (rawName.find("Smiler") != std::string::npos) return "笑脸怪";
    if (rawName.find("Hound") != std::string::npos) return "猎犬";
    if (rawName.find("SkinStealer") != std::string::npos) return "窃皮";


    // 3. 玩家类
    if (rawName.find("BPCharacter_Demo") != std::string::npos) return "队友 (Player)";

    return rawName;
}

// 空心方框
void Features::ESP::DrawBox(SDK::UCanvas* Canvas, SDK::FVector2D Top, SDK::FVector2D Bottom, SDK::FLinearColor Color) {
    float Height = Bottom.Y - Top.Y;
    float Width = Height * 0.45f;
    SDK::FVector2D TopLeft{ Top.X - Width / 2, Top.Y };
    SDK::FVector2D BottomRight{ Bottom.X + Width / 2, Bottom.Y };

    Canvas->K2_DrawLine(TopLeft, { BottomRight.X, TopLeft.Y }, 1.5f, Color);
    Canvas->K2_DrawLine({ BottomRight.X, TopLeft.Y }, BottomRight, 1.5f, Color);
    Canvas->K2_DrawLine(BottomRight, { TopLeft.X, BottomRight.Y }, 1.5f, Color);
    Canvas->K2_DrawLine({ TopLeft.X, BottomRight.Y }, TopLeft, 1.5f, Color);
}

SDK::FVector Features::ESP::GetBonePos(SDK::USkeletalMeshComponent* Mesh, const wchar_t* BoneName) {
    if (!Mesh) return { 0, 0, 0 };
    SDK::FName Name = SDK::UKismetStringLibrary::GetDefaultObj()->Conv_StringToName(SDK::FString(BoneName));
    return Mesh->GetSocketLocation(Name);
}

// 绘制人物骨架
void Features::ESP::DrawSkeleton(SDK::UCanvas* Canvas, SDK::APlayerController* PC, SDK::USkeletalMeshComponent* Mesh, SDK::FLinearColor Color) {
    if (!Mesh) return;
    const wchar_t* BoneNames[] = {
        L"head", L"neck_01", L"spine_02", L"pelvis",
        L"upperarm_l", L"lowerarm_l", L"hand_l",
        L"upperarm_r", L"lowerarm_r", L"hand_r",
        L"thigh_l", L"calf_l", L"foot_l",
        L"thigh_r", L"calf_r", L"foot_r"
    };

    SDK::FVector2D Bones2D[16];
    bool bValid[16];

    for (int i = 0; i < 16; i++) {
        SDK::FVector Bone3D = GetBonePos(Mesh, BoneNames[i]);
        bValid[i] = PC->ProjectWorldLocationToScreen(Bone3D, &Bones2D[i], false);
    }

    auto DrawBoneLine = [&](int Start, int End) {
        if (bValid[Start] && bValid[End]) {
            Canvas->K2_DrawLine(Bones2D[Start], Bones2D[End], 1.5f, Color);
        }
        };

    DrawBoneLine(0, 1); DrawBoneLine(1, 2); DrawBoneLine(2, 3);
    DrawBoneLine(1, 4); DrawBoneLine(4, 5); DrawBoneLine(5, 6);
    DrawBoneLine(1, 7); DrawBoneLine(7, 8); DrawBoneLine(8, 9);
    DrawBoneLine(3, 10); DrawBoneLine(10, 11); DrawBoneLine(11, 12);
    DrawBoneLine(3, 13); DrawBoneLine(13, 14); DrawBoneLine(14, 15);
}

void Features::ESP::Render(SDK::UCanvas* Canvas, SDK::APlayerController* PC) {
    if (!Canvas || !PC || !PC->Pawn) return;

    auto World = SDK::UWorld::GetWorld();
    if (!World || !World->PersistentLevel) return;

    SDK::FVector MyPos = PC->Pawn->K2_GetActorLocation();
    auto Level = World->PersistentLevel;

    SDK::FVector2D ScreenCenter{ Canvas->SizeX / 2.0f, Canvas->SizeY / 2.0f };

    for (int i = 0; i < Level->Actors.Num(); i++) {
        auto Actor = Level->Actors[i];
        if (!Actor || Actor == PC->Pawn) continue;

        std::string rawName = Actor->GetName();
        bool bIsItem = false;

        std::string friendlyName = CleanItemName(rawName, bIsItem);

        // 1. 物品与交互道具透视 (蓝色)
        if (UI::bItemESP && bIsItem) {
            SDK::FVector ItemPos = Actor->K2_GetActorLocation();
            SDK::FVector2D ScreenPos;

            if (PC->ProjectWorldLocationToScreen(ItemPos, &ScreenPos, false)) {
                float Distance = MyPos.GetDistanceToInMeters(ItemPos);

                if (Distance < 1000.0f) {
                    std::string itemText = "[*] " + friendlyName + " [" + std::to_string((int)Distance) + "m]";
                    std::wstring wText = Utf8ToWstring(itemText);

                    SDK::FLinearColor ItemColor = { 0.2f, 0.6f, 1.0f, 1.0f };
                    Canvas->K2_DrawText(SDK::UEngine::GetEngine()->MediumFont, SDK::FString(wText.c_str()), ScreenPos, { 1.0f, 1.0f }, ItemColor, 1.0f, { 0,0,0,1 }, { 1,1 }, true, true, true, { 0,0,0,1 });
                }
            }
            continue;
        }

        // 2. 玩家与怪物透视
        if (UI::bESP && Actor->IsA(SDK::ACharacter::StaticClass())) {
            SDK::ACharacter* Character = static_cast<SDK::ACharacter*>(Actor);
            if (!Character->Mesh) continue;

            SDK::FVector RootPos = Actor->K2_GetActorLocation();
            SDK::FVector HeadPos = GetBonePos(Character->Mesh, L"head");

            if (HeadPos.X == 0 && HeadPos.Y == 0) {
                HeadPos = RootPos; HeadPos.Z += 90.0f;
            }
            else {
                HeadPos.Z += 15.0f;
            }

            SDK::FVector2D ScreenTop, ScreenBottom;
            if (PC->ProjectWorldLocationToScreen(HeadPos, &ScreenTop, false) &&
                PC->ProjectWorldLocationToScreen(RootPos, &ScreenBottom, false)) {

                float Distance = MyPos.GetDistanceToInMeters(RootPos);
                if (Distance > 1000.0f) continue;

                SDK::FLinearColor ESPColor;
                if (Actor->IsA(SDK::ABPCharacter_Demo_C::StaticClass())) {
                    ESPColor = { 0.0f, 1.0f, 0.0f, 1.0f };
                }
                else {
                    ESPColor = { 1.0f, 0.0f, 0.0f, 1.0f }; 
                }

                DrawBox(Canvas, ScreenTop, ScreenBottom, ESPColor);

                if (UI::bTracers) {
                    Canvas->K2_DrawLine(ScreenCenter, ScreenBottom, 1.0f, ESPColor);
                }

                if (UI::bSkeleton) {
                    DrawSkeleton(Canvas, PC, Character->Mesh, ESPColor);
                }

                if (UI::bNames) {
                    std::string displayText = friendlyName + " [" + std::to_string((int)Distance) + "m]";
                    std::wstring wText = Utf8ToWstring(displayText);
                    SDK::FVector2D TextPos{ ScreenTop.X, ScreenTop.Y - 15.0f };

                    Canvas->K2_DrawText(SDK::UEngine::GetEngine()->MediumFont, SDK::FString(wText.c_str()), TextPos, { 1.1f, 1.1f }, ESPColor, 1.0f, { 0,0,0,1 }, { 1, 1 }, true, true, true, { 0,0,0,1 });
                }
            }
        }
    }
}