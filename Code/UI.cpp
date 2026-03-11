#include "UI.hpp"
#include "ZeroGUI.h"
#include <string>

static SDK::FVector2D MenuPos{ 150.0f, 150.0f };
static float TeleportX = 0.0f;
static float TeleportY = 0.0f;
static float TeleportZ = 0.0f;
static bool menu_opened = false;

void UI::Render(SDK::UCanvas* Canvas) {
    auto World = SDK::UWorld::GetWorld();
    auto Engine = SDK::UEngine::GetEngine();
    if (!World || !Engine || !Canvas) return;

    auto PC = World->OwningGameInstance->LocalPlayers[0]->PlayerController;
    if (!PC) return;

    static bool ins_pressed = false;
    if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
        if (!ins_pressed) {
            menu_opened = !menu_opened;
            ins_pressed = true;
        }
    }
    else {
        ins_pressed = false;
    }

    
    static bool del_pressed = false;
    if (GetAsyncKeyState(VK_DELETE) & 0x8000) {
        if (!del_pressed) {
            UI::bShouldExit = true;
            del_pressed = true;
        }
    }
    else {
        del_pressed = false;
    }



    PC->bShowMouseCursor = menu_opened;
    if (menu_opened) {
        PC->SetIgnoreLookInput(true); 
    }
    else {
        PC->SetIgnoreLookInput(false);
    }

    if (PC->Pawn) {
        SDK::FVector LocalPos = PC->Pawn->K2_GetActorLocation();
        std::string coordStr = "POS - X: " + std::to_string((int)LocalPos.X) +
            " Y: " + std::to_string((int)LocalPos.Y) +
            " Z: " + std::to_string((int)LocalPos.Z);

        std::wstring wCoord(coordStr.begin(), coordStr.end());
        SDK::FVector2D Scale{ 1.1f, 1.1f };
        Canvas->K2_DrawText(Engine->MediumFont, SDK::FString(wCoord.c_str()), { 20.0f, 20.0f }, Scale, { 0, 1, 1, 1 }, 1.0f, { 0,0,0,1 }, Scale, false, false, true, { 0,0,0,1 });
    }

    if (!menu_opened) return;

    ZeroGUI::SetupCanvas(Canvas);
    ZeroGUI::Input::Handle();

    if (ZeroGUI::Window("ETB CHEAT", &MenuPos, SDK::FVector2D{ 600.0f, 450.0f }, menu_opened)) {

        static int tab = 0;
        if (ZeroGUI::ButtonTab("Visual", SDK::FVector2D{ 130, 30 }, tab == 0)) tab = 0;
        if (ZeroGUI::ButtonTab("Teleport", SDK::FVector2D{ 130, 30 }, tab == 1)) tab = 1;
        if (ZeroGUI::ButtonTab("Misc", SDK::FVector2D{ 130, 30 }, tab == 2)) tab = 2;

        ZeroGUI::NextColumn(150.0f);

        
        if (tab == 0) {
            ZeroGUI::Text("Visual Settings");
            ZeroGUI::PushNextElementY(20.0f);
            ZeroGUI::Checkbox("ESP", &UI::bESP);
            ZeroGUI::Checkbox("Show Names", &UI::bNames);
            ZeroGUI::Checkbox("Show Skeleton", &UI::bSkeleton);
            ZeroGUI::Checkbox("Show Tracers", &UI::bTracers);
            ZeroGUI::Checkbox("Show Items/Doors", &UI::bItemESP);
        }

        if (tab == 1) {
            ZeroGUI::Text("Teleportation System");
            ZeroGUI::PushNextElementY(20.0f);

            if (ZeroGUI::Button("Copy Current Position", SDK::FVector2D{ 300, 35 })) {
                if (PC->Pawn) {
                    SDK::FVector p = PC->Pawn->K2_GetActorLocation();
                    TeleportX = p.X;
                    TeleportY = p.Y;
                    TeleportZ = p.Z;
                }
            }

            ZeroGUI::PushNextElementY(15.0f);

            ZeroGUI::SliderFloat("Target X", &TeleportX, -300000.0f, 300000.0f);
            ZeroGUI::SliderFloat("Target Y", &TeleportY, -300000.0f, 300000.0f);
            ZeroGUI::SliderFloat("Target Z", &TeleportZ, -50000.0f, 50000.0f);

            ZeroGUI::PushNextElementY(20.0f);

            if (ZeroGUI::Button("Do Teleport", SDK::FVector2D{ 300, 40 })) {
                if (PC->Pawn) {
                    SDK::FVector Dest = { TeleportX, TeleportY, TeleportZ };
                    PC->Pawn->K2_SetActorLocation(Dest, false, nullptr, true);
                }
            }
        }

        if (tab == 2) {
            ZeroGUI::Checkbox("GodMode", &UI::bGodMode);
            ZeroGUI::Checkbox("Infinite Stamina", &UI::bInfiniteStamina);
            ZeroGUI::Checkbox("Infinite Sanity", &UI::bInfiniteSanity);
            ZeroGUI::Checkbox("No Fall Damage", &UI::bNoFall);
        }
    }

    ZeroGUI::Render();
    ZeroGUI::Draw_Cursor(menu_opened);
}