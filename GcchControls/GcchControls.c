#include "GcchControls.h"
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"
#include <assert.h>

// Ӧ�ó�����
static HINSTANCE g_hInstance = NULL;
// �ؼ��Ĵ�������
static const TCHAR GcchControlClass[] = _T("GCCHCONTROLCLASS");
// ���ڻ�ͼ�Ļ���λͼ
static GcchBitmap* g_bufferBitmap = NULL;
// Ĭ������
static GcchFont* g_defaultFont = NULL;
// ��ťƤ��λͼ
static GcchBitmap* g_buttonBitmap = NULL;
// ��ѡ��λͼ
static GcchBitmap* g_checkBoxBitmap = NULL;
// ��ѡ��λͼ
static GcchBitmap* g_radioBitmap = NULL;
// �߿�λͼ
static GcchBitmap* g_borderBitmap = NULL;
// ��ǰ��ͣ�Ĵ���
static HWND g_hWndMouseHover = NULL;

// ���пؼ����õ���Ϣ������
static LRESULT CALLBACK GcchControlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GcchControl* control = NULL;
	if (msg == WM_NCCREATE)
	{
		LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
		GcchControl* param = (GcchControl*)createStruct->lpCreateParams;
		if (param->bytes >= sizeof(GcchControl))
		{
			// ����һ���ؼ��������봰�ھ����
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
	{
		switch (msg)
		{
		case WM_MOUSEMOVE:
			if (hWnd != g_hWndMouseHover)
			{
				SendMessage(g_hWndMouseHover, WM_MOUSELEAVE, (WPARAM)hWnd, 0);
				g_hWndMouseHover = hWnd;
			}
		}
		ret = control->func(control, hWnd, msg, wParam, lParam);
	}
	else
		ret = DefWindowProc(hWnd, msg, wParam, lParam);

	if (msg == WM_NCDESTROY)
	{
		// �ͷſؼ�����
		SetWindowLong(hWnd, GWL_USERDATA, 0);
		GcchFree((LPVOID*)&control);
	}
	return ret;
}

BOOL GcchInitialize(HINSTANCE hInstance)
{
	g_hInstance = hInstance;

	// ע�ᴰ����
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

	g_defaultFont = GcchCreateFont(6, 12, 0, _T("����"));
	if (g_defaultFont == NULL)
		return FALSE;

	g_bufferBitmap = GcchCreateBitmap(512, 512);
	if (g_bufferBitmap == NULL)
		return FALSE;
	
	g_buttonBitmap = GcchLoadBitmap(MAKEINTRESOURCE(IDB_BUTTON), 0);
	if (g_buttonBitmap == NULL)
		return FALSE;

	g_checkBoxBitmap = GcchLoadBitmap(MAKEINTRESOURCE(IDB_CHECKBOX), 0);
	if (g_checkBoxBitmap == NULL)
		return FALSE;

	g_radioBitmap = GcchLoadBitmap(MAKEINTRESOURCE(IDB_RADIO), 0);
	if (g_radioBitmap == NULL)
		return FALSE;

	g_borderBitmap = GcchLoadBitmap(MAKEINTRESOURCE(IDB_BORDER), 0);
	if (g_borderBitmap == NULL)
		return FALSE;

	return TRUE;
}


void GcchUninitialize()
{
	GcchDestroyBitmap(&g_buttonBitmap);
	GcchDestroyBitmap(&g_checkBoxBitmap);
	GcchDestroyBitmap(&g_radioBitmap);
	GcchDestroyBitmap(&g_borderBitmap);
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

LRESULT RaiseEvent(GcchControl* control, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (control && control->eventHandler)
		return control->eventHandler(control, hWnd, msg, wParam, lParam);
	return 0;
}

void GcchInitControl(GcchControl* control, GcchControlFunc func, GcchEventFunc eventHandler, LPVOID data,
	UINT id, size_t bytes, GcchControlType type)
{
	control->func = func;
	control->data = data;
	control->id = id;
	control->bytes = bytes;
	control->type = type;
	control->eventHandler = eventHandler;
}

GcchControlType GcchGetControlType(HWND hWnd)
{
	GcchControl* control = (GcchControl*)GetWindowLong(hWnd, GWL_USERDATA);
	if (control)
		return (GcchControlType)control->type;
	return GCCH_CT_NONE;
}


int GcchGetText(HWND hWnd, LPTSTR buffer, int bufferSize)
{
	return GetWindowText(hWnd, buffer, bufferSize);
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

BOOL GcchRedrawRect(HWND hWnd, LPCRECT rect)
{
	return RedrawWindow(hWnd, rect, NULL, RDW_INVALIDATE);
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
	// ������ν����ı߽�ֵ
	LONG left = max(rect1->left, rect2->left);
	LONG top = max(rect1->top, rect2->top);
	LONG right = min(rect1->right, rect2->right);
	LONG bottom = min(rect1->bottom, rect2->bottom);

	// ����Ƿ��н���
	BOOL hasIntersection = (left < right && top < bottom);

	// ����н���������������Σ���������������
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
	// ���㲢����������ε����Ͻ����꣬ȡ���߽�Сֵ
	outRect->left = min(rect1->left, rect2->left);
	outRect->top = min(rect1->top, rect2->top);

	// ���㲢����������ε����½����꣬ȡ���߽ϴ�ֵ
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
	HBITMAP hBitmap;	// λͼ���
	DWORD* pBits;		// ��������
	if (!bmp)
		return NULL;
	bmp->hdc = CreateCompatibleDC(NULL);
	// ����һ���豸�޹�λͼ
	bmi.bmiHeader.biWidth = width;
	// ����δѹ���� RGB λͼ����� biHeight Ϊ��ֵ����λͼΪ���¶��ϵ� DIB��ԭ��λ�����½ǡ�
	// ��� biHeight Ϊ��������λͼΪ���϶��� DIB��ԭ��λ�����Ͻǡ�
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
	// �����µ�λͼ
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

// ����λͼ�������λͼ����
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

	// ��ȡBITMAP��Ϣ
	BITMAP bmInfo;
	GetObject(hImage, sizeof(BITMAP), &bmInfo);               // ��2��������Ҫд�뻺�������ֽ���
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

		// ����
		BitBlt(bmp->hdc, x, y, sliceRect->left, sliceRect->top,
			srcBmp->hdc, srcX, srcY, SRCCOPY);
		// ����
		StretchBlt(bmp->hdc, x + sliceRect->left, y, centerWidth, sliceRect->top,
			srcBmp->hdc, srcX + sliceRect->left, srcY, srcCenterWidth, sliceRect->top, SRCCOPY);
		// ����
		BitBlt(bmp->hdc, rect->right - sliceRect->right, y, sliceRect->right, sliceRect->top,
			srcBmp->hdc, srcRect->right - sliceRect->right, srcY, SRCCOPY);
		y += sliceRect->top;
		srcY += sliceRect->top;

		// ����
		StretchBlt(bmp->hdc, x, y, sliceRect->left, centerHeight,
			srcBmp->hdc, srcX, srcY, sliceRect->left, srcCenterHeight, SRCCOPY);
		// ����
		StretchBlt(bmp->hdc, x + sliceRect->left, y, centerWidth, centerHeight,
			srcBmp->hdc, srcX + sliceRect->left, srcY, srcCenterWidth, srcCenterHeight, SRCCOPY);
		// ����
		StretchBlt(bmp->hdc, rect->right - sliceRect->right, y, sliceRect->right, centerHeight,
			srcBmp->hdc, srcRect->right - sliceRect->right, srcY, sliceRect->right, srcCenterHeight, SRCCOPY);
		y += centerHeight;
		srcY += srcCenterHeight;

		// ����
		BitBlt(bmp->hdc, x, y, sliceRect->left, sliceRect->bottom,
			srcBmp->hdc, srcX, srcY, SRCCOPY);
		// ����
		StretchBlt(bmp->hdc, x + sliceRect->left, y, centerWidth, sliceRect->bottom,
			srcBmp->hdc, srcX + sliceRect->left, srcY, srcCenterWidth, sliceRect->bottom, SRCCOPY);
		// ����
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
		height, // ����߶�
		width, // ƽ���ַ����
		0, // ���ݽǶ�
		0, // ���߷���Ƕ�
		weight, // �����ϸ
		FALSE, // б��
		FALSE, // �»���
		FALSE, // ɾ����
		DEFAULT_CHARSET, // �ַ�����ʶ��
		OUT_DEFAULT_PRECIS, // �������
		CLIP_DEFAULT_PRECIS, // ��������
		DEFAULT_QUALITY, // �������
		DEFAULT_PITCH | FF_DONTCARE, // ������
		familyName // ������
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

HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style, int width, int height, 
	GcchControlFunc func, GcchEventFunc eventHandler, LPVOID data)
{
	GcchWindow window = { 0 };
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	RECT screen = { 0, 0, cx, cy };
	RECT rect = { 0, 0, width, height };
	// ���ݿͻ�����С���㴰�ڴ�С
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);
	GcchMakeRectCenter(&rect, &screen);

	GcchInitControl((GcchControl*)&window, func, eventHandler, data, 0, sizeof(GcchWindow), GCCH_CT_WINDOW);
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


// �����ǩ����Ҫ�Ĵ�С
static void CalcLabelRect(LPRECT rect, LPCTSTR text, int width, int height)
{
	GcchMeasureString(g_bufferBitmap, text, -1, rect, 0);  // �������Ҫ�ľ��δ�С
	if (width <= 0)
		width = GcchGetRectWidth(rect) + 10;		// ����һ����
	if (height <= 0)
		height = GcchGetRectHeight(rect) + 6;
	GcchRectBySize(rect, 0, 0, width, height);
}

// ������ǩ�Ĵ�С
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

static LRESULT LabelFunc(GcchLabel *label, HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
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
					   LRESULT ret = GcchDefControlFunc((GcchControl*)label, hWnd, msg, wParam, lParam);		// ����Ĭ�ϴ����������ı�
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
	GcchInitControl((GcchControl*)&label, (GcchControlFunc)LabelFunc, NULL,
		NULL, id, sizeof(GcchLabel), GCCH_CT_LABEL);
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

BOOL GcchSetText(HWND hWnd, LPCTSTR text)
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


LRESULT GcchDefButtonFunc(GcchButtonBase* button, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		if ((button->state & GCCH_CS_HOVER) == 0)
		{
			button->state |= GCCH_CS_HOVER;
			GcchRedraw(hWnd);
		}
		return 0;
	case WM_MOUSELEAVE:
		button->state &= ~GCCH_CS_HOVER;
		GcchRedraw(hWnd);
		return 0;
	}
	return GcchDefControlFunc((GcchControl*)button, hWnd, msg, wParam, lParam);
}

static LRESULT ButtonFunc(GcchButton* button, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT clientRect;
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		button->state |= GCCH_CS_PRESSED;
		GcchRedrawNow(hWnd);
		RaiseEvent((GcchControl*)button, hWnd, WM_USER_BUTTON_CLICK, 0, 0);
		return 0;
	case WM_LBUTTONUP:
		if (hWnd == GetCapture())
		{
			button->state &= ~GCCH_CS_PRESSED;
			GcchRedraw(hWnd);
			ReleaseCapture();
			// ����������������ʱ������ť�ĵ���¼���Ҳ�����ڵ���ʱ����
			//GetClientRect(hWnd, &clientRect);
			//if (GcchRectContains(&clientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
			//{
			//	button->eventHandler(button, hWnd, WM_USER_BUTTON_CLICK, 0, 0);
			//}
		}
		return 0;
	case WM_CAPTURECHANGED:
		button->state &= ~GCCH_CS_PRESSED;
		GcchRedraw(hWnd);
		return 0;
	case WM_MOUSEMOVE:
		GetClientRect(hWnd, &clientRect);
		// �Ƿ񲶻񣬵��Ƿ��ڰ�ť��
		if (hWnd == GetCapture())
		{
			if (GcchRectContains(&clientRect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
			{
				// ���ڰ�ť��
				if ((button->state & GCCH_CS_PRESSED) == 0)
				{
					button->state |= GCCH_CS_PRESSED;
					GcchRedraw(hWnd);
				}
			}
			else
			{
				// �㲻�ڰ�ť��
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
				// ���ڰ�ť��
				if ((button->state & GCCH_CS_HOVER) == 0)
				{
					button->state |= GCCH_CS_HOVER;
					GcchRedraw(hWnd);
				}
			}
			//else
			//{
			//	// �㲻�ڰ�ť��, Ӧ�ò������
			//	if ((button->state & GCCH_CS_HOVER) != 0)
			//	{
			//		button->state &= ~GCCH_CS_HOVER;
			//		GcchRedraw(hWnd);
			//	}
			//}
		}
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
					 x = 31 * index;		// 31 ��λͼ��һ��С��Ŀ��
					 GcchRectBySize(&srcRect, x, 0, 31, 21);	// 21 ��λͼ��һ��С��ĸ߶�
					 GcchRect(&sliceRect, 4, 4, 4, 4);
					 GcchDrawSliceBitmap(g_bufferBitmap, &clientRect, g_buttonBitmap, &srcRect, &sliceRect);
					 GetWindowText(hWnd, text, 256);
					 GcchDrawStringEx(g_bufferBitmap, text, -1, &clientRect, RGB(255, 255, 255), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					 GcchDrawBitmap(&bitmap, &clientRect, g_bufferBitmap, 0, 0);
					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}
	return GcchDefButtonFunc((GcchButtonBase*)button, hWnd, msg, wParam, lParam);
}


HWND GcchCreateButton(LPCTSTR text, int x, int y, int width, int height,
	HWND hWndParent, UINT id, GcchEventFunc eventHandler, LPVOID data)
{
	GcchButton control = { 0 };
	GcchInitControl((GcchControl*)&control, (GcchControlFunc)ButtonFunc, eventHandler,
		data, id, sizeof(GcchButton), GCCH_CT_BUTTON);
	control.width = width;
	control.height = height;
	return GcchCreateControl(0, text, WS_CHILD | WS_VISIBLE,
		x, y, width, height,
		hWndParent, (HMENU)id, (GcchControl*)&control);
}

// ���㸴ѡ��Ŀ��
static LONG GetCheckBoxWidth(LPCTSTR text)
{
	return GcchMeasureString(g_bufferBitmap, text, -1, NULL, 0) + 18;
}


static LRESULT CheckBoxFunc(GcchCheckBox* button, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT clientRect;
	int width;
	switch (msg)
	{
	case WM_USER_GET_CHECK:
		return (button->state & GCCH_CS_CHECKED) != 0;
	case WM_USER_SET_CHECK:
		if (wParam == GcchGetCheck(hWnd))
			return 0;
	case WM_USER_SWITCH_CHECK:
	case WM_LBUTTONDOWN:
		button->state ^= GCCH_CS_CHECKED;
		GcchRedraw(hWnd);
		RaiseEvent((GcchControl*)button, hWnd, WM_USER_CHECK_CHANGED,
			GcchGetCheck(hWnd), 0);
		return 0;
	case WM_SETTEXT:
		GcchDefButtonFunc((GcchButtonBase*)button, hWnd, msg, wParam, lParam);
		width = GetCheckBoxWidth((LPCTSTR)lParam);
		SetWindowPos(hWnd, NULL, 0, 0, width, 13, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	case WM_PAINT:
	{
					 COLORREF color;
					 int x, y;
					 GcchBitmap bitmap = { 0 };
					 PAINTSTRUCT ps;
					 RECT rect;
					 TCHAR text[256];
					 bitmap.hdc = BeginPaint(hWnd, &ps);
					 GetClientRect(hWnd, &clientRect);
					 GcchFillRectangle(g_bufferBitmap, &clientRect, 0xFAFAFA);
					 if (IsWindowEnabled(hWnd))
					 {
						 color = 0;
						 x = 0;
						 if ((button->state & 1) != 0)
						 {
							 color = 0x5645BE;
							 x = 14;
						 }
					 }
					 else
					 {
						 color = 0xC0C0C0;
						 x = 28;
					 }
					 y = GcchGetCheck(hWnd) ? 14 : 0;
					 GcchRect(&rect, 0, 0, 13, 13);
					 GcchDrawBitmap(g_bufferBitmap, &rect, g_checkBoxBitmap, x, y);
					 GetWindowText(hWnd, text, 255);
					 rect = clientRect;
					 rect.left += 16;
					 GcchDrawStringEx(g_bufferBitmap, text, -1, &rect, color, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
					 GcchDrawBitmap(&bitmap, &clientRect, g_bufferBitmap, 0, 0);
					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}
	return GcchDefButtonFunc((GcchButtonBase*)button, hWnd, msg, wParam, lParam);
}


HWND GcchCreateCheckBox(LPCTSTR text, int x, int y,
	HWND hWndParent, UINT id, GcchEventFunc eventHandler, LPVOID data)
{
	GcchCheckBox control = { 0 };
	GcchInitControl((GcchControl*)&control, (GcchControlFunc)CheckBoxFunc, eventHandler, 
		data, id, sizeof(GcchCheckBox), GCCH_CT_CHECKBOX);
	control.width = GetCheckBoxWidth(text);
	control.height = 13;
	return GcchCreateControl(0, text, WS_CHILD | WS_VISIBLE,
		x, y, control.width, control.height,
		hWndParent, (HMENU)id, (GcchControl*)&control);
}

void GcchSetCheck(HWND hWnd, BOOL isCheck)
{
	SendMessage(hWnd, WM_USER_SET_CHECK, isCheck, 0);
}

void GcchSwitchCheck(HWND hWnd)
{
	SendMessage(hWnd, WM_USER_SWITCH_CHECK, 0, 0);
}

BOOL GcchGetCheck(HWND hWnd)
{
	return SendMessage(hWnd, WM_USER_GET_CHECK, 0, 0);
}


static LRESULT RadioButtonFunc(GcchRadioButton* button, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT clientRect;
	int width;
	switch (msg)
	{
	case WM_USER_GET_CHECK:
		if (button->item)
			return button->hWnd == button->item->hWnd;
		return 0;
	case WM_USER_SET_CHECK:
		if (wParam == GcchGetCheck(hWnd))
			return 0;
	case WM_USER_SWITCH_CHECK:
		if (!button->item)
			return 0;
		// ��Ҫ�л�ѡ��״̬
		if (button->hWnd == button->item->hWnd)
		{
			// ԭ����ѡ�еģ�����ȡ��ѡ�����
			button->item->hWnd = NULL;
		}
		else
		{
			// ԭ��û��ѡ��
			if (button->item->hWnd)
				GcchSetCheck(button->item->hWnd, FALSE); // ȡ��ԭ����ѡ��
			button->item->hWnd = hWnd; // ѡ�����
		}
		GcchRedraw(hWnd);
		RaiseEvent((GcchControl*)button, hWnd, WM_USER_CHECK_CHANGED,
			RadioButtonFunc(button, hWnd, WM_USER_GET_CHECK, 0, 0), 0);
		return 0;
	case WM_LBUTTONDOWN: // ������±�ʾҪѡ�����
		return RadioButtonFunc(button, hWnd, WM_USER_SET_CHECK, TRUE, 0);
	case WM_SETTEXT:
		GcchDefButtonFunc((GcchButtonBase*)button, hWnd, msg, wParam, lParam);
		width = GetCheckBoxWidth((LPCTSTR)lParam);
		SetWindowPos(hWnd, NULL, 0, 0, width, 13, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	case WM_PAINT:
	{
					 COLORREF color;
					 int x, y;
					 GcchBitmap bitmap = { 0 };
					 PAINTSTRUCT ps;
					 RECT rect;
					 TCHAR text[256];
					 bitmap.hdc = BeginPaint(hWnd, &ps);
					 GetClientRect(hWnd, &clientRect);
					 GcchFillRectangle(g_bufferBitmap, &clientRect, 0xFAFAFA);
					 if (IsWindowEnabled(hWnd))
					 {
						 color = 0;
						 x = 0;
						 if ((button->state & 1) != 0)
						 {
							 color = 0x5645BE;
							 x = 14;
						 }
					 }
					 else
					 {
						 color = 0xC0C0C0;
						 x = 28;
					 }
					 y = 0;
					 if (button->item && GcchGetCheck(hWnd))
					 {
						 // λͼ���14��14
						 y = 14;
					 }
					 GcchRect(&rect, 0, 0, 13, 13);
					 GcchDrawBitmap(g_bufferBitmap, &rect, g_radioBitmap, x, y);
					 GetWindowText(hWnd, text, 255);
					 rect = clientRect;
					 rect.left += 16;
					 GcchDrawStringEx(g_bufferBitmap, text, -1, &rect, color, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
					 GcchDrawBitmap(&bitmap, &clientRect, g_bufferBitmap, 0, 0);
					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}
	return GcchDefButtonFunc((GcchButtonBase*)button, hWnd, msg, wParam, lParam);
}

HWND GcchCreateRadioButton(GcchRadioItem* item, LPCTSTR text, int x, int y,
	HWND hWndParent, UINT id, GcchEventFunc eventHandler, LPVOID data)
{
	GcchRadioButton control = { 0 };
	GcchInitControl((GcchControl*)&control, (GcchControlFunc)RadioButtonFunc, eventHandler,
		data, id, sizeof(GcchRadioButton), GCCH_CT_RADIO);
	control.item = item;
	control.width = GetCheckBoxWidth(text);
	control.height = 13;
	return GcchCreateControl(0, text, WS_CHILD | WS_VISIBLE,
		x, y, control.width, control.height,
		hWndParent, (HMENU)id, (GcchControl*)&control);
}

LRESULT GroupBoxFunc(GcchGroupBox* groupBox, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_NCCALCSIZE:
	{
						  // ��С�ͻ�����Ϊ�ǿͻ���
						  LPNCCALCSIZE_PARAMS lpcp = (LPNCCALCSIZE_PARAMS)lParam;
						  RECT rect;
						  GcchMeasureString(g_bufferBitmap, _T("���"), 2, &rect, 0); // �ı��ĸ߶ȶ���һ����
						  groupBox->titleHeight = GcchGetRectHeight(&rect);
						  GcchRectAdd(&lpcp->rgrc[0], 4, groupBox->titleHeight, -4, -4);
						  return 0;
	}
	case WM_NCPAINT:
	{
					   HDC hdc = GetWindowDC(hWnd);
					   GcchBitmap bitmap = { 0 };
					   bitmap.hdc = hdc;
					   RECT winRect, rect, srcRect, sliceRect;
					   TCHAR text[64];
					   GetWindowRect(hWnd, &winRect);
					   // ��Ҫ�����Ͻ���������
					   GcchMoveRect(&winRect, -winRect.left, -winRect.top);
					   // ���Ʊ���
					   GcchFillRectangle(g_bufferBitmap, &winRect, groupBox->background);
					   // ���Ʊ߿�
					   GcchRectBySize(&srcRect, 0, 0, 36, 36);
					   GcchRect(&sliceRect, 4, 4, 4, 4);
					   // �߿���ϱ�Ҫ����
					   GcchRect(&rect, 0, groupBox->titleHeight / 2 - 2, GcchGetRectWidth(&winRect), winRect.bottom);
					   GcchDrawSliceBitmap(g_bufferBitmap, &rect, g_borderBitmap, &srcRect, &sliceRect);
					   // �����ı�
					   GetWindowText(hWnd, text, ARRAYSIZE(text));
					   GcchMeasureString(g_bufferBitmap, text, -1, &rect, 0);
					   // �����ı�����
					   GcchRectAdd(&rect, 10, 0, 16, 0);  // ����һ��߾�
					   GcchFillRectangle(g_bufferBitmap, &rect, groupBox->background);
					   GcchDrawString(g_bufferBitmap, text, rect.left + 3, rect.top, groupBox->foreground);
					   // ���Ƶ�������
					   GcchDrawBitmap(&bitmap, &winRect, g_bufferBitmap, 0, 0);
					   ReleaseDC(hWnd, hdc);
					   return 0;
	}
	case WM_PAINT:
	{
					 PAINTSTRUCT ps;
					 GcchBitmap bitmap = { 0 };
					 bitmap.hdc = BeginPaint(hWnd, &ps);
					 RECT rect;
					 GetClientRect(hWnd, &rect);
					 GcchFillRectangle(&bitmap, &rect, RGB(0, 250, 250));
					 EndPaint(hWnd, &ps);
					 return 0;
	}
	}
	return GcchDefButtonFunc((GcchButtonBase*)groupBox, hWnd, msg, wParam, lParam);
}

HWND GcchCreateGroupBox(LPCTSTR text, int x, int y, int width, int height, HWND hWndParent, UINT id)
{
	GcchGroupBox control = { 0 };
	GcchInitControl((GcchControl*)&control, (GcchControlFunc)GroupBoxFunc, NULL,
		NULL, id, sizeof(GcchGroupBox), GCCH_CT_GROUPBOX);
	control.background = RGB(250, 250, 250);
	control.foreground = RGB(0, 0, 0);
	return GcchCreateControl(0, text, WS_CHILD | WS_VISIBLE | WS_BORDER,
		x, y, width, height,
		hWndParent, (HMENU)id, (GcchControl*)&control);
}

static void ScrollListBox(GcchListBox* listBox, BOOL redraw)
{
	RECT rect;
	GcchList* list = listBox->list;
	GcchListNode* item;
	int count;

	GetClientRect(listBox->hWnd, &rect);
	if (list)
	{
		item = list->head;
		count = 0;
		for (count = 0; item; ++count)
			item = item->next;
	}
	else
	{
		count = listBox->columns * listBox->rows;
	}

	DWORD style = GetWindowLong(listBox->hWnd, GWL_STYLE);
	int hPos = GetScrollPos(listBox->hWnd, SB_HORZ);
	int vPos = GetScrollPos(listBox->hWnd, SB_VERT);
	SCROLLINFO scrollInfo = { 0 };
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE;
	if ((style & WS_HSCROLL) != 0)
	{
		scrollInfo.nPage = rect.right - rect.left;
		scrollInfo.nMax = listBox->itemWidth * listBox->columns +listBox->paddingX - 1;
	}
	SetScrollInfo(listBox->hWnd, 0, &scrollInfo, redraw);

	memset(&scrollInfo, 0, sizeof(scrollInfo));
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE;
	if ((style & WS_VSCROLL) != 0)
	{
		scrollInfo.nPage = rect.bottom - rect.top;
		scrollInfo.nMax = listBox->itemHeight * ((listBox->columns + count - 1) / listBox->columns) +listBox->paddingY - 1;
	}

	SetScrollInfo(listBox->hWnd, SB_VERT, &scrollInfo, redraw);
	int deltaX = hPos - GetScrollPos(listBox->hWnd, SB_HORZ);
	int deltaY = vPos - GetScrollPos(listBox->hWnd, SB_VERT);
	if (deltaX || deltaY)
		ScrollWindowEx(listBox->hWnd, deltaX, deltaY, 0, 0, 0, 0, SW_INVALIDATE);
}

static void DrawListBoxItem(GcchListBox* listBox, GcchBitmap* bmp, GcchListBoxItem* listBoxItem, GcchListNode *item, LPRECT rect)
{
	COLORREF textColor, backColor;
	if ((listBoxItem->state & 4) != 0)
	{
		textColor = 0;
		backColor = RGB(255, 255, 255);
		if ((listBoxItem->state & GCCH_CS_CHECKED) != 0)		// ѡ��
		{
			backColor = RGB(184, 184, 192);
		}
		else if ((listBoxItem->state & GCCH_CS_HOVER) != 0)	// ��ͣ
		{
			backColor = RGB(224, 224, 232);
		}
	}
	else
	{
		textColor = RGB(192, 192, 192);
		backColor = RGB(255, 255, 255);
	}
	GcchFillRectangle(bmp, rect, backColor);
	GcchRectBySize(rect, 4, 0, listBoxItem->rect.right - listBoxItem->rect.left - 8, listBoxItem->rect.bottom - listBoxItem->rect.top);
	GcchDrawStringEx(bmp, item->text, -1, rect, textColor, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
}

static void InitListBoxItem(GcchListBoxItem* listBoxItem, int row, int column)
{
	listBoxItem->index = -1;
	listBoxItem->item = NULL;
	listBoxItem->state = 0;
	listBoxItem->row = row;
	listBoxItem->column = column;
	listBoxItem->x = 0;
	listBoxItem->y = 0;
}

static LRESULT OnListBoxPaint(GcchListBox* listBox)
{
	PAINTSTRUCT paint;
	HWND hWnd = listBox->hWnd;
	HDC hdc = BeginPaint(hWnd, &paint);
	GcchBitmap bitmap = { 0 };
	GcchBitmap* bmp = g_bufferBitmap;
	bitmap.hdc = hdc;
	
	BOOL isEnable = IsWindowEnabled(hWnd);
	RECT clientRect, rect;
	GetClientRect(hWnd, &clientRect);
	int posH = GetScrollPos(hWnd, SB_HORZ);
	int posV = GetScrollPos(hWnd, SB_VERT);

	if (-posV >= clientRect.bottom)
	{
		EndPaint(hWnd, &paint);
		return 0;
	}

	GcchListBoxItem listBoxItem;
	listBoxItem.rect.left = clientRect.left;
	listBoxItem.rect.right = clientRect.right;
	GcchListNode *item = listBox->list ? listBox->list->head : NULL;
	listBoxItem.rect.top = -posV;
	listBoxItem.rect.bottom = listBoxItem.rect.top + listBox->itemHeight;
	int index = 0;
	for (int row = 0; listBoxItem.rect.top < clientRect.bottom; ++row)
	{
		for (int column = 0; column < listBox->columns || listBoxItem.rect.left < clientRect.right; ++column)
		{
			listBoxItem.rect.left = listBox->itemWidth * column - posH;
			listBoxItem.rect.right = listBoxItem.rect.left + listBox->itemWidth;
			if (GcchRectAnd(NULL, &listBoxItem.rect, &paint.rcPaint))
			{
				InitListBoxItem(&listBoxItem, row, column);
				if (column < listBox->columns)
				{
					listBoxItem.item = item;
					listBoxItem.index = index;
					if (isEnable)
					{
						listBoxItem.state = GCCH_CS_ENABLE;
						if (index == listBox->selectIndex)
							listBoxItem.state |= GCCH_CS_HOVER;
						if (index == listBox->listBoxItem.index)
						{
							listBoxItem.x = listBox->listBoxItem.x;
							listBoxItem.y = listBox->listBoxItem.y;
							listBoxItem.state |= GCCH_CS_CHECKED;
						}
					}
				}
				if (RaiseEvent((GcchControl*)listBox, hWnd, WM_USER_DRAW_ITEM, (WPARAM)bmp, (LPARAM)&listBoxItem))
				{
					if (!isEnable)
					{
						GcchHalfTone(bmp, &listBoxItem.rect);
					}
				}
				else
				{
					GcchRectBySize(&rect, 0, 0, listBoxItem.rect.right - listBoxItem.rect.left, listBoxItem.rect.bottom - listBoxItem.rect.top);
					if (item)
						DrawListBoxItem(listBox, bmp, &listBoxItem, item, &rect);
					else
						GcchFillRectangle(bmp, &rect, RGB(192, 192, 192));
				}
				GcchDrawBitmap(&bitmap, &listBoxItem.rect, bmp, 0, 0);
			}

			if (column < listBox->columns)
			{
				if (item)
					item = item->next;
				++index;
			}
		}
		listBoxItem.rect.top += listBox->itemHeight;
		listBoxItem.rect.bottom += listBox->itemHeight;
	}
	EndPaint(hWnd, &paint);
	return 0;
}

BOOL GetListBoxItem(GcchListBox* listBox, int x, int y, BOOL containsLastRow, GcchListBoxItem *listBoxItem)
{
	RECT rect;

	memset(listBoxItem, 0, sizeof(GcchListBoxItem));
	InitListBoxItem(listBoxItem, 0, 0);
	listBoxItem->item = 0;
	listBoxItem->index = -1;
	listBoxItem->column = x;
	listBoxItem->row = y;

	GetClientRect(listBox->hWnd, &rect);
	if (containsLastRow)
		rect.bottom += listBox->itemHeight;

	if (!GcchRectContains(&rect, x, y))
		return FALSE;

	int posH = GetScrollPos(listBox->hWnd, SB_HORZ);
	int posV = GetScrollPos(listBox->hWnd, SB_VERT);
	GcchMoveRect(&rect, posH, posV);
	if (rect.bottom <= 0)
		return FALSE;

	x += posH;
	y += posV;

	GcchListNode *item = listBox->list ? listBox->list->head : NULL;
	int index = 0;
	for (int row = 0;; ++row)
	{
		listBoxItem->rect.top = listBox->itemHeight * row;
		if (listBoxItem->rect.top >= rect.bottom) // �������ӿڵ��±�
			break;
		listBoxItem->rect.bottom = listBoxItem->rect.top + listBox->itemHeight;
		for (int col = 0; col < listBox->columns; ++col)
		{
			listBoxItem->rect.left = listBox->itemWidth * col;
			if (listBoxItem->rect.left >= rect.right) // �������ӿڵ��ұ�
				break;
			listBoxItem->rect.right = listBoxItem->rect.left + listBox->itemWidth;

			// �ж�����Ƿ�����ǰ��
			if (GcchRectContains(&listBoxItem->rect, x, y))
			{
				listBoxItem->x = x - listBoxItem->rect.left;
				listBoxItem->y = y - listBoxItem->rect.top;
				listBoxItem->item = item;
				listBoxItem->index = index;
				return TRUE;
			}

			if (item)
				item = item->next;
			++index;
		}
	}
	return FALSE;
}

static BOOL GetListNodeRect(GcchListBox *listBox, GcchListNode *item, LPRECT rect)
{
	GcchList* list = listBox->list;
	GcchListNode* node;
	if (!list)
		return FALSE;

	node = list->head;
	for (int row = 0; node; ++row)
	{
		for (int column = 0; node && column < listBox->columns; ++column)
		{
			if (node == item)
			{
				int posH = GetScrollPos(listBox->hWnd, SB_HORZ);
				int posV = GetScrollPos(listBox->hWnd, SB_VERT);
				GcchRect(rect, column * listBox->itemWidth - posH, row * listBox->itemHeight - posV, listBox->itemWidth, listBox->itemHeight);
				return TRUE;
			}
			node = node->next;
		}
	}
	return FALSE;
}

static BOOL RedrawListBoxItem(GcchListBox *listBox, GcchListBoxItem *listBoxItem)
{
	RECT rect;
	if (TRUE) // û���
	{
		int posH =GetScrollPos(listBox->hWnd, SB_HORZ);
		int posV =GetScrollPos(listBox->hWnd, SB_VERT);
		rect = listBoxItem->rect;
		GcchMoveRect(&rect, -posH, -posV);
		return GcchRedrawRect(listBox->hWnd, &rect);
	}

	if (GetListNodeRect(listBox, listBoxItem->item, &rect))
		return GcchRedrawRect(listBox->hWnd, &rect);

	return GcchRedraw(listBox->hWnd);
}

static LRESULT ListBoxFunc(GcchListBox* listBox, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int rows, columns;
	RECT rect;
	GcchListBoxItem listBoxItem;
	switch (msg)
	{
	case WM_USER_SET_ITEMSIZE:
		listBox->itemWidth = wParam;
		listBox->itemHeight = lParam;
		ScrollListBox(listBox, TRUE);
		return 0;
	case WM_USER_SET_ROWS:
		rows = wParam;
		if (!wParam)
			rows = 1;
		if (listBox->rows != rows)
		{
			listBox->rows = rows;
			ScrollListBox(listBox, TRUE);
		}
		return 0;
	case WM_USER_SET_COLUMNS:
		columns = wParam;
		if (!wParam)
			columns = 1;
		if (listBox->columns != columns)
		{
			listBox->columns = columns;
			ScrollListBox(listBox, TRUE);
		}
		return 0;
	case WM_USER_SET_PADDING:
		listBox->paddingX = wParam;
		listBox->paddingY = lParam;
		ScrollListBox(listBox, TRUE);
		return 0;
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		listBox->listBoxItem.index = -1;
		listBox->listBoxItem.item = 0;
		if (!listBox->itemWidth)
		{
			listBox->itemWidth = rect.right - rect.left;
			if (listBox->itemWidth <= 0)
				listBox->itemWidth = 16;
		}
		if (!listBox->itemHeight)
		{
			listBox->itemHeight = 16;
			if (listBox->itemHeight <= 0)
				listBox->itemHeight = 16;
		}
		ScrollListBox(listBox, FALSE);
		RaiseEvent((GcchControl*)listBox, hWnd, WM_USER_INIT, 0, 0);
		return 0;
	case WM_SIZE:
		if ((listBox->state & GCCH_LBS_AUTO_ITEM_SIZE) != 0)
		{
			GetClientRect(hWnd, &rect);
			listBox->itemWidth = rect.right - rect.left;
		}
		ScrollListBox(listBox, TRUE);
		return 0;
	case WM_PAINT:
		return OnListBoxPaint(listBox);
	case WM_MOUSEMOVE:
		if ((listBox->state & GCCH_CS_HOVER) == 0)
		{
			listBox->state |= GCCH_CS_HOVER;
		}
		GetListBoxItem(listBox, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE, &listBoxItem);
		if (listBox->listBoxItem.index != listBoxItem.index)
		{
			if (listBoxItem.index >= 0)
				RedrawListBoxItem(listBox, &listBoxItem);
			if (listBox->listBoxItem.index >= 0)
				RedrawListBoxItem(listBox, &listBox->listBoxItem);
			memcpy(&listBox->listBoxItem, &listBoxItem, sizeof(listBoxItem));
		}
		return 0;
	case WM_MOUSELEAVE:
		listBox->state &= ~GCCH_CS_HOVER;
		if (listBox->listBoxItem.index >= 0)
		{
			listBox->listBoxItem.index = -1;
			listBox->listBoxItem.item = NULL;
			int posH = GetScrollPos(listBox->hWnd, SB_HORZ);
			int posV = GetScrollPos(listBox->hWnd, SB_VERT);
			rect = listBox->listBoxItem.rect;
			GcchMoveRect(&rect, -posH, -posV);
			GcchRedrawRect(hWnd, &rect);
		}
		return 0;
	case WM_LBUTTONDOWN:
		GetListBoxItem(listBox, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), FALSE, &listBoxItem);
		RaiseEvent((GcchControl*)listBox, hWnd, WM_USER_LSBX_LDOWN, 0, (WPARAM)&listBoxItem);
		return 0;
	}
	return GcchDefControlFunc((GcchControl*)listBox, hWnd, msg, wParam, lParam);
}

HWND GcchCreateListBox(DWORD exStyle, DWORD style, int x, int y, int width, int height,
	HWND parent, UINT id, GcchEventFunc eventHandler, LPVOID data)
{
	GcchListBox control = { 0 };
	GcchInitControl((GcchControl*)&control, (GcchControlFunc)ListBoxFunc, eventHandler,
		data, id, sizeof(GcchListBox), GCCH_CT_LISTBOX);
	control.itemWidth = 16;
	control.itemHeight = 16;
	control.rows = 1;
	control.columns = 1;
	
	return GcchCreateControl(exStyle, NULL, style, x, y, width, height,
		parent, (HMENU)id, (GcchControl*)&control);
}

void GcchSetItemSize(HWND hWnd, int width, int height)
{
	SendMessage(hWnd, WM_USER_SET_ITEMSIZE, width, height);
}

void GcchSetRows(HWND hWnd, int rows)
{
	SendMessage(hWnd, WM_USER_SET_ROWS, rows, 0);
}

void GcchSetColumns(HWND hWnd, int columns)
{
	SendMessage(hWnd, WM_USER_SET_COLUMNS, columns, 0);
}

void GcchSetPadding(HWND hWnd, int paddingX, int paddingY)
{
	SendMessage(hWnd, WM_USER_SET_PADDING, paddingX, paddingY);
}
