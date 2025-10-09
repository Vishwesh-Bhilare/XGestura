#include <Windows.h>

// custom header files
#include "win32-handler.h"

// call-back function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// window size macros
#define WINDOW_WIDTH  440
#define WINDOW_HEIGHT 160

// global handle to window
HWND ghwnd;

// created buttons related
HWND hRecordActionsButtonWnd;
HWND hStopRecordingActionsButtonWnd;
HWND hPerformRecordedActionsButtonWnd;

// global linked list state variables
BOOL gIsRecording = FALSE;
ActionNode *gpHead = NULL;
ActionNode *gpTail = NULL;

// hook handles for global recording (keyboard + mouse)
HHOOK gKeyboardHook = NULL;
HHOOK gMouseHook = NULL;

// handle of the window that was active during recording
HWND gRecordedTargetWindow = NULL;

// forward declarations for hook callback functions
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
    int initialize(void);

    WNDCLASSEX wndclass;
    TCHAR szClassName[] = TEXT("MyWindowClass");
    TCHAR szWindowName[] = TEXT("My-Window");
    HWND hwnd;
    MSG msg;

    ZeroMemory((void*)&wndclass, sizeof(WNDCLASSEX));

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpszClassName = szClassName;
    wndclass.hInstance = hInstance;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(APP_ICON));
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(APP_ICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszMenuName = NULL;

    RegisterClassEx(&wndclass);

    hwnd = CreateWindow(szClassName,
                        szWindowName,
                        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        WINDOW_WIDTH,
                        WINDOW_HEIGHT,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    ghwnd = hwnd;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    if (initialize())
    {
        MessageBox(NULL, TEXT("Failed to initialize the program"), TEXT("EXIT_ERROR"), MB_ICONERROR | MB_OK);
        DestroyWindow(hwnd);
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    FreeRecordedActions();

    if (gKeyboardHook)
    {
        UnhookWindowsHookEx(gKeyboardHook);
        gKeyboardHook = NULL;
    }

    if (gMouseHook)
    {
        UnhookWindowsHookEx(gMouseHook);
        gMouseHook = NULL;
    }

    return ((int)msg.wParam);
}

int initialize(void)
{
    Sleep(50);

    hRecordActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),
        TEXT("Record Actions"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        120,
        100,
        ghwnd,
        (HMENU)IDM_RECORD_ACTIONS,
        GetModuleHandle(NULL),
        NULL);

    hStopRecordingActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),
        TEXT("Stop Recording"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10 + 140,
        10,
        120,
        100,
        ghwnd,
        (HMENU)IDM_STOP_RECORD_ACTIONS,
        GetModuleHandle(NULL),
        NULL);

    hPerformRecordedActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),
        TEXT("Perform Actions"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10 + 140 + 140,
        10,
        120,
        100,
        ghwnd,
        (HMENU)IDM_PERFORM_ACTIONS,
        GetModuleHandle(NULL),
        NULL);

    return (0);
}

void AddAction(INPUT in)
{
    ActionNode *node = (ActionNode*)malloc(sizeof(ActionNode));
    if (!node)
    {
        return;
    }

    node->input = in;
    node->next = NULL;

    if (!gpHead)
    {
        gpHead = gpTail = node;
    }
    else
    {
        gpTail->next = node;
        gpTail = node;
    }
}

void RemoveLastNActions(int n)
{
    if (!gpHead || n <= 0)
    {
        return;
    }

    ActionNode *temp = gpHead;
    ActionNode *prev = NULL;
    int count = 0;

    while (temp)
    {
        count++;
        temp = temp->next;
    }

    if (n >= count)
    {
        FreeRecordedActions();
        return;
    }

    temp = gpHead;
    for (int i = 0; i < count - n; i++)
    {
        prev = temp;
        temp = temp->next;
    }

    prev->next = NULL;

    while (temp)
    {
        ActionNode *del = temp;
        temp = temp->next;
        free(del);
    }

    gpTail = prev;
}

void PlayRecordedActions(void)
{
    if (!gpHead)
    {
        MessageBox(ghwnd, TEXT("No actions recorded!"), TEXT("INFO"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    ShowWindow(ghwnd, SW_MINIMIZE);
    Sleep(50);

    if (gRecordedTargetWindow)
    {
        // Ask user to click on target window to give focus
        MessageBox(ghwnd, TEXT("Click on the target window now, then press OK to continue."), TEXT("INFO"), MB_OK);

        // Ensure the window is foreground after user interaction
        DWORD fgThread = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
        DWORD targetThread = GetWindowThreadProcessId(gRecordedTargetWindow, NULL);

        AttachThreadInput(GetCurrentThreadId(), fgThread, TRUE);
        AttachThreadInput(GetCurrentThreadId(), targetThread, TRUE);

        SetForegroundWindow(gRecordedTargetWindow);
        SetFocus(gRecordedTargetWindow);

        AttachThreadInput(GetCurrentThreadId(), fgThread, FALSE);
        AttachThreadInput(GetCurrentThreadId(), targetThread, FALSE);

        Sleep(50); // allow the window to settle
    }

    RemoveLastNActions(3);

    ActionNode *temp = gpHead;
    while (temp)
    {
        SendInput(1, &temp->input, sizeof(INPUT));
        Sleep(50);
        temp = temp->next;
    }

    MessageBox(ghwnd, TEXT("Playback completed!"), TEXT("INFO"), MB_OK | MB_ICONINFORMATION);
}

void FreeRecordedActions(void)
{
    ActionNode *temp;
    while (gpHead)
    {
        temp = gpHead;
        gpHead = gpHead->next;
        free(temp);
    }
    gpTail = NULL;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && gIsRecording)
    {
        KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT *)lParam;
        INPUT in = {0};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = (WORD)pKeyboard->vkCode;

        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
        {
            in.ki.dwFlags = KEYEVENTF_KEYUP;
        }
        else
        {
            in.ki.dwFlags = 0;
        }

        AddAction(in);
    }

    return CallNextHookEx(gKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && gIsRecording)
    {
        MSLLHOOKSTRUCT *pMouse = (MSLLHOOKSTRUCT *)lParam;
        INPUT in = {0};
        in.type = INPUT_MOUSE;

        if (wParam == WM_LBUTTONDOWN)
        {
            in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            AddAction(in);
        }
        else if (wParam == WM_LBUTTONUP)
        {
            in.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            AddAction(in);
        }
        else if (wParam == WM_RBUTTONDOWN)
        {
            in.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            AddAction(in);
        }
        else if (wParam == WM_RBUTTONUP)
        {
            in.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            AddAction(in);
        }
        else if (wParam == WM_MBUTTONDOWN)
        {
            in.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            AddAction(in);
        }
        else if (wParam == WM_MBUTTONUP)
        {
            in.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            AddAction(in);
        }
    }

    return CallNextHookEx(gMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        case WM_DESTROY:
        {
            if (gKeyboardHook)
            {
                UnhookWindowsHookEx(gKeyboardHook);
                gKeyboardHook = NULL;
            }

            if (gMouseHook)
            {
                UnhookWindowsHookEx(gMouseHook);
                gMouseHook = NULL;
            }

            PostQuitMessage(0);
            break;
        }

        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            if (gIsRecording)
            {
                INPUT in = {0};
                in.type = INPUT_KEYBOARD;
                in.ki.wVk = (WORD)wParam;
                in.ki.dwFlags = (iMsg == WM_KEYUP) ? KEYEVENTF_KEYUP : 0;
                AddAction(in);
            }
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        {
            if (gIsRecording)
            {
                INPUT in = {0};
                in.type = INPUT_MOUSE;
                in.mi.dwFlags = (iMsg == WM_LBUTTONDOWN) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
                AddAction(in);
            }
            break;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDM_RECORD_ACTIONS:
                {
                    gIsRecording = TRUE;
                    FreeRecordedActions();

                    gRecordedTargetWindow = GetForegroundWindow();

                    gKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
                    gMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

                    ShowWindow(ghwnd, SW_MINIMIZE);

                    MessageBox(hwnd, TEXT("Recording started! The window is minimized."), TEXT("INFO"), MB_OK);
                    break;
                }

                case IDM_STOP_RECORD_ACTIONS:
                {
                    gIsRecording = FALSE;

                    if (gKeyboardHook)
                    {
                        UnhookWindowsHookEx(gKeyboardHook);
                        gKeyboardHook = NULL;
                    }

                    if (gMouseHook)
                    {
                        UnhookWindowsHookEx(gMouseHook);
                        gMouseHook = NULL;
                    }

                    MessageBox(hwnd, TEXT("Recording stopped!"), TEXT("INFO"), MB_OK);
                    break;
                }

                case IDM_PERFORM_ACTIONS:
                {
                    PlayRecordedActions();
                    break;
                }
            }
            break;
        }
    }

    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}
