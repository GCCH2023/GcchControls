#include "GcchControls.h"
#include <tchar.h>
#include "resource.h"

GcchBitmap* g_bitmap = NULL;
HWND g_hWnd0 = NULL;
HWND g_hWnd1 = NULL;
GcchFont* g_font = NULL;
GcchBitmap* g_colorButtons = NULL;

LRESULT ButtonEventHandler(GcchControl* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_USER_BUTTON_CLICK)
	{
		if (control->id == 2)
		{
			HWND hWnd = (HWND)control->data;
			GcchSwitchCheck(hWnd);
		}
		else if (control->id == 8)
		{
		/*	HWND hWnd = (HWND)control->data;
			GcchSetPadding(hWnd, 20, 20);*/
		}
	}
	return 0;
}

LRESULT CheckBoxEventHandler(GcchControl* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_USER_CHECK_CHANGED)
	{
		HWND hWnd = (HWND)control->data;
		if (wParam)
			GcchSetText(hWnd, _T("复选框选中"));
		else
			GcchSetText(hWnd, _T("复选框未选中"));
	}
	return 0;
}

LRESULT RadioButtonEventHandler(GcchControl* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_USER_CHECK_CHANGED)
	{
		if (wParam)
		{
			TCHAR buffer[256];
			HWND hwnd = (HWND)control->data;
			GcchGetText(hWnd, buffer, ARRAYSIZE(buffer));
			GcchSetText(hwnd, buffer);
		}
	}
	return 0;
}

LRESULT ListBoxEventHandler(GcchControl* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int toggles = 0;
	GcchListBoxItem* listBoxItem = (GcchListBoxItem*)lParam;
	if (msg == WM_USER_DRAW_ITEM)
	{
		GcchBitmap* bmp = (GcchBitmap*)wParam;
		RECT rect = { 0, 0, listBoxItem->rect.right - listBoxItem->rect.left, listBoxItem->rect.bottom - listBoxItem->rect.top };
		GcchFillRectangle(bmp, &rect, RGB(250, 250, 250));
		if (listBoxItem->index < 6)
		{
			int row = listBoxItem->index, column = 0;
			column = 2 * ((toggles >> row) & 1);
			if ((listBoxItem->state & 2) != 0)
				column |= 1;
			rect.right = 21;
			rect.bottom = 19;
			GcchDrawBitmap(bmp, &rect, g_colorButtons, 21 * column, 20 * row);
		}
		return TRUE;
	}
	else if (msg == WM_USER_LSBX_LDOWN)
	{
		if (listBoxItem->index >= 0 && listBoxItem->index < 6)
			toggles ^= 1 << listBoxItem->index;
	}
	return 0;
}



