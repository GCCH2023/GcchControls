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
