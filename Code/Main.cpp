#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include "SDK.hpp"
#include "UI.hpp"
#include "ESP.hpp" 
#include "Misc.hpp"
#include "GodMode.hpp"

void** VTable = nullptr;
void(*OPostRender)(void* view, SDK::UCanvas* Canvas) = nullptr;
void PostRenderHook(void* view, SDK::UCanvas* Canvas) {
    if (Canvas) {
        auto World = SDK::UWorld::GetWorld();
        if (World && World->OwningGameInstance && World->OwningGameInstance->LocalPlayers.IsValidIndex(0)) {
            auto PC = World->OwningGameInstance->LocalPlayers[0]->PlayerController;

            if (PC && PC->Pawn) {

                Features::Misc::Tick(PC);
                if (UI::bESP) {
                    Features::ESP::Render(Canvas, PC);
                }

                if (UI::bGodMode) {
                    Features::GodMode::Tick(PC);
                }
                else {
                    Features::GodMode::Disable(PC);
                }
            }
        }
        UI::Render(Canvas);
    }


    return OPostRender(view, Canvas);
}

DWORD WINAPI HackThread(LPVOID lpParam) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "[*] Initializing..." << std::endl;
    std::cout << "[*] 正在等待游戏引擎加载..." << std::endl;

    SDK::UEngine* Engine = nullptr;
    while (!(Engine = SDK::UEngine::GetEngine())) {
        Sleep(100);
    }

    while (!Engine->GameViewport) {
        Sleep(100);
    }

    std::cout << "[+] 引擎就绪! Engine: " << std::hex << Engine << std::endl;

    VTable = *reinterpret_cast<void***>(Engine->GameViewport);
    if (!VTable) {
        std::cout << "[-] 获取虚表失败!" << std::endl;
        FreeConsole();
        FreeLibraryAndExitThread((HMODULE)lpParam, 0);
        return 0;
    }

    DWORD protecc;
    size_t idx = SDK::Offsets::PostRenderIdx;

    std::cout << "[*] 正在 Hook PostRender (索引: " << std::dec << idx << ")" << std::endl;

    VirtualProtect(&VTable[idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &protecc);
    OPostRender = reinterpret_cast<decltype(OPostRender)>(VTable[idx]);
    VTable[idx] = reinterpret_cast<void*>(&PostRenderHook);
    VirtualProtect(&VTable[idx], sizeof(void*), protecc, &protecc);

    std::cout << "[+] Hook 成功! 请返回游戏查看." << std::endl;
    std::cout << "[+] 按 [INSERT] 呼出/隐藏菜单" << std::endl;
    std::cout << "[+] 按 [DELETE] 安全卸载插件" << std::endl;

    while (!UI::bShouldExit) {
        Sleep(100);
    }

    std::cout << "[!] 收到退出指令，正在还原内存..." << std::endl;

    VirtualProtect(&VTable[idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &protecc);
    VTable[idx] = OPostRender;
    VirtualProtect(&VTable[idx], sizeof(void*), protecc, &protecc);

    std::cout << "[+] 还原成功，等待渲染队列清空..." << std::endl;
    Sleep(1500);

    if (f) fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread((HMODULE)lpParam, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, HackThread, hModule, 0, nullptr);
    }
    return TRUE;
}