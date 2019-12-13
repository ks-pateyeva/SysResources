// SysResources.cpp

#include "framework.h"
#include "SysResources.h"
#include "TreeView.h"
#include "PlotView.h"
#include  "CPUInfo.h"
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include "RAMInfo.h"
#include "WiFiInfo.h"
#include "DiskInfo.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

enum MonitorState { CPU, RAM, DISK, DISK_WRITE, DISK_READ, WIFI_SEND, WIFI_RECEIVE};

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hwndEdit;
TreeView* treeView;
MonitorState state = CPU;

bool isDrawing = false, isFirst = true;


ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

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
			treeView = new TreeView(hInst, hWnd);
			hwndEdit = CreateWindowEx(
				0, L"EDIT", // predefined class 
				NULL, // no window title 
				WS_CHILD | WS_VISIBLE |
				ES_LEFT | ES_MULTILINE,
				10, 270, 100, 300, // set size in WM_SIZE message 
				hWnd, // parent window 
				(HMENU)ID_EDITCHILD, // edit control ID 
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);
			SendMessage(hwndEdit, EM_SETREADONLY, TRUE, NULL);
			SetTimer(hWnd, IDT_TIMER1, (UINT)1000, (TIMERPROC)NULL);
		}
		break;
	case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case TVN_SELCHANGED:
				UINT treeViewId = GetDlgCtrlID(treeView->HTreeView);
				if (((LPNMHDR)lParam)->idFrom == treeViewId)
				{
					isFirst = true;
					state = (MonitorState)treeView->GetSelectedItem();
					return TRUE;
				}
				break;
			}
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
	case WM_TIMER:
		{
			if (wParam == IDT_TIMER1)
			{
				if (isFirst)
				{
					switch (state)
					{
					case CPU:
						CpuInfo::InitializeSystemTimes();
						PlotView::prevValue = CpuInfo::GetCpuUsage();
						SetWindowText(hwndEdit, CpuInfo::GetCpuInfoText().c_str());
						break;
					case RAM:
						PlotView::prevValue = RAMInfo::GetRamUsage();
						SetWindowText(hwndEdit, RAMInfo::GetRamInfoText().c_str());
						break;
					case WIFI_RECEIVE:
						PlotView::prevValue = WiFiInfo::GetSpeed(L"BytesReceivedPerSec");
						SetWindowText(hwndEdit, WiFiInfo::GetNetworkAdapterCaption().c_str());
						break;
					case WIFI_SEND:
						PlotView::prevValue = WiFiInfo::GetSpeed(L"BytesSentPerSec");
						SetWindowText(hwndEdit, WiFiInfo::GetNetworkAdapterCaption().c_str());
						break;
					case DISK:
						PlotView::prevValue = DiskInfo::GetDiskUsage();
						SetWindowText(hwndEdit, DiskInfo::GetDiskDriveCaption().c_str());
						break;
					case DISK_WRITE:
						PlotView::prevValue = DiskInfo::GetDiskSpeed(L"DiskWriteBytesPerSec");
						SetWindowText(hwndEdit, DiskInfo::GetDiskDriveCaption().c_str());
						break;
					case DISK_READ:
						PlotView::prevValue = DiskInfo::GetDiskSpeed(L"DiskReadBytesPerSec");
						SetWindowText(hwndEdit, DiskInfo::GetDiskDriveCaption().c_str());
						break;
					}
					PlotView::prevTime = 0;
					isFirst = false;
					isDrawing = false;
				}
				else
				{
					isDrawing = true;
				}
				InvalidateRect(hWnd, &(PlotView::rect), false);
			}
			break;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			Graphics graphics(hdc);
			if (isDrawing)
			{
				double currValue = 0.0;
				switch (state)
				{
				case CPU:
					currValue = CpuInfo::GetCpuUsage();
					break;
				case RAM:
					currValue = RAMInfo::GetRamUsage();
					break;
				case DISK:
					currValue = DiskInfo::GetDiskUsage();
					break;
				case DISK_WRITE:
					currValue = DiskInfo::GetDiskSpeed(L"DiskWriteBytesPerSec");
					break;
				case DISK_READ:
					currValue = DiskInfo::GetDiskSpeed(L"DiskReadBytesPerSec");
					break;
				case WIFI_RECEIVE:
					currValue = WiFiInfo::GetSpeed(L"BytesReceivedPerSec");
					break;
				case WIFI_SEND:
					currValue = WiFiInfo::GetSpeed(L"BytesSentPerSec");
					break;
				}
				PlotView::Line(graphics, PlotView::prevTime, PlotView::prevValue,
				               PlotView::prevTime + 10.0, currValue, true);
				isDrawing = false;
			}
			else
			{
				PlotView::DrawPlot(graphics);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		KillTimer(hWnd, IDT_TIMER1);
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
