#include "GcchControls.h"
#include <tchar.h>


// 应用程序句柄
static HINSTANCE g_hInstance = NULL;
// 控件的窗口类名
static const TCHAR GcchControlClass[] = _T("GCCHCONTROLCLASS");


// 所有控件公用的消息处理函数
static LRESULT CALLBACK GcchControlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GcchControl* control;
	if (msg == WM_NCCREATE)
	{
		LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
		GcchControl* param = (GcchControl*)createStruct->lpCreateParams;
		// 分配一个控件变量并与窗口句柄绑定
		control = (GcchControl*)GcchMalloc(sizeof(GcchControl));
		*control = *param;
		control->hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)control);
	}
	else
	{
		control = (GcchControl*)GetWindowLong(hWnd, GWL_USERDATA);
	}
	LRESULT ret;
	if (control)
		ret = control->func(hWnd, msg, wParam, lParam, control->data);
	else
		ret = DefWindowProc(hWnd, msg, wParam, lParam);

	if (msg == WM_NCDESTROY)
	{
		// 释放控件变量
		SetWindowLong(hWnd, GWL_USERDATA, 0);
		GcchFree((LPVOID*)&control);
	}
	return ret;
}

BOOL GcchInitialize(HINSTANCE hInstance)
{
	g_hInstance = hInstance;

	// 注册窗口类
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GcchControlWndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_WINLOGO);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = GcchControlClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_WINLOGO);

	return RegisterClassEx(&wcex);
}


void GcchUninitialize()
{

}

LPVOID GcchMalloc(size_t bytes)
{
	return malloc(bytes);
}

void GcchFree(LPVOID* ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

HWND GcchCreateControl(_In_ DWORD dwExStyle, _In_opt_ LPCWSTR text, _In_ DWORD dwStyle,
	_In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight,
	_In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu,
	GcchControlFunc func, LPVOID data)
{
	GcchControl param;
	param.hWnd = NULL;
	if (func == NULL)
		func = GcchDefControlFunc;
	param.func = func;
	param.data = data;
	return CreateWindowEx(dwExStyle, GcchControlClass, text, dwStyle, X, Y, nWidth, nHeight, hWndParent,
		hMenu, g_hInstance, &param);
}

LRESULT GcchDefControlFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID data)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LPRECT GcchRect(LPRECT rect, LONG left, LONG top, LONG right, LONG bottom)
{
	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;
	return rect;
}

LPRECT GcchRectBySize(LPRECT rect, LONG x, LONG y, LONG width, LONG height)
{
	rect->left = x;
	rect->top = y;
	rect->right = x + width;
	rect->bottom = y + height;
	return rect;
}

LPRECT GcchRectByPoints(LPRECT rect, POINT leftTop, POINT rightBottom)
{
	*(LPPOINT)rect = leftTop;
	*(LPPOINT)&rect->right = rightBottom;
	return rect;
}

LONG GcchGetRectWidth(LPCRECT rect)
{
	return rect->right - rect->left;
}

LONG GcchGetRectHeight(LPCRECT rect)
{
	return rect->bottom - rect->top;
}

BOOL GcchRectValid(LPCRECT rect)
{
	return rect->left < rect->right && rect->top < rect->bottom;
}

LPRECT GcchRectCorrect(LPRECT rect)
{
	rect->left = min(rect->left, rect->right);
	rect->top = min(rect->top, rect->bottom);
	rect->right = max(rect->left, rect->right);
	rect->bottom = max(rect->top, rect->bottom);
	return rect;
}

BOOL GcchRectAnd(LPRECT outRect, LPCRECT rect1, LPCRECT rect2)
{
	// 计算矩形交集的边界值
	LONG left = max(rect1->left, rect2->left);
	LONG top = max(rect1->top, rect2->top);
	LONG right = min(rect1->right, rect2->right);
	LONG bottom = min(rect1->bottom, rect2->bottom);

	// 检查是否有交集
	BOOL hasIntersection = (left < right && top < bottom);

	// 如果有交集，更新输出矩形；否则清空输出矩形
	if (outRect)
	{
		if (hasIntersection)
		{
			outRect->left = left;
			outRect->top = top;
			outRect->right = right;
			outRect->bottom = bottom;
		}
		else
		{
			memset(outRect, 0, sizeof(RECT));
		}
	}

	return hasIntersection;
}

