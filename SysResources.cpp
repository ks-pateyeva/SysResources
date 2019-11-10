// SysResources.cpp

#include "framework.h"
#include "SysResources.h"
#include "TreeView.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

int diff_x = 0;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
VOID OnPaint(HDC hdc, HWND hwnd);

const int CENTER_X = 150;
const int CENTER_Y = 530;
const int SCALE_X = 5;
const int SCALE_Y = 5;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SYSRESOURCES, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SYSRESOURCES));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	GdiplusShutdown(gdiplusToken);
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SYSRESOURCES));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SYSRESOURCES);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(
		szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		5, 5, 700, 640, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			TreeView treeView;
			treeView.CreateATreeView(hInst, hWnd);
		}
		break;
	case WM_COMMAND:
		{
			const int wmId = LOWORD(wParam);
			switch (wmId)
			{
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
			OnPaint(hdc, hWnd);
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

void PlotLine(Graphics& g, Pen& p, float x1, float y1, float x2, float y2, bool fDiff)
{
	if (fDiff)
	{
		x1 = x1 - diff_x;
		x2 = x2 - diff_x;
	}
	x1 = CENTER_X + x1 * SCALE_X;
	x2 = CENTER_X + x2 * SCALE_X;
	y1 = CENTER_Y - y1 * SCALE_Y;
	y2 = CENTER_Y - y2 * SCALE_Y;

	if ((x1 >= 0 && x2 >= 0) || (!fDiff))
	{
		g.DrawLine(&p, x1, y1, x2, y2);
	}
}

void PlotString(Graphics& g, float x1, float y1, PCWSTR text, int len)
{
	x1 = CENTER_X + x1 * SCALE_X;
	y1 = CENTER_Y - y1 * SCALE_Y;

	PointF origin(x1, y1);
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	Font myFont(L"Arial", 10);

	wchar_t* wa = new wchar_t[len];

	g.DrawString(
		text,
		len,
		&myFont,
		origin,
		&blackBrush);
}

VOID OnPaint(HDC hdc, HWND hwnd)
{
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0));
	graphics.DrawRectangle(&pen, 120, 10, 550, 550);

	// AXIS   
	PlotLine(graphics, pen, 0, 0.0, 0, 100.0, false);
	PlotLine(graphics, pen, 0.0, 0.0, 100.0, 0.0, false);

	for (float i = 10.0; i <= 100.0; i += 10.0)
	{
		PlotLine(graphics, pen, 0.0, i, -1, i, false);
		PlotString(graphics, (i == 100.0 ? -6.0f : -5.0f), i,
		           (PCWSTR)std::to_wstring(static_cast<int>(i)).c_str(), 3);
	}
}
