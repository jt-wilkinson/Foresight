// Foresight.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Foresight.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND hNetPay, hSavings, hExpenses, hComboBox, hResult;
double netPay = 0.0, savings = 0.0, expenses = 0.0;
int projectionPeriod = 1;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FORESIGHT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FORESIGHT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FORESIGHT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FORESIGHT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 300, 325, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Create UI elements
    CreateWindowW(L"static", L"Monthly Credits:", WS_VISIBLE | WS_CHILD, 20, 20, 150, 20, hWnd, nullptr, hInstance, nullptr);
    hNetPay = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 170, 20, 100, 20, hWnd, nullptr, hInstance, nullptr);

    CreateWindowW(L"static", L"Savings:", WS_VISIBLE | WS_CHILD, 20, 60, 100, 20, hWnd, nullptr, hInstance, nullptr);
    hSavings = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 120, 60, 100, 20, hWnd, nullptr, hInstance, nullptr);

    CreateWindowW(L"static", L"Monthly Debits:", WS_VISIBLE | WS_CHILD, 20, 100, 120, 20, hWnd, nullptr, hInstance, nullptr);
    hExpenses = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 140, 100, 100, 20, hWnd, nullptr, hInstance, nullptr);

    CreateWindowW(L"static", L"Projection Period:", WS_VISIBLE | WS_CHILD, 20, 140, 120, 20, hWnd, nullptr, hInstance, nullptr);
    hComboBox = CreateWindowW(L"combobox", NULL, CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 140, 140, 100, 100, hWnd, nullptr, hInstance, nullptr);

    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 Month");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"3 Months");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"6 Months");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 Year");

    CreateWindowW(L"button", L"Calculate", WS_VISIBLE | WS_CHILD, 20, 180, 100, 30, hWnd, (HMENU)1, hInstance, nullptr);
    hResult = CreateWindowW(L"static", L"", WS_VISIBLE | WS_CHILD, 20, 220, 220, 20, hWnd, nullptr, hInstance, nullptr);

    return TRUE;
}

void CalculateProjection()
{
    wchar_t buffer[256];
    GetWindowText(hNetPay, buffer, 256);
    netPay = _wtof(buffer);

    GetWindowText(hSavings, buffer, 256);
    savings = _wtof(buffer);

    GetWindowText(hExpenses, buffer, 256);
    expenses = _wtof(buffer);

    int periodIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
    switch (periodIndex)
    {
    case 0: projectionPeriod = 1; break;
    case 1: projectionPeriod = 3; break;
    case 2: projectionPeriod = 6; break;
    case 3: projectionPeriod = 12; break;
    default: projectionPeriod = 1; break;
    }

    double projectedSavings = savings + (netPay - expenses) * projectionPeriod;
    swprintf_s(buffer, L"Projected Savings: %.2f", projectedSavings);
    SetWindowText(hResult, buffer);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case 1: // Calculate button clicked
            CalculateProjection();
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}