LPRECT GcchRectOr(LPRECT outRect, LPCRECT rect1, LPCRECT rect2)
{
	// 计算并更新输出矩形的左上角坐标，取两者较小值
	outRect->left = min(rect1->left, rect2->left);
	outRect->top = min(rect1->top, rect2->top);

	// 计算并更新输出矩形的右下角坐标，取两者较大值
	outRect->right = max(rect1->right, rect2->right);
	outRect->bottom = max(rect1->bottom, rect2->bottom);

	return outRect;
}

BOOL GcchRectContains(LPCRECT rect, LONG x, LONG y)
{
	return x >= rect->left && y >= rect->top && x < rect->right && y < rect->bottom;
}

LPRECT GcchMoveRect(LPRECT rect, LONG deltaX, LONG deltaY)
{
	rect->left += deltaX;
	rect->top += deltaY;
	rect->right += deltaX;
	rect->bottom += deltaY;
	return rect;
}

LPRECT GcchRectAdd(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom)
{
	rect->left += dLeft;
	rect->top += dTop;
	rect->right += dRight;
	rect->bottom += dBottom;
	return rect;
}

LPRECT GcchRectShiftLeft(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom)
{
	rect->left <<= dLeft;
	rect->top <<= dTop;
	rect->right <<= dRight;
	rect->bottom <<= dBottom;
	return rect;
}

LPRECT GcchRectShiftRight(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom)
{
	rect->left >>= dLeft;
	rect->top >>= dTop;
	rect->right >>= dRight;
	rect->bottom >>= dBottom;
	return rect;
}

LPRECT GcchMakeRectCenter(LPRECT rect, LPCRECT bound)
{
	LONG width = rect->right - rect->left;
	LONG height = rect->bottom - rect->top;
	LONG x = (bound->right - bound->left - width) / 2;
	LONG y = (bound->bottom - bound->top - height) / 2;
	return GcchRectBySize(rect, x, y, width, height);
}

GcchBitmap* GcchCreateBitmap(int width, int height)
{
	GcchBitmap* bmp = (GcchBitmap*)GcchMalloc(sizeof(GcchBitmap));
	BITMAPINFO bmi = { 0 };
	HBITMAP hBitmap;	// 位图句柄
	DWORD* pBits;		// 像素数据
	if (!bmp)
		return NULL;
	bmp->hdc = CreateCompatibleDC(NULL);
	// 创建一个设备无关位图
	bmi.bmiHeader.biWidth = width;
	// 对于未压缩的 RGB 位图，如果 biHeight 为正值，则位图为自下而上的 DIB，原点位于左下角。
	// 如果 biHeight 为负数，则位图为自上而下 DIB，原点位于左上角。
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biSize = sizeof(bmi);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	hBitmap = CreateDIBSection(bmp->hdc, &bmi, 0, (void **)&pBits, 0, 0);
	if (!hBitmap)
	{
		GcchDestroyBitmap(&bmp);
		return NULL;
	}
	// 设置新的位图
	bmp->width = width;
	bmp->height = height;
	bmp->bitmap = hBitmap;
	bmp->pBits = pBits;
	bmp->oldBitmap = SelectObject(bmp->hdc, hBitmap);
	return bmp;
}

// 根据位图句柄创建位图对象
GcchBitmap* GcchLoadBitmap(LPCTSTR filename)
{
	HDC compDC;
	HANDLE oldImage;
	GcchBitmap* bitmap;
	HANDLE hImage = LoadImage(g_hInstance, filename,
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hImage == NULL)
		return NULL;

	compDC = CreateCompatibleDC(NULL);
	if (!compDC)
		return NULL;

	oldImage = SelectObject(compDC, hImage);
	if (oldImage == NULL)
	{
		DeleteDC(compDC);
		return NULL;
	}

	// 获取BITMAP信息
	BITMAP bmInfo;
	GetObject(hImage, sizeof(BITMAP), &bmInfo);               // 第2个参数是要写入缓冲区的字节数
	bitmap = GcchCreateBitmap(bmInfo.bmWidth, bmInfo.bmHeight);
	if (!bitmap)
	{
		SelectObject(compDC, oldImage);
		DeleteObject(hImage);
		DeleteDC(compDC);
		return NULL;
	}

	BitBlt(bitmap->hdc, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, compDC, 0, 0, SRCCOPY);

	SelectObject(compDC, oldImage);
	DeleteObject(hImage);
	DeleteDC(compDC);

	return bitmap;
}

