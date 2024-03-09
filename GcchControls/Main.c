#include "GcchControls.h"
#include <tchar.h>

GcchBitmap* g_bitmap = NULL;
HWND g_hWnd0 = NULL;
HWND g_hWnd1 = NULL;
GcchFont* g_font = NULL;

LRESULT Button_Click(GcchControl* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//if (msg == WM_USER_BUTTON_CLICK)
	//{
	//	MessageBox(NULL, _T("xxx"), _T("按钮点击"), MB_OK);
	//}
	return 0;
}

LRESULT GcchWindowFunc(GcchWindow* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
					  //RECT rect;
					  // g_bitmap = GcchCreateBitmap(400, 400);
					  // g_bitmap = GcchLoadBitmapById(IDB_BUTTON_SKIN);
					  //g_bitmap = GcchLoadBitmap(_T("SKIN_BUTTON.bmp")); 	GcchControls.exe!GcchControlWndProc(HWND__ * hWnd, unsigned int msg, unsigned int wParam, long lParam) 行 32	C

					  //GcchRect(&rect, 0, 0, g_bitmap->width, g_bitmap->height);
					  //GcchHalfTone(g_bitmap, &rect);
					  g_font = GcchCreateFont(24, 24, 700, _T("Arial"));

					  HWND hwnd = GcchCreateLabelEx(hWnd, 1, 50, 50, 0, 0,  _T("HelloWorld"),
						  GCCH_HA_CENTER, GCCH_VA_CENTER);

					  GcchCreateButton(_T("按钮"), 100, 100, 100, 40, hWnd, 2, Button_Click, NULL);

					  return 0;
	}
	case WM_LBUTTONDOWN:
	{
						   HWND hwnd = GetDlgItem(hWnd, 1);
						   GcchSetLabelBackground(hwnd, RGB(255, 0, 0));
						   // GcchSetControlSize(hwnd, 100, 40);
						   return 0;
	}
	case WM_LBUTTONUP:
	{
						 HWND hwnd = GetDlgItem(hWnd, 1);
						 GcchSetLabelFont(hwnd, g_font);
						 return 0;
	}
	case WM_RBUTTONUP:
	{
						 HWND hwnd = GetDlgItem(hWnd, 1);
						 GcchSetLabelFont(hwnd, NULL);
						 return 0;
	}
	case WM_RBUTTONDOWN:
	{
						   HWND hwnd = GetDlgItem(hWnd, 1);
						   GcchSetLabelForeground(hwnd, RGB(255, 255, 255));
						   GcchSetLabelAlignment(hwnd, GCCH_HA_LEFT, GCCH_VA_TOP);
						   return 0;
	}
	case WM_MBUTTONDOWN:
	{
						   HWND hwnd = GetDlgItem(hWnd, 1);
						   GcchSetLabelText(hwnd, _T("你好2342342啊金刚狼拉进来"));
						   return 0;
	}
	case WM_PAINT:
	{
					 PAINTSTRUCT ps;
					 HDC hdc = BeginPaint(hWnd, &ps);

					 //Bitmap bitmap = { 0 };
					 //bitmap.hdc = hdc;
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
	hWnd = GcchCreateWindow(0, _T("Window"), WS_OVERLAPPEDWINDOW,
		800, 600, (GcchControlFunc)GcchWindowFunc, NULL);
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