#include "Interact.h"
#include "Logger.h"
#include <iostream>
#include <Windows.h>

cv::Mat captureScreenshot() {
    HWND hwndDBD = ::FindWindow(L"UnrealWindow", L"DeadByDaylight  ");
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    BITMAPINFO MyBMInfo = { 0 };
    MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);
    MyBMInfo.bmiHeader.biWidth = width;
    MyBMInfo.bmiHeader.biHeight = -height;
    MyBMInfo.bmiHeader.biPlanes = 1;
    MyBMInfo.bmiHeader.biBitCount = 24;
    MyBMInfo.bmiHeader.biCompression = BI_RGB;
    MyBMInfo.bmiHeader.biSizeImage = 0;
    MyBMInfo.bmiHeader.biXPelsPerMeter = 0;
    MyBMInfo.bmiHeader.biYPelsPerMeter = 0;
    MyBMInfo.bmiHeader.biClrUsed = 0;
    MyBMInfo.bmiHeader.biClrImportant = 0;

    // Create compatible DC, create a compatible bitmap and copy the screen using BitBlt()
    HDC hdcScreen = GetDC(hwndDBD);
    HDC hdcCompatible = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, width, height);
    HGDIOBJ hOldBmp = (HGDIOBJ)SelectObject(hdcCompatible, hBmp);

    BOOL bOK = BitBlt(hdcCompatible, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY | CAPTUREBLT);

    SelectObject(hdcCompatible, hOldBmp); // always select the previously selected object once done
    // Get the BITMAPINFO structure from the bitmap
    GetDIBits(hdcScreen, hBmp, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS);

    MyBMInfo.bmiHeader.biCompression = BI_RGB;
    MyBMInfo.bmiHeader.biBitCount = 24;

    // Create Mat object, which also allocates data buffer
    cv::Mat ret(height, width, CV_8UC3);
    // get the actual bitmap buffer
    GetDIBits(hdcScreen, hBmp, 0, -MyBMInfo.bmiHeader.biHeight, (LPVOID)ret.ptr(), &MyBMInfo, DIB_RGB_COLORS);

    //Clean Up
    ReleaseDC(0, hdcScreen);
    ReleaseDC(0, hdcCompatible);
    DeleteDC(hdcCompatible);
    DeleteDC(hdcScreen);
    DeleteObject(hBmp);
    DeleteObject(hOldBmp);

    return ret;
}

void mouseClick() {
    INPUT in[2];
    in[0].type = INPUT_MOUSE;
    in[0].mi.mouseData = 0;
    in[0].mi.time = 0;
    in[0].mi.dx = 0;
    in[0].mi.dy = 0;
    in[0].mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
    in[1].type = INPUT_MOUSE;
    in[1].mi.mouseData = 0;
    in[1].mi.time = 0;
    in[1].mi.dx = 0;
    in[1].mi.dy = 0;
    in[1].mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
    SendInput(2, (LPINPUT) &in, sizeof(INPUT));
}

void mouseMove(int x, int y) {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.time = 0;
    input.mi.dx = x * (0xFFFF / GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
    input.mi.dy = y * (0xFFFF / GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(input));
}

void moveAndClickDBD(int x, int y) {
    LOG_DEBUG("[MoveN'Click]: x: %d, y: %d\n", x, y);
    // Restore cursor position after selecting
    POINT p;
    GetCursorPos(&p);
    HWND dbd = FindWindow(L"UnrealWindow", L"DeadByDaylight  ");
    if (dbd == nullptr)
    {
        LOG_DEBUG("[MoveN'Click]: DBD window sanity check failed\n");
        return;
    }
    // Save foreground window so it can be restored as well
    HWND old = GetForegroundWindow();
    SetForegroundWindow(dbd);
    mouseMove(x, y);
    Sleep(75);
    mouseClick();
    Sleep(75);
    mouseMove(p.x, p.y);
    SetForegroundWindow(old);
}