void GcchDestroyBitmap(GcchBitmap** pBitmap)
{
	GcchBitmap* bmp;
	if (!pBitmap || !*pBitmap)
		return;

	bmp = *pBitmap;
	if (bmp->hdc && bmp->oldBitmap)
	{
		SelectObject(bmp->hdc, bmp->oldBitmap);
		DeleteObject(bmp->bitmap);
	}
	GcchFree((LPVOID*)&bmp);
	*pBitmap = NULL;
}

BOOL GcchDrawRectangle(GcchBitmap* bitmap, LPCRECT rect, COLORREF color)
{
	if (!bitmap || !bitmap->hdc)
		return FALSE;
	{
		HBRUSH brush = CreateSolidBrush(color);
		FrameRect(bitmap->hdc, rect, brush);
		DeleteObject(brush);
	}
	return TRUE;
}

BOOL GcchFillRectangle(GcchBitmap* bitmap, LPCRECT rect, COLORREF color)
{
	if (!bitmap || !bitmap->hdc)
		return FALSE;
	{
		HBRUSH brush = CreateSolidBrush(color);
		FillRect(bitmap->hdc, rect, brush);
		DeleteObject(brush);
	}
	return TRUE;
}

BOOL GcchDrawBitmap(GcchBitmap *bitmap, LPCRECT rect, GcchBitmap *srcBitmap, LONG srcX, LONG srcY)
{
	if (!bitmap || !srcBitmap || !bitmap->hdc)
		return FALSE;
	if (!GcchRectValid(rect))
		return FALSE;

	return BitBlt(bitmap->hdc, rect->left, rect->top, GcchGetRectWidth(rect), GcchGetRectHeight(rect),
		srcBitmap->hdc, srcX, srcY, SRCCOPY);
}


BOOL GcchDrawBitmapEx(GcchBitmap *bitmap, LPCRECT rect, GcchBitmap *srcBitmap, LPCRECT srcRect)
{
	if (!bitmap || !srcBitmap || !bitmap->hdc)
		return FALSE;
	if (!GcchRectValid(rect))
		return FALSE;

	return StretchBlt(bitmap->hdc, rect->left, rect->top, GcchGetRectWidth(rect), GcchGetRectHeight(rect),
		srcBitmap->hdc,srcRect->left, srcRect->top, GcchGetRectWidth(srcRect), GcchGetRectHeight(srcRect), SRCCOPY);
}

