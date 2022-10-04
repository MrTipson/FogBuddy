#pragma once
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <d3d11.h>
#include <tchar.h>

#define ID_OPEN_POPUP 40001

int main(int argc, char** argv);
bool selectPerkSlot(int* perkSlot);