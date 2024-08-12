#include <windows.h>
#include <windowsx.h> 
#include <fstream>
#include <strsafe.h>
#include <iostream>
#include <shellapi.h>

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

const TCHAR g_szClassName[] = TEXT("myWindowClass");

#define ID_TRAY_EXIT  1001
#define ID_TRAY_ICON   1002

NOTIFYICONDATA nid;
PROCESS_INFORMATION pi;

void ExecuteCMD(const TCHAR* cmd);

HHOOK hMouseHook;

std::ofstream myfile;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    myfile.open(".//webapp//data.txt");
    myfile << "start";
    myfile.close();

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_APPWINDOW,
        g_szClassName,                 
        TEXT(""), 
        WS_POPUP,                     
        0, 0,                        
        10, 10,     
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_COLORKEY | LWA_ALPHA);

    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1; 
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    lstrcpy(nid.szTip, TEXT(""));

    Shell_NotifyIcon(NIM_ADD, &nid);

    ExecuteCMD(TEXT("cd webapp && npm install && npm start"));

    ShowWindow(hwnd, SW_HIDE); 
    UpdateWindow(hwnd);

    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
    if (!hMouseHook) {
        return 0;
    }

    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        if (Msg.message == WM_USER + 1) {
            if (LOWORD(Msg.lParam) == WM_RBUTTONDOWN) {
                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Quit"));

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
            }
        }
        else if (Msg.message == WM_COMMAND) {
            if (LOWORD(Msg.wParam) == ID_TRAY_EXIT) {
                std::ofstream myfile;
                myfile.open(".//webapp//data.txt");
                myfile << "quit";
                myfile.close();
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            }
        }
        else {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }

    UnhookWindowsHookEx(hMouseHook);

    return Msg.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CLOSE:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    case WM_USER + 1:
        if (LOWORD(lParam) == WM_RBUTTONDOWN) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Quit"));

            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

            PostMessage(hwnd, WM_NULL, 0, 0);
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        switch (wParam) {
        case WM_LBUTTONDOWN: {
            MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
            if (pMouseStruct != NULL) {
                int xPos = pMouseStruct->pt.x;
                int yPos = pMouseStruct->pt.y;
                myfile.open(".//webapp//data.txt");
                myfile << "X:" << xPos << '\n' << "Y:" << yPos;
                myfile.close();
            }
            break;
        }
        }
    }
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void ExecuteCMD(const TCHAR* cmd)
{
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; 

    ZeroMemory(&pi, sizeof(pi));

    TCHAR szCmdLine[256];
    StringCchCopy(szCmdLine, 256, TEXT("cmd.exe /C "));
    StringCchCat(szCmdLine, 256, cmd);

    if (!CreateProcess(NULL,  
        szCmdLine,      
        NULL,           
        NULL,           
        FALSE,         
        CREATE_NO_WINDOW, 
        NULL,           
        NULL,          
        &si,           
        &pi)           
        )
    {
        MessageBox(NULL, TEXT("CreateProcess failed"), TEXT("Error"), MB_OK);
        return;
    }
}