LRESULT GcchWindowFunc(GcchWindow* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static GcchRadioItem radioItem = { 0 };
	switch (msg)
	{
	case WM_CREATE:
	{
					  //RECT rect;
					  // g_bitmap = GcchCreateBitmap(400, 400);
					  // g_bitmap = GcchLoadBitmapById(IDB_BUTTON);
					  //g_bitmap = GcchLoadBitmap(_T("SKIN_BUTTON.bmp")); 	GcchControls.exe!GcchControlWndProc(HWND__ * hWnd, unsigned int msg, unsigned int wParam, long lParam) 行 32	C
					  g_colorButtons = GcchLoadBitmap(MAKEINTRESOURCE(IDB_COLOR_BTNS), 0);
					  //GcchRect(&rect, 0, 0, g_bitmap->width, g_bitmap->height);
					  //GcchHalfTone(g_bitmap, &rect);
					  g_font = GcchCreateFont(24, 24, 700, _T("Arial"));

					  HWND hwnd = GcchCreateLabelEx(hWnd, 1, 50, 50, 0, 0,  _T("HelloWorld"),
						  GCCH_HA_CENTER, GCCH_VA_CENTER);
					  GcchCreateRadioButton(&radioItem, _T("单选框0"), 50, 150, hWnd, 4, RadioButtonEventHandler, hwnd);
					  GcchCreateRadioButton(&radioItem, _T("单选框1"), 50, 180, hWnd, 5, RadioButtonEventHandler, hwnd);
					  GcchCreateRadioButton(&radioItem, _T("单选框2"), 50, 210, hWnd, 6, RadioButtonEventHandler, hwnd);
					  hwnd = GcchCreateCheckBox(_T("复选框"), 50, 80, hWnd, 3, CheckBoxEventHandler, hwnd);
					  GcchCreateButton(_T("按钮"), 50, 100, 100, 40, hWnd, 2, ButtonEventHandler, hwnd);

					  DWORD style = WS_CHILD | WS_VISIBLE;
					 hwnd = GcchCreateListBox(0, style, 200, 50, 138, 19, hWnd, 7, ListBoxEventHandler, NULL);
					 GcchSetItemSize(hwnd, 23, 19);
					 GcchSetColumns(hwnd, 6);

					 GcchCreateButton(_T("按钮"), 350, 50, 60, 20, hWnd, 8, ButtonEventHandler, hwnd);

					 return 0;
	}
	case WM_LBUTTONDOWN:
	{
						  /* HWND hwnd = GetDlgItem(hWnd, 1);
						   GcchSetLabelBackground(hwnd, RGB(255, 0, 0));*/
						   // GcchSetControlSize(hwnd, 100, 40);
						   return 0;
	}
	case WM_LBUTTONUP:
	{
						/* HWND hwnd = GetDlgItem(hWnd, 1);
						 GcchSetLabelFont(hwnd, g_font);*/
						 return 0;
	}
	case WM_RBUTTONUP:
	{
						/* HWND hwnd = GetDlgItem(hWnd, 1);
						 GcchSetLabelFont(hwnd, NULL);*/
						 return 0;
	}
	case WM_RBUTTONDOWN:
	{
						   /*HWND hwnd = GetDlgItem(hWnd, 1);
						   GcchSetLabelForeground(hwnd, RGB(255, 255, 255));
						   GcchSetLabelAlignment(hwnd, GCCH_HA_LEFT, GCCH_VA_TOP);*/
						   return 0;
	}
	case WM_MBUTTONDOWN:
	{
						   /*HWND hwnd = GetDlgItem(hWnd, 1);
						   GcchSetLabelText(hwnd, _T("你好2342342啊金刚狼拉进来"));*/
						   return 0;
	}
	case WM_PAINT:
	{
					 PAINTSTRUCT ps;
					 HDC hdc = BeginPaint(hWnd, &ps);

					/* RECT rect;
					 GcchBitmap bitmap = { 0 };
					 LPCTSTR text = _T("复选框");
					 int width;
					 bitmap.hdc = hdc;
					 width = GcchMeasureString(&bitmap, text, -1, &rect, 0);
					 GcchMoveRect(&rect, 20, 20);
					 GcchDrawStringEx(&bitmap, text, -1, &rect, RGB(255, 0, 0), DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);*/
					 //RECT rect = { 100, 100, 410, 310 };
					 ////COLORREF color = hWnd == g_hWnd0 ? RGB(255, 0, 0) : RGB(0, 255, 0);
					 ////GcchDrawRectangle(g_bitmap, &rect, color);
					 ////GcchRectBySize(&rect, 50, 50, 50, 50);
					 ////GcchDrawBitmap(&bitmap, &rect, g_bitmap, 0, 0);
					 //RECT srcRect = { 0, 0, 31, 21 };
					 //RECT sliceRect = { 4, 4, 4, 4 };
					 //if (hWnd == g_hWnd0)
					 //{
						// GcchDrawBitmapEx(&bitmap, &rect, g_bitmap, &srcRect);
						// //GcchHalfTone(&g_bitmap, &rect);
						// //GcchDrawBitmap(&bitmap, &rect, &g_bitmap, rect.left, rect.top);
					 //}
					 //else
					 //{
						// GcchDrawSliceBitmap(&bitmap, &rect, g_bitmap, &srcRect, &sliceRect);
					 //}

					 //GcchRectBySize(&rect, 10, 10, 100, 40);
					 //GcchDrawString(&bitmap, _T("Hello"), 10, 10, RGB(0, 255, 255));
					 //GcchMeasureString(&bitmap, _T("World!"), -1, &rect, DT_VCENTER | DT_CENTER);
					 //GcchMoveRect(&rect, 100, 20);
					 //GcchDrawStringEx(&bitmap, _T("World!"), -1, &rect, RGB(255, 0, 255), DT_VCENTER | DT_CENTER);

					 EndPaint(hWnd, &ps);
					 return 0;
	}
	case WM_DESTROY:
		GcchDestroyBitmap(&g_bitmap);
		GcchDestroyFont(&g_font);
		GcchDestroyBitmap(&g_colorButtons);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	HWND hWnd;
	MSG msg = { 0 };

	if (GcchInitialize(hInstance) == FALSE)
		return 0;

	//// 使用固定坐标和窗口大小来创建
	//hWnd = GcchCreateControl(0, _T("Window0"), WS_OVERLAPPEDWINDOW,
	//	300, 100, 800, 600, NULL, NULL, GcchWindowFunc, NULL);
	//g_hWnd0 = hWnd;
	//GcchShowWindow(hWnd, nShowCmd);

	// 使用客户区大小来创建，客户区加上边框会比上面的窗口更大
	hWnd = GcchCreateWindow(0, _T("Window"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		800, 600, (GcchControlFunc)GcchWindowFunc, NULL, NULL);
	g_hWnd1 = hWnd;
	GcchShowWindow(hWnd, nShowCmd);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GcchUninitialize();
	return 0;
}