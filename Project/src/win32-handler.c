// header files
#include <Windows.h>
#include "win32-handler.h"

// callback function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// define window width and height
#define WINDOW_WIDTH  440
#define WINDOW_HEIGHT 160

// global handle to window
HWND ghwnd;

// related to creating buttons
HWND hRecordActionsButtonWnd;
HWND hStopRecordingActionsButtonWnd;
HWND hPerformRecordedActionsButtonWnd;

// recording state and linked list globals
BOOL gIsRecording = FALSE;
ActionNode *gpHead = NULL;
ActionNode *gpTail = NULL;

// global hook handles
HHOOK gKeyboardHook = NULL;
HHOOK gMouseHook = NULL;

// handle for the target window during playback
HWND gRecordedTargetWindow = NULL;

// hook procedure declarations
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

// entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
    // function declarations
    int initialize(void);

    // variable declarations
    WNDCLASSEX wndclass;
    TCHAR szClassName[] = TEXT("MyWindowClass");
    TCHAR szWindowName[] = TEXT("Macro Recorder");
    HWND hwnd;
    MSG msg;

    // clean the wndclass struct
    ZeroMemory(&wndclass, sizeof(WNDCLASSEX));

    // define the window class
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpszClassName = szClassName;
    wndclass.hInstance = hInstance;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(APP_ICON));
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(APP_ICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

    // register the above window class
    RegisterClassEx(&wndclass);

    // create window
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
    
    // show the window created in memory
    ShowWindow(hwnd, nCmdShow);

    // update the window
    UpdateWindow(hwnd);

    // initialize components
    if (initialize())
    {
        MessageBox(NULL, TEXT("Failed to initialize the program"), TEXT("EXIT_ERROR"), MB_ICONERROR | MB_OK);
        DestroyWindow(hwnd);
    }

    // message loop
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // cleanup before exit
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

// function to initialize UI elements
int initialize(void)
{
    // code
    Sleep(50);

    // create buttons
    hRecordActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),        // Predefined class
        TEXT("Record Actions"),// Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles
        10,                    // x position
        10,                    // y position
        120,                   // Button width
        100,                   // Button height
        ghwnd,                 // Parent window
        (HMENU)IDM_RECORD_ACTIONS, // Menu ID
        GetModuleHandle(NULL),
        NULL);                 // Pointer not needed.

    hStopRecordingActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),        // Predefined class
        TEXT("Stop Recording"),// Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles
        150,                   // x position
        10,                    // y position
        120,                   // Button width
        100,                   // Button height
        ghwnd,                 // Parent window
        (HMENU)IDM_STOP_RECORD_ACTIONS, // Menu ID
        GetModuleHandle(NULL),
        NULL);                 // Pointer not needed.

    hPerformRecordedActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),         // Predefined class
        TEXT("Perform Actions"),// Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles
        290,                    // x position
        10,                     // y position
        120,                    // Button width
        100,                    // Button height
        ghwnd,                  // Parent window
        (HMENU)IDM_PERFORM_ACTIONS, // Menu ID
        GetModuleHandle(NULL),
        NULL);                  // Pointer not needed.

    return 0;
}

// adds a new action to the end of the linked list
void AddAction(INPUT in)
{
    // code
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

// removes the last N actions from the list (used to discard clicks on the recorder window)
void RemoveLastNActions(int n)
{
    // code
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

    if (prev)
    {
        prev->next = NULL;
    }

    while (temp) 
    { 
        ActionNode *del = temp; 
        temp = temp->next; 
        free(del); 
    }

    gpTail = prev;
}

// function to play back the recorded actions
void PlayRecordedActions(void)
{
    // code
    if (!gpHead)
    {
        MessageBox(ghwnd, TEXT("No actions recorded!"), TEXT("INFO"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    // let the user select the target window
    MessageBox(ghwnd, TEXT("Click on the target window now, then press OK."), TEXT("INFO"), MB_OK);
    gRecordedTargetWindow = GetForegroundWindow();

    Sleep(100); // minimal delay for focus

    // discard recorder window clicks that initiated playback
    RemoveLastNActions(3); 

    // iterate through linked list and send inputs
    ActionNode *temp = gpHead;
    while (temp)
    {
        SendInput(1, &temp->input, sizeof(INPUT));
        Sleep(50);
        temp = temp->next;
    }

    MessageBox(ghwnd, TEXT("Playback completed!"), TEXT("INFO"), MB_OK | MB_ICONINFORMATION);
}

// frees all nodes in the linked list
void FreeRecordedActions(void)
{
    // code
    ActionNode *temp;
    while (gpHead)
    {
        temp = gpHead;
        gpHead = gpHead->next;
        free(temp);
    }
    gpTail = NULL;
}

// low-level keyboard hook procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // code
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
        AddAction(in);
    }
    return CallNextHookEx(gKeyboardHook, nCode, wParam, lParam);
}

// low-level mouse hook procedure
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // code
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

// callback function definition
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    // code
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

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDM_RECORD_ACTIONS:
                {
                    gIsRecording = TRUE;
                    FreeRecordedActions(); // clear any previous recordings
                    gKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
                    gMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
                    MessageBox(hwnd, TEXT("Recording started! Interact with target window."), TEXT("INFO"), MB_OK);
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

                default:
                    break;
            }
            break;
        }
        
        default:
            break;
    }

    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}
