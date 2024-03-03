#pragma once
#include <Windows.h>

// 初始化, 用于创建需要的资源，返回是否初始化成功
BOOL GcchInitialize(HINSTANCE hInstance);
// 反初始化，释放资源
void GcchUninitialize();
// 分配 bytes 大小的内存
LPVOID GcchMalloc(size_t bytes);
// 释放内存
void GcchFree(LPVOID* ptr);

// 消息处理函数类型
typedef LRESULT(*GcchControlFunc)(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam,
	LPVOID data
	);

// 控件
typedef struct GcchControl
{
	HWND hWnd;		// 窗口句柄
	GcchControlFunc func;		// 消息处理函数
	LPVOID data;		// 附加数据，用于扩展
}GcchControl;


// 创建控件, 对 CreateWindowEx 的封装
HWND GcchCreateControl(DWORD exStyle, LPCTSTR text, DWORD style,
	int x, int y, int width, int height, HWND parent, HMENU menu,
	GcchControlFunc func,
	LPVOID data);


// 控件的默认消息处理函数
LRESULT GcchDefControlFunc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam,
	LPVOID data);

// 使用 左，上，右，下 四个值来构造
LPRECT GcchRect(LPRECT rect, LONG left, LONG top, LONG right, LONG bottom);
// 使用 左上角坐标和大小来构造
LPRECT GcchRectBySize(LPRECT rect, LONG x, LONG y, LONG width, LONG height);
// 使用 左上角 和 右下角 两个点来构造
LPRECT GcchRectByPoints(LPRECT rect, POINT leftTop, POINT rightBottom);
// 判断矩形是否有效，也就是对应的区域面积大于0
BOOL GcchRectValid(LPCRECT rect);
// 修正矩形，使用矩形的左边不大于右边，上边不大于下边
LPRECT GcchRectCorrect(LPRECT rect);
// 计算两个矩形的交集，返回是否相交, outRect 如果不为空，则返回交集
BOOL GcchRectAnd(LPRECT outRect, LPCRECT rect1, LPCRECT rect2);
// 计算两个矩形的并集, outRect 如果返回并集，不能为空
LPRECT GcchRectOr(LPRECT outRect, LPCRECT rect1, LPCRECT rect2);
// 判断点是否在矩形内
BOOL GcchRectContains(LPCRECT rect, LONG x, LONG y);
// 平移矩形, 矩形的各个分量分别分别加上指定数值
LPRECT GcchRectAdd(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// 缩放矩形，矩形的各个分量分别左移 / 右移指定数值
LPRECT GcchRectShiftLeft(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
LPRECT GcchRectShiftRight(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// 让一个矩形在另一个矩形中居中
LPRECT GcchMakeRectCenter(LPRECT rect, LPCRECT bound);


// 创建窗口控件，窗口控件是狭义的窗口，仅是指容纳其他控件的容器
HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style,
	int width, int height,
	GcchControlFunc func,
	LPVOID data);
// 显示窗口
void GcchShowWindow(HWND hWnd, int nCmdShow);