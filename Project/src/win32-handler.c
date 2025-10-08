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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
    // function declarations
    int initialize(void);
    
    // variable declarations
    WNDCLASSEX wndclass;
    TCHAR szClassName[] = TEXT("MyWindowClass");
    TCHAR szWindowName[] = TEXT("My-Window");
    HWND hwnd;
    MSG msg;

    // ZeroMemory wndclass
    ZeroMemory((void*)&wndclass, sizeof(WNDCLASSEX));

    // create custom window class
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

    // register custom window class
    RegisterClassEx(&wndclass);

    // create window of custom window class in memory
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
    
    // set ghwnd to hwnd
    ghwnd = hwnd;

    // show window from memory
    ShowWindow(hwnd, nCmdShow);

    // update the window
    UpdateWindow(hwnd);

    // initialize
    if (initialize())
    {
        MessageBox(NULL, TEXT("Failed to initialize the program"), TEXT("EXIT_ERROR"), MB_ICONERROR | MB_OK);
        DestroyWindow(hwnd);
    }

    // message pump
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // cleanup linked list memory
    FreeRecordedActions();

    return ((int)msg.wParam);
}

int initialize(void)
{
    // code
    // precautionary sleep 
    Sleep(50);

    // create button and set properties
    hRecordActionsButtonWnd = CreateWindow( 
        TEXT("BUTTON"),  // Predefined class
        TEXT("Record Actions"),      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        10,         // x position 
        10,         // y position 
        120,        // Button width
        100,        // Button height
        ghwnd,     // Parent window
        (HMENU)IDM_RECORD_ACTIONS,       // menu aahe.
        GetModuleHandle(NULL), 
        NULL);      // Pointer not needed.

    hStopRecordingActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),  // Predefined class
        TEXT("Stop Recording"),      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        10 + 140,         // x position 
        10,         // y position 
        120,        // Button width
        100,        // Button height
        ghwnd,     // Parent window
        (HMENU)IDM_STOP_RECORD_ACTIONS,       // menu aahe.
        GetModuleHandle(NULL), 
        NULL);      // Pointer not needed. 

    hPerformRecordedActionsButtonWnd = CreateWindow(
        TEXT("BUTTON"),  // Predefined class
        TEXT("Perform Actions"),      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        10 + 140 + 140,         // x position 
        10,         // y position 
        120,        // Button width
        100,        // Button height
        ghwnd,     // Parent window
        (HMENU)IDM_PERFORM_ACTIONS,       // menu aahe.
        GetModuleHandle(NULL), 
        NULL);      // Pointer not needed. 

    return (0);
}

// function to add a recorded input to linked list
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

// function to play back all recorded inputs
void PlayRecordedActions(void)
{
    if (!gpHead)
    {
        MessageBox(ghwnd, TEXT("No actions recorded!"), TEXT("INFO"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    ActionNode *temp = gpHead;
    while (temp)
    {
        SendInput(1, &temp->input, sizeof(INPUT));
        Sleep(50); // delay between inputs
        temp = temp->next;
    }

    MessageBox(ghwnd, TEXT("Playback completed!"), TEXT("INFO"), MB_OK | MB_ICONINFORMATION);
}

// function to free linked list
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

// callback function body
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        case WM_DESTROY:
        {
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
                    FreeRecordedActions(); // clear old data
                    MessageBox(hwnd, TEXT("Recording started!"), TEXT("INFO"), MB_OK);
                    break;
                }

                case IDM_STOP_RECORD_ACTIONS:
                {
                    gIsRecording = FALSE;
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
