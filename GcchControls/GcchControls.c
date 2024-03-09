#include "GcchControls.h"
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"


// 应用程序句柄
static HINSTANCE g_hInstance = NULL;
// 控件的窗口类名
static const TCHAR GcchControlClass[] = _T("GCCHCONTROLCLASS");
// 用于绘图的缓冲位图
static GcchBitmap* g_bufferBitmap = NULL;
// 默认字体
static GcchFont* g_defaultFont = NULL;
// 按钮皮肤位图
static GcchBitmap* g_buttonBitmap = NULL;

// 所有控件公用的消息处理函数
static LRESULT CALLBACK GcchControlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GcchControl* control = NULL;
	if (msg == WM_NCCREATE)
	{
		LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
		GcchControl* param = (GcchControl*)createStruct->lpCreateParams;
		if (param->bytes >= sizeof(GcchControl))
		{
			// 分配一个控件变量并与窗口句柄绑定
			control = (GcchControl*)GcchMalloc(param->bytes);
			memcpy_s(control, param->bytes, param, param->bytes);
			control->hWnd = hWnd;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)control);
		}
	}
	else
	{
		control = (GcchControl*)GetWindowLong(hWnd, GWL_USERDATA);
	}
	LRESULT ret;

	if (control)
		ret = control->func(control, hWnd, msg, wParam, lParam);
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

	if (!RegisterClassEx(&wcex))
		return FALSE;

	g_defaultFont = GcchCreateFont(6, 12, 0, _T("宋体"));
	if (g_defaultFont == NULL)
		return FALSE;

	g_bufferBitmap = GcchCreateBitmap(512, 512);
	if (g_bufferBitmap == NULL)
		return FALSE;
	
	g_buttonBitmap = GcchLoadBitmap(MAKEINTRESOURCE(IDB_BUTTON_SKIN), 0);
	if (g_buttonBitmap == NULL)
		return FALSE;

	return TRUE;
}


void GcchUninitialize()
{
	GcchDestroyBitmap(&g_buttonBitmap);
	GcchDestroyBitmap(&g_bufferBitmap);
	GcchDestroyFont(&g_defaultFont);
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

void GcchInitControl(GcchControl* control, GcchControlFunc func, LPVOID data,
	UINT id, size_t bytes, GcchControlType type)
{
	control->func = func;
	control->data = data;
	control->id = id;
	control->bytes = bytes;
	control->type = type;
}

GcchControlType GcchGetControlType(HWND hWnd)
{
	GcchControl* control = (GcchControl*)GetWindowLong(hWnd, GWL_USERDATA);
	if (control)
		return (GcchControlType)control->type;
	return GCCH_CT_NONE;
}

HWND GcchCreateControl(_In_ DWORD dwExStyle, _In_opt_ LPCWSTR text, _In_ DWORD dwStyle,
	_In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight,
	_In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, GcchControl* control)
{
	if (control->func == NULL)
		control->func = GcchDefControlFunc;
	return CreateWindowEx(dwExStyle, GcchControlClass, text, dwStyle, X, Y, nWidth, nHeight, hWndParent,
		hMenu, g_hInstance, control);
}

LRESULT GcchDefControlFunc(GcchControl* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void GcchSetControlSize(HWND hWnd, int width, int height)
{
	GcchControl* control = (GcchControl*)GetWindowLong(hWnd, GWL_USERDATA);
	if (control)
	{
		control->width = width;
		control->height = height;
		SendMessage(hWnd, WM_USER_SET_SIZE, width, height);
	}
}

BOOL GcchRedraw(HWND hWnd)
{
	return RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
}

BOOL GcchRedrawNow(HWND hWnd)
{
	return RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
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

BOOL GcchRectEqual(LPCRECT rect1, LPCRECT rect2)
{
	return rect1->left == rect2->left &&
		rect1->top == rect2->top &&
		rect1->right == rect2->right &&
		rect1->bottom == rect2->bottom;
}

void GcchInitFont(GcchFont* font, int width, int height, int weight, LPCTSTR familyName)
{
	font->width = width;
	font->height = height;
	font->weight = weight;
	_tcscpy_s(font->familyName, 32, familyName);
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
	SetBkMode(bmp->hdc, TRANSPARENT);
	bmp->font = g_defaultFont->hFont;
	bmp->oldFont = SelectObject(bmp->hdc, bmp->font);
	return bmp;
}

// 根据位图句柄创建位图对象
GcchBitmap* GcchLoadBitmap(LPCTSTR filename, UINT flags)
{
	HDC compDC;
	HANDLE oldImage;
	GcchBitmap* bitmap;

	flags |= LR_CREATEDIBSECTION;

	HANDLE hImage = LoadImage(g_hInstance, filename,
		IMAGE_BITMAP, 0, 0, flags);
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
	if (bmp->hdc)
	{
		if (bmp->bitmap)
		{
			SelectObject(bmp->hdc, bmp->oldBitmap);
			DeleteObject(bmp->bitmap);
		}
		if (bmp->oldFont)
			SelectObject(bmp->hdc, bmp->oldFont);
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


BOOL GcchSetFont(GcchBitmap* bitmap, GcchFont* font)
{
	if (!bitmap->hdc || !font || bitmap->font == font->hFont)
		return FALSE;
	bitmap->font = font->hFont;
	SelectObject(bitmap->hdc, font->hFont);
	return TRUE;
}

BOOL GcchUnsetFont(GcchBitmap* bitmap)
{
	if (!bitmap->hdc)
		return FALSE;
	if (bitmap->font != g_defaultFont->hFont)
	{
		bitmap->font = g_defaultFont->hFont;
		SelectObject(bitmap->hdc, bitmap->font);
	}
	return TRUE;
}

GcchFont* GcchCreateFont(int width, int height, int weight, LPCTSTR familyName)
{
	GcchFont* font = GcchMalloc(sizeof(GcchFont));
	if (!font)
		return NULL;
	font->hFont = CreateFont(
		height, // 字体高度
		width, // 平均字符宽度
		0, // 逃逸角度
		0, // 基线方向角度
		weight, // 字体粗细
		FALSE, // 斜体
		FALSE, // 下划线
		FALSE, // 删除线
		DEFAULT_CHARSET, // 字符集标识符
		OUT_DEFAULT_PRECIS, // 输出精度
		CLIP_DEFAULT_PRECIS, // 剪辑精度
		DEFAULT_QUALITY, // 输出质量
		DEFAULT_PITCH | FF_DONTCARE, // 间距和族
		familyName // 字体族
		);
	if (!font->hFont)
	{
		GcchFree((LPVOID*)&font);
		return NULL;
	}
	font->width = width;
	font->height = height;
	font->weight = weight;
	_tcscpy_s(font->familyName, 32, familyName);
	return font;
}

void GcchDestroyFont(GcchFont** pFont)
{
	if (!pFont || !*pFont || !(*pFont)->hFont)
		return;
	DeleteObject((*pFont)->hFont);
	*pFont = NULL;
}

HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style, int width, int height, GcchControlFunc func, LPVOID data)
{
	GcchWindow window = { 0 };
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	RECT screen = { 0, 0, cx, cy };
	RECT rect = { 0, 0, width, height };
	// 根据客户区大小计算窗口大小
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);
	GcchMakeRectCenter(&rect, &screen);

	GcchInitControl((GcchControl*)&window, func, data, 0, sizeof(GcchWindow), GCCH_CT_WINDOW);
	return GcchCreateControl(exStyle, text, style,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, (GcchControl*)&window);
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


// 计算标签的需要的大小
static void CalcLabelRect(LPRECT rect, LPCTSTR text, int width, int height)
{
	GcchMeasureString(g_bufferBitmap, text, -1, rect, 0);  // 计算出需要的矩形大小
	if (width <= 0)
		width = GcchGetRectWidth(rect) + 10;		// 增加一点间距
	if (height <= 0)
		height = GcchGetRectHeight(rect) + 6;
	GcchRectBySize(rect, 0, 0, width, height);
}

// 调整标签的大小
static void AdjustLabelRect(GcchLabel* label)
{
	RECT rect, clientRect;
	TCHAR text[1024];
	GetWindowText(label->hWnd, text, 1023);
	text[1023] = '\0';
	GcchSetFont(g_bufferBitmap, label->font);
	CalcLabelRect(&rect, text, label->width, label->height);
	GcchUnsetFont(g_bufferBitmap);
	GetClientRect(label->hWnd, &clientRect);
	if (!GcchRectEqual(&clientRect, &rect))
		SetWindowPos(label->hWnd, NULL, 0, 0, GcchGetRectWidth(&rect), GcchGetRectHeight(&rect), SWP_NOMOVE | SWP_NOZORDER);
}

LRESULT LabelFunc(GcchLabel *label, HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_USER_SET_FOREGROUND:
		label->foreground = wParam;
		GcchRedraw(hWnd);
		return 0;
	case WM_USER_SET_BACKGROUND:
		label->background = wParam;
		GcchRedraw(hWnd);
		return 0;
	case WM_USER_SET_ALIGNMENT:
		label->horizontalAlignment = wParam;
		label->verticalAlignment = lParam;
		GcchRedraw(hWnd);
		return 0;
	case WM_USER_SET_FONT:
		label->font = (GcchFont*)wParam;
		AdjustLabelRect(label);
		GcchRedraw(hWnd);
		return 0;
	case WM_USER_SET_SIZE:
		AdjustLabelRect(label);
		GcchRedraw(hWnd);
		return 0;
	case WM_SETTEXT:
	{
					   LRESULT ret = GcchDefControlFunc((GcchControl*)label, hWnd, msg, wParam, lParam);		// 调用默认处理来设置文本
					   AdjustLabelRect(label);
					   return ret;
	}
	case WM_SIZE:
		GcchRedraw(hWnd);
		return 0;
	case WM_PAINT:
	{
					 RECT clientRect;
					 PAINTSTRUCT ps;
					 HDC hdc = BeginPaint(hWnd, &ps);
					 GcchBitmap bitmap = { 0 };
					 COLORREF color;
					 TCHAR text[1024];
					 GetClientRect(hWnd, &clientRect);
					 
					 bitmap.hdc = hdc;
					 if (IsWindowEnabled(hWnd))
						 color = label->foreground;
					 else
						 color = RGB(192, 192, 192);
					
					 GetWindowText(hWnd, text, 1023);
					 text[1023] = '\0';
					 GcchFillRectangle(g_bufferBitmap, &clientRect, label->background);
					 GcchSetFont(g_bufferBitmap, label->font);
					 GcchDrawStringEx(g_bufferBitmap, text, -1, &clientRect, color,
						 label->horizontalAlignment | label->verticalAlignment | DT_SINGLELINE);
					 GcchUnsetFont(g_bufferBitmap);
					 GcchDrawBitmap(&bitmap, &clientRect, g_bufferBitmap, 0, 0);
					 
					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}
	return GcchDefControlFunc((GcchControl*)label, hWnd, msg, wParam, lParam);
}



HWND GcchCreateLabel(HWND hWndParent, UINT id, int x, int y, int width, int height, LPCTSTR text)
{
	return GcchCreateLabelEx(hWndParent, id, x, y, width, height, text, GCCH_HA_LEFT, GCCH_VA_TOP);
}

HWND GcchCreateLabelEx(HWND hWndParent, UINT id, int x, int y, int width, int height, LPCTSTR text,
	GcchHorizontalAlignment horizontalAlignment, GcchVerticalAlignment verticalAlignment)
{
	GcchLabel label = { 0 };
	RECT rect;
	GcchInitControl((GcchControl*)&label, (GcchControlFunc)LabelFunc, NULL, id, sizeof(GcchLabel), GCCH_CT_LABEL);
	label.background = RGB(250, 250, 250);
	label.verticalAlignment = verticalAlignment;
	label.horizontalAlignment = horizontalAlignment;
	label.width = width;
	label.height = height;
	CalcLabelRect(&rect, text, width, height);
	GcchMoveRect(&rect, x, y);
	return GcchCreateControl(0, text, WS_CHILD | WS_VISIBLE,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		hWndParent, (HMENU)id, (GcchControl*)&label);

}

BOOL GcchSetLabelText(HWND hWnd, LPCTSTR text)
{
	BOOL ret = SetWindowText(hWnd, text);
	if (ret)
		GcchRedraw(hWnd);
	return ret;
}

void GcchSetLabelBackground(HWND hWnd, COLORREF color)
{
	SendMessage(hWnd, WM_USER_SET_BACKGROUND, color, 0);
}

void GcchSetLabelForeground(HWND hWnd, COLORREF color)
{
	SendMessage(hWnd, WM_USER_SET_FOREGROUND, color, 0);
}

void GcchSetLabelAlignment(HWND hWnd, GcchHorizontalAlignment horizontalAlignment, GcchVerticalAlignment verticalAlignment)
{
	SendMessage(hWnd, WM_USER_SET_ALIGNMENT, horizontalAlignment, verticalAlignment);
}

void GcchSetLabelFont(HWND hWnd, GcchFont* font)
{
	SendMessage(hWnd, WM_USER_SET_FONT, (WPARAM)font, 0);
	GcchRedraw(hWnd);
}


LRESULT ButtonFunc(GcchButton* button, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT clientRect;
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		button->state |= GCCH_CS_PRESSED;
		GcchRedrawNow(hWnd);
		button->event((GcchControl*)button, hWnd, WM_USER_BUTTON_CLICK, 0, 0);
		return 0;
	case WM_LBUTTONUP:
		if (hWnd == GetCapture())
		{
			button->state &= ~GCCH_CS_PRESSED;
			GcchRedraw(hWnd);
			ReleaseCapture();
			// 可以在鼠标左键按下时触发按钮的点击事件，也可以在弹起时触发
			//GetClientRect(hWnd, &clientRect);
			//if (GcchRectContains(&clientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
			//{
			//	button->event(button, hWnd, WM_USER_BUTTON_CLICK, 0, 0);
			//}
		}
		return 0;
	case WM_CAPTURECHANGED:
		button->state &= ~GCCH_CS_PRESSED;
		GcchRedraw(hWnd);
		return 0;
	case WM_MOUSEMOVE:
		GetClientRect(hWnd, &clientRect);
		// 是否捕获，点是否在按钮内
		if (hWnd == GetCapture())
		{
			if (GcchRectContains(&clientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
			{
				// 点在按钮内
				if ((button->state & GCCH_CS_PRESSED) == 0)
				{
					button->state |= GCCH_CS_PRESSED;
					GcchRedraw(hWnd);
				}
			}
			else
			{
				// 点不在按钮内
				if ((button->state & GCCH_CS_PRESSED) != 0)
				{
					button->state &= ~GCCH_CS_PRESSED;
					GcchRedraw(hWnd);
				}
			}
		}
		else
		{
			if (GcchRectContains(&clientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
			{
				// 点在按钮内
				if ((button->state & GCCH_CS_HOVER) == 0)
				{
					button->state |= GCCH_CS_HOVER;
					GcchRedraw(hWnd);

					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hWnd;
					// 调用TrackMouseEvent函数，要不然收不到鼠标离开的消息
					TrackMouseEvent(&tme);
				}
			}
			//else
			//{
			//	// 点不在按钮内, 应该不会出现
			//	if ((button->state & GCCH_CS_HOVER) != 0)
			//	{
			//		button->state &= ~GCCH_CS_HOVER;
			//		GcchRedraw(hWnd);
			//	}
			//}
		}
		return 0;
	case WM_MOUSELEAVE:
		button->state &= ~GCCH_CS_HOVER;
		GcchRedraw(hWnd);
		return 0;
	case WM_PAINT:
	{
					 int index, x;
					 GcchBitmap bitmap = { 0 };
					 PAINTSTRUCT ps;
					 RECT srcRect, sliceRect;
					 TCHAR text[256];
					 bitmap.hdc = BeginPaint(hWnd, &ps);
					 GetClientRect(hWnd, &clientRect);
					 if (IsWindowEnabled(hWnd))
					 {
						 if (button->state & GCCH_CS_PRESSED)
							 index = 2;
						 else
							 index = (button->state & GCCH_CS_HOVER) ? 1 : 0;
					 }
					 else
					 {
						 index = 3;
					 }
					 x = 31 * index;		// 31 是位图中一个小块的宽度
					 GcchRectBySize(&srcRect, x, 0, 31, 21);	// 21 是位图中一个小块的高度
					 GcchRect(&sliceRect, 4, 4, 4, 4);
					 GcchDrawSliceBitmap(g_bufferBitmap, &clientRect, g_buttonBitmap, &srcRect, &sliceRect);
					 GetWindowText(hWnd, text, 256);
					 GcchDrawStringEx(g_bufferBitmap, text, -1, &clientRect, RGB(255, 255, 255), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					 GcchDrawBitmap(&bitmap, &clientRect, g_bufferBitmap, 0, 0);
					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}
	return GcchDefControlFunc((GcchControl*)button, hWnd, msg, wParam, lParam);
}


HWND GcchCreateButton(LPCTSTR text, int x, int y, int width, int height,
	HWND hWndParent, UINT id, GcchEventFunc func, LPVOID data)
{
	GcchButton control = { 0 };
	GcchInitControl((GcchControl*)&control, (GcchControlFunc)ButtonFunc, data, id, sizeof(GcchButton), GCCH_CT_BUTTON);
	control.width = width;
	control.height = height;
	control.event = func;
	return GcchCreateControl(0, text, WS_CHILD | WS_VISIBLE,
		x, y, width, height,
		hWndParent, (HMENU)id, (GcchControl*)&control);
}