BOOL GcchDrawSliceBitmap(GcchBitmap* bmp, LPCRECT rect, GcchBitmap *srcBmp, LPCRECT srcRect, LPCRECT sliceRect)
{
	if (!bmp || !bmp->hdc || !srcBmp || !srcBmp->hdc)
		return FALSE;

	{
		LONG srcWidth = srcRect->right - srcRect->left;
		LONG srcHeight = srcRect->bottom - srcRect->top;
		LONG srcX = srcRect->left;
		LONG srcY = srcRect->top;
		LONG srcCenterWidth = srcWidth - sliceRect->left - sliceRect->right;
		LONG srcCenterHeight = srcHeight - sliceRect->top - sliceRect->bottom;

		LONG width = rect->right - rect->left;
		LONG height = rect->bottom - rect->top;
		LONG x = rect->left;
		LONG y = rect->top;
		LONG centerWidth = width - sliceRect->left - sliceRect->right;
		LONG centerHeight = height - sliceRect->top - sliceRect->bottom;

		// 左上
		BitBlt(bmp->hdc, x, y, sliceRect->left, sliceRect->top,
			srcBmp->hdc, srcX, srcY, SRCCOPY);
		// 中上
		StretchBlt(bmp->hdc, x + sliceRect->left, y, centerWidth, sliceRect->top,
			srcBmp->hdc, srcX + sliceRect->left, srcY, srcCenterWidth, sliceRect->top, SRCCOPY);
		// 右上
		BitBlt(bmp->hdc, rect->right - sliceRect->right, y, sliceRect->right, sliceRect->top,
			srcBmp->hdc, srcRect->right - sliceRect->right, srcY, SRCCOPY);
		y += sliceRect->top;
		srcY += sliceRect->top;

		// 中左
		StretchBlt(bmp->hdc, x, y, sliceRect->left, centerHeight,
			srcBmp->hdc, srcX, srcY, sliceRect->left, srcCenterHeight, SRCCOPY);
		// 中中
		StretchBlt(bmp->hdc, x + sliceRect->left, y, centerWidth, centerHeight,
			srcBmp->hdc, srcX + sliceRect->left, srcY, srcCenterWidth, srcCenterHeight, SRCCOPY);
		// 中右
		StretchBlt(bmp->hdc, rect->right - sliceRect->right, y, sliceRect->right, centerHeight,
			srcBmp->hdc, srcRect->right - sliceRect->right, srcY, sliceRect->right, srcCenterHeight, SRCCOPY);
		y += centerHeight;
		srcY += srcCenterHeight;

		// 坐下
		BitBlt(bmp->hdc, x, y, sliceRect->left, sliceRect->bottom,
			srcBmp->hdc, srcX, srcY, SRCCOPY);
		// 中下
		StretchBlt(bmp->hdc, x + sliceRect->left, y, centerWidth, sliceRect->bottom,
			srcBmp->hdc, srcX + sliceRect->left, srcY, srcCenterWidth, sliceRect->bottom, SRCCOPY);
		// 右上
		BitBlt(bmp->hdc, rect->right - sliceRect->right, y, sliceRect->right, sliceRect->bottom,
			srcBmp->hdc, srcRect->right - sliceRect->right, srcY, SRCCOPY);
	}
	return TRUE;
}

BOOL GcchDrawString(GcchBitmap *bmp, LPCTSTR text, LONG x, LONG y, COLORREF color)
{
	if (!bmp || !bmp->hdc)
		return FALSE;

	SetTextColor(bmp->hdc, color);
	TextOut(bmp->hdc, x, y, text, _tcslen(text));
	return TRUE;
}

BOOL GcchDrawStringEx(GcchBitmap *bmp, LPCTSTR text, int count, LPRECT rect, COLORREF color, UINT format)
{
	if (!bmp || !bmp->hdc)
		return FALSE;

	SetTextColor(bmp->hdc, color);
	DrawText(bmp->hdc, text, count, rect, format);
	return TRUE;
}

LONG GcchMeasureString(GcchBitmap *bitmap, LPCTSTR text, int count, LPRECT rect, UINT format)
{
	RECT rc = { 0 };
	if (!bitmap || !bitmap->hdc)
		return 0;

	DrawText(bitmap->hdc, text, count, &rc, format | DT_CALCRECT);
	if (rect)
		*rect = rc;
	return rc.right - rc.left;
}



BOOL GcchHalfTone(GcchBitmap* bmp, LPCRECT rect)
{
	if (!bmp || !bmp->hdc || !bmp->pBits)
		return FALSE;

	if (!bmp->width || !bmp->height || !GcchRectValid(rect))
		return TRUE;

	{
		LONG x, y;
		DWORD* p;
		RECT bound = { 0, 0, bmp->width, bmp->height };
		if (!GcchRectAnd(&bound, rect, &bound))
			return TRUE;

		for (y = bound.top; y < bound.bottom; ++y)
		{
			p = &bmp->pBits[y * bmp->width];
			for (x = bound.left; x < bound.right; ++x)
			{
				p[x] = ((p[x] >> 2) & 0x3F3F3F) + 0xBCBCBC;
			}
		}
	}
	return TRUE;
}

HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style, int width, int height, GcchControlFunc func, LPVOID data)
{
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	RECT screen = { 0, 0, cx, cy };
	RECT rect = { 0, 0, width, height };
	// 根据客户区大小计算窗口大小
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);
	GcchMakeRectCenter(&rect, &screen);
	return GcchCreateControl(exStyle, text, style,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, func, data);
}

void GcchShowWindow(HWND hWnd, int nCmdShow)
{
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
}

void GcchSetWindowTitle(HWND hWnd, LPCTSTR title)
{
	SetWindowText(hWnd, title);
}
