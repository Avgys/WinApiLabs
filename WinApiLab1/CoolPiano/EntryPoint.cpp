#include <Windows.h>
#include <WindowsX.h>
#include <stdio.h>
#include <iostream>
#include <gdiplus.h>
#include <chrono>
#include <time.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#define WM_SELFCONTROL 0x0401

using namespace std;
int x, dx = 5;
int y, dy = 5;

struct MovementStruct {
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false; 
};

MovementStruct direction;
MovementStruct autoDirection;
HPEN hpPen;
HBRUSH hbrPoint;
RECT rect;
RECT windowRect;
Bitmap* bmp;
bool isActive;
HINSTANCE hInst;
const UINT_PTR unActiveTimer = 0x201;
const UINT_PTR moveTimer = 0x202;

void Set(int _x, int _y, bool fix = false) {
	int width = (bmp->GetWidth() / 2);
	int height = (bmp->GetHeight() / 2);
	if (x != _x) {
		if (_x - width > windowRect.left && _x + width < windowRect.right) {
			x = _x;
		}
		else if (fix) {
			direction.left ^= 1;
			direction.right ^= 1;
		}
	}

	if (y != _y) {
		if (_y - height > windowRect.top && _y + height < windowRect.bottom) {
			y = _y;
		}
		else if (fix){
			direction.up ^= 1;
			direction.down ^= 1;
		}
	}
}

void offSet(bool fix = false) {		
	Set( x + dx * (-direction.left + direction.right), y + dy * (-direction.up + direction.down), fix);		
}

void clearOffSet() {
	direction.up = false;
	direction.down = false;
	direction.left = false;
	direction.right = false;
}

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	int zDelta = 0;

	wchar_t strCode[3];

	switch (uMsg)
	{
	case WM_CREATE:
		
		hbrPoint = CreateSolidBrush(0x00FF00FF);
		hpPen = CreatePen(PS_DASH, 1, 0x0000FF00);
		isActive = false;		
		break;

	case WM_DESTROY:
		DeleteObject(hpPen);
		DeleteObject(hbrPoint);
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &windowRect);
		break;
	case WM_LBUTTONDOWN:
		clearOffSet();
		Set(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_LBUTTONUP:
		break;

	case WM_MOUSEMOVE:		
		if (GET_KEYSTATE_WPARAM(wParam) == MK_LBUTTON) {
			Set(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

		case WM_KILLFOCUS:
			if (isActive) {
				clearOffSet();
			}
			break;

	case WM_KEYDOWN:	
		if (!isActive) {
			clearOffSet();
			KillTimer(hWnd, moveTimer);
		}

		if (wParam == VK_LEFT) 
			direction.left = true;

		if (wParam == VK_UP)
			direction.up = true;

		if (wParam == VK_DOWN)
			direction.down = true;

		if (wParam == VK_RIGHT)
			direction.right = true;
		offSet();
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYUP:	
		SetTimer(hWnd, unActiveTimer, 2000, NULL);
		if (wParam == VK_LEFT)
			direction.left = false;

		if (wParam == VK_UP)
			direction.up = false;

		if (wParam == VK_DOWN)
			direction.down = false;

		if (wParam == VK_RIGHT)
			direction.right = false;
		break;

	case WM_TIMER:
		if (!isActive) {
			
			switch (wParam)
			{
			case unActiveTimer:		
				KillTimer(hWnd, unActiveTimer);				
				SetTimer(hWnd, moveTimer, 100, NULL);
				direction = autoDirection;
				break;

			case moveTimer:
				
				offSet(true);
				InvalidateRect(hWnd, NULL, FALSE);
				SetTimer(hWnd, moveTimer, 100, NULL);
				break;
			}
		}
		else {
			KillTimer(hWnd, moveTimer);
			SetTimer(hWnd, unActiveTimer, 2000, NULL);			
		}
		break;

	case WM_MOUSEWHEEL:
		if (!isActive) {
			clearOffSet();
			KillTimer(hWnd, moveTimer);
			SetTimer(hWnd, unActiveTimer, 2000, NULL);
		}
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		switch (GET_KEYSTATE_WPARAM(wParam)) {
		case MK_SHIFT:
			if (zDelta > 0) {
				direction.right = true;
			}
			else {
				direction.left = true;
			}
			break;
		default:
			if (zDelta > 0) {
				direction.up = true;
			}
			else {
				direction.down = true;
			}
			break;
		}
		offSet();
		clearOffSet();
		InvalidateRect(hWnd, NULL, TRUE);
		break;


	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);					
		Graphics graphics(hDC);
		graphics.DrawImage(bmp, x - bmp->GetWidth() / 2, y - bmp->GetHeight() / 2,(INT)bmp->GetWidth(), (INT)bmp->GetHeight());		
		EndPaint(hWnd, &ps);
		break;
	}	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	hInst = hInstance;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	ULONG_PTR gdiToken;
	GdiplusStartupInput gdiInput;
	GdiplusStartup(&gdiToken, &gdiInput, NULL);
	bmp = new Bitmap(L"C:\\folder\\pngegg.bmp");
	
	hWnd = CreateWindowEx(0, L"MyWindowClass", L"App", (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
		0, 0, 800, 700, 0, 0, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	autoDirection.down = true;
	autoDirection.right = true;
	GetClientRect(hWnd, &windowRect);
	x = windowRect.right / 2;
	y = windowRect.bottom / 2;
	SetTimer(hWnd, unActiveTimer, 100, NULL);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiToken);

	return msg.wParam;
}