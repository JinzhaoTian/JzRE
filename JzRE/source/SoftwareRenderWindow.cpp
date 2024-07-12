#include "SoftwareRenderWindow.h"

namespace JzRE {

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    SoftwareRenderWindow *window = static_cast<SoftwareRenderWindow *>(GetProp(hWnd, TEXT("Owner")));

    if (window == nullptr)
        return DefWindowProc(hWnd, msg, wParam, lParam);

    switch (msg) {
    case WM_QUIT:
        window->hasClosed = true;
        break;
    case WM_KEYDOWN:
        break;
    case WM_KEYUP:
        break;
    case WM_LBUTTONDOWN:
        break;
    case WM_LBUTTONUP:
        break;
    case WM_RBUTTONDOWN:
        break;
    case WM_RBUTTONUP:
        break;
    case WM_MOUSEWHEEL:
        break;
    case WM_SIZE:
        window->ResizeWindow(LOWORD(lParam), HIWORD(lParam));
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

SoftwareRenderWindow::SoftwareRenderWindow():
hwnd(NULL), wndWidth(800), wndHeight(600), title("Software/CPU Render Engine"){
    
}

SoftwareRenderWindow::~SoftwareRenderWindow() {
}

bool SoftwareRenderWindow::Initialize(I32 w, I32 h, const String &title) {
    this->wndWidth = w;
    this-> wndHeight= h; 
    this->hasClosed = false;

    frontBuffer = CreateSharedPtr<Framebuffer>(wndWidth, wndHeight);
    backBuffer = CreateSharedPtr<Framebuffer>(wndWidth, wndHeight);

    WNDCLASSEX wndClass;
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.style = CS_DBLCLKS;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon = NULL;
    wndClass.hIconSm = NULL;
    wndClass.lpfnWndProc = WndProc;
    wndClass.lpszClassName = TEXT("Test");
    wndClass.lpszMenuName = NULL;

    if (!RegisterClassEx(&wndClass))
        return false;

    hwnd = CreateWindowEx(
        0,
        TEXT("Test"),
        title.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        0, 0,
        0, 0,
        HWND_DESKTOP,
        NULL,
        wndClass.hInstance,
        NULL);

    if (hwnd == NULL)
        return false;

    BITMAPINFOHEADER biheader = {
        sizeof(BITMAPINFOHEADER),
        wndWidth, wndHeight,
        1,
        32,
        BI_RGB,
        (DWORD)wndWidth * wndHeight * 4,
        0, 0,
        0,
        0};

    BITMAPINFO bi = {
        biheader,
    };

    HDC hdc = GetDC(hwnd);
    screenHDC = CreateCompatibleDC(hdc);
    ReleaseDC(hwnd, hdc);

    HBITMAP dib = CreateDIBSection(
        screenHDC,
        &bi,
        DIB_RGB_COLORS,
        (void **)&frontBuffer->data,
        0,
        0);

    if (dib == NULL)
        return false;

    HBITMAP screenObject = (HBITMAP)SelectObject(screenHDC, dib);

    RECT rect = {0, 0, wndWidth, wndHeight};

    AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), 0);

    int wx = rect.right - rect.left;
    int wy = rect.bottom - rect.top;
    int sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
    int sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
    if (sy < 0) sy = 0;

    SetWindowPos(hwnd, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));

    ShowWindow(hwnd, SW_NORMAL);
    UpdateWindow(hwnd);

    return true;
}



bool SoftwareRenderWindow::ShouldClose() {
    return hasClosed;
}

void SoftwareRenderWindow::PollEvents() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void SoftwareRenderWindow::SwapBuffer() {
    memcpy(frontBuffer->data, backBuffer->data, wndWidth * wndHeight * sizeof(U32));

    HDC hdc = GetDC(hwnd);
    BitBlt(hdc, 0, 0, wndWidth, wndHeight, screenHDC, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}

void SoftwareRenderWindow::ClearBuffer() {
    frontBuffer->Clear();
    backBuffer->Clear();
}

void SoftwareRenderWindow::ResizeWindow(I32 w, I32 h) {
    wndWidth = w, wndHeight = h;

    frontBuffer->Resize(w, h);
    backBuffer->Resize(w, h);
}

} // namespace JzRE