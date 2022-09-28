#include "App.h"
#include <iostream>
#include "CVController.h"
#include "PerkEquipper.h"
#include "Logger.h"

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Show the main window
bool showSearch = false;
// Focus filter input first time window opens
bool focusFlag = false;
// Pointer that ImGui radio buttons will change
int* logLevel = (int*) Logger::getLogLevel();

// Main code
int main(int argc, char** argv)
{
    // Default log level is INFO
    *logLevel = Logger::INFO;
    std::cout << "\n\tFogBuddy console(don't close).\n\n";
    PerkEquipper perkEquipper;
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();

    // Create window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, CreateSolidBrush(RGB(0,0,0)), NULL, _T("ImGui Example"), NULL};
    RegisterClassEx(&wc);
    // Initial window size. Later on it always adjusts to DBD's size.
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    // Create window
    HWND hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, wc.lpszClassName, _T("FogBuddy overlay"), WS_POPUP, 0, 0, width, height, NULL, NULL, wc.hInstance, NULL);
    SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, LWA_COLORKEY); // Makes it so window bg is transparent

    // Grab DBD handle
    HWND hwndDBD = FindWindow(_T("UnrealWindow"), _T("DeadByDaylight  "));
    if (hwndDBD == nullptr)
    {
        LOG_INFO("Waiting for Dead by Daylight window.\n");
        while (hwndDBD == nullptr)
        {
            Sleep(2000);
            hwndDBD = FindWindow(_T("UnrealWindow"), _T("DeadByDaylight  "));
        }
        LOG_INFO("Window found. Starting.\n");
    }

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        //::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    
    // Show the window
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);

    // Create hotkey to bring up the overlay (global)
    RegisterHotKey(hwnd, ID_OPEN_POPUP, MOD_ALT, 0x53); // alt-S

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Only show overlay if DBD is the foreground window
        HWND fg = GetForegroundWindow();
        bool isDBDForeground = fg == hwndDBD || fg == hwnd;

        // Adjust overlay to DBD window
        // For proper windowed mode support, window screenshots and CVController offsets must be implemented as well
        RECT rect;
        GetWindowRect(hwndDBD, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        MoveWindow(hwnd, rect.left, rect.top, width, height, true);

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

        ImVec2 size = { 500, 400 };
        ImGui::SetNextWindowSize(size);
        
        if (isDBDForeground && showSearch && ImGui::Begin("FogBuddy perk search", &showSearch, window_flags))
        {
            // Recalibrate creates a new CVController (recalculate pages and perk size)
            if (ImGui::Button("Recalibrate"))
            {
                perkEquipper.recalibrate();
            }
            ImGui::SameLine();

            // Selectors for log level
            ImGui::RadioButton("None", logLevel, Logger::NONE); ImGui::SameLine();
            ImGui::RadioButton("Info", logLevel, Logger::INFO); ImGui::SameLine();
            ImGui::RadioButton("Debug", logLevel, Logger::DEBUG);

            // Input box and filtering for perks list
            static ImGuiTextFilter filter;
            filter.Draw();
            
            // Focus input if window just opened
            if (focusFlag) {
                LOG_DEBUG("Focusing\n");
                SetForegroundWindow(hwnd);
                ImGui::SetKeyboardFocusHere(-1);
                focusFlag = false;
            }

            // Choose between sides
            static int sideToggle = 0;
            ImGui::RadioButton("Killer", &sideToggle, 0); ImGui::SameLine();
            ImGui::RadioButton("Survivor", &sideToggle, 1); ImGui::SameLine();
            
            // Optionally filter perks/characters out
            static bool includeCharacters = true;
            ImGui::Checkbox("Show characters", &includeCharacters); ImGui::SameLine();
            static bool includePerks = true;
            ImGui::Checkbox("Show perks", &includePerks);
            
            // Add in perks from either side
            std::vector<std::string> perksList;
            std::map<std::string, std::vector<std::string>> characters;
            std::string basePath;
            switch(sideToggle)
            {
            case 0: // Killer
                perksList = perkEquipper.killerPerks;
                characters = perkEquipper.killers;
                basePath = "data\\Killers\\";
                break;
            case 1: // Survivor
                perksList = perkEquipper.survivorPerks;
                characters = perkEquipper.survivors;
                basePath = "data\\Survivors\\";
                break;
            default:
                LOG_DEBUG("Side toggle sanity check failed.\n");
                continue;
            }
            if (includePerks)
            {
                for (auto& it = perksList.begin(); it != perksList.end(); it++)
                {
                    std::string s = PerkEquipper::nameFromPathstr(*it);
                    const char* cstr = s.c_str();
                    if (filter.PassFilter(cstr)) {
                        // Each perk is a menu item
                        if (ImGui::MenuItem(cstr)) {
                            perkEquipper.equipPerk(*it, true);
                        }
                    }
                }

            }
            if (includeCharacters)
            {
                // Loop through all mappings
                // character name -> vector<character perk names>
                for (auto& it = characters.begin(); it != characters.end(); it++)
                {
                    std::string character = it->first;
                    std::vector<std::string> perks = it->second;
                    const char* cstr = character.c_str();
                    if (filter.PassFilter(cstr))
                    {
                        // Each killer is a menu
                        if (ImGui::BeginMenu(cstr)) {
                            for (auto& it = perks.begin(); it != perks.end(); it++)
                            {
                                std::string s = (*it).substr(0, (*it).find_first_of('.'));
                                const char* cstr = s.c_str();
                                // that contains their teachables
                                if (ImGui::MenuItem(cstr)) {
                                    perkEquipper.equipPerk(basePath + character + "\\" + *it, true);
                                }
                            }
                            ImGui::EndMenu();
                        }
                    }
                }
            }
            ImGui::End();
        }
        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0,0,0,0 };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_HOTKEY:
        switch (LOWORD(wParam))
        {
        case ID_OPEN_POPUP:
            showSearch = true;
            focusFlag = true;
            break;
        }
        return 0;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}