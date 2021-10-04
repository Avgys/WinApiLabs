#include <Windows.h>
#include <WindowsX.h>
#include <stdio.h>
#include <iostream>
#include "commctrl.h"
#include <math.h>
#include <string>
#include <time.h>
#include <fstream>

using namespace std;

#define DELTA 3

RECT windowRect;
HINSTANCE hInst;
int rowCount = 10;
int columnCount = 5;
string** stringMatrix;

void CreateStringMatrix();
void DrawTable(HDC hDC, RECT rect, int borderSize);

LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	int zDelta = 0;
	static int width = 0, height = 0;
	wchar_t strCode[3];

	switch (uMsg)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &windowRect);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &windowRect);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		DrawTable(hDC, windowRect, 3);
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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);


	hWnd = CreateWindowEx(0, L"MyWindowClass", L"App", (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
		0, 0, 800, 700, 0, 0, hInstance, NULL);

	CreateStringMatrix();


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	GetClientRect(hWnd, &windowRect);

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void CreateStringMatrix() {
	stringMatrix = new string * [rowCount];

	for (int i = 0; i < rowCount; i++) {
		stringMatrix[i] = new string[columnCount];
	}

	string strTemp;

	ifstream inputFile("text.txt");

	for (char c = inputFile.get(); inputFile.good(); c = inputFile.get())
	{
		if (c != '\n' && c != ' ')
			strTemp += c;
	}
	inputFile.close();

	int len = strTemp.length();

	int quantityOfCharInCell = (int)len / (columnCount * rowCount);
	int charactersLeft = len % (columnCount * rowCount);

	int currentOffset = 0;

	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
			int countCharsInThisCell = quantityOfCharInCell;

			if (charactersLeft-- > 0)
				++countCharsInThisCell;

			stringMatrix[i][j] = strTemp.substr(currentOffset, countCharsInThisCell);

			currentOffset += countCharsInThisCell;
		}
	}
}


string getLongestString(int currentLine)
{
	string longestSrting = stringMatrix[currentLine][0];
	for (int j = 1; j < columnCount; j++)
	{
		if (stringMatrix[currentLine][j].length() > longestSrting.length())
			longestSrting = stringMatrix[currentLine][j];
	}

	return longestSrting;
}

int GetBlockHeight(HDC hdc, int currentLine, int width)
{
	string longestString = getLongestString(currentLine);

	RECT nonDrawableBlock;
	nonDrawableBlock.left = 0;
	nonDrawableBlock.top = 0;
	nonDrawableBlock.bottom = 1;
	nonDrawableBlock.right = width;

	string a = longestString.c_str();
	int b = longestString.length();
	std::wstring widestr = std::wstring(longestString.begin(), longestString.end());
	int height = (int)DrawText(hdc, widestr.c_str(), longestString.length(), &nonDrawableBlock,
		DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER) + DELTA;

	return height;
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}


void DrawTextBlock(HDC hdc, int left, int top, int width, int height, int row, int column, int borderSize)
{
	RECT rect;
	rect.top = (long)(top + borderSize);
	//rect.top = (long)(100);
	rect.left = (long)(left + borderSize);
	//rect.left = (long)(100);
	rect.right = (long)(left + width - borderSize);
	//rect.right = (long)(left + width);
	rect.bottom = (long)(top + height - borderSize);
	//rect.bottom = (long)(top + height);

	std::wstring widestr = std::wstring(stringMatrix[row][column].begin(), stringMatrix[row][column].end());

	DrawText(hdc, widestr.c_str(), stringMatrix[row][column].length(), &rect, DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER);
}

HFONT generateFont()
{
	int fnWeight = 400;

	HFONT hFont = CreateFont(20, 7, 0, 0, fnWeight, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));

	return hFont;
}

void DrawTable(HDC hDC, RECT rect, int borderSize)
{
	float posX, posY = 0;
	float cellWidth = (float)rect.right / columnCount;
	float cellHeight = 0;

	HFONT hFont = generateFont();
	SelectObject(hDC, hFont);


	//DrawTextBlock(hDC, 80, 80, rect.right, rect.bottom, 1, 1, borderSize);
	for (int i = 0; i < rowCount; i++)
	{
		cellHeight = GetBlockHeight(hDC, i, cellWidth);
		posX = 0;
		for (int j = 0; j < columnCount; j++)
		{
			//hFont = generateFont();
			//SelectObject(hDC, hFont);

			DrawLine(hDC, (int)posX, (int)posY, (int)posX, (int)(posY + cellHeight));
			DrawTextBlock(hDC, posX, posY, cellWidth, cellHeight, i, j, borderSize);
			posX += cellWidth;

			//DeleteObject(hFont);
		}

		DrawLine(hDC, (int)(posX - 1), (int)posY, (int)(posX - 1), (int)(posY + cellHeight));
		DrawLine(hDC, 0, (int)posY, rect.right, (int)posY);
		posY += cellHeight;
	}

	DeleteObject(hFont);
	DrawLine(hDC, 0, (int)posY, rect.right, (int)posY);
}