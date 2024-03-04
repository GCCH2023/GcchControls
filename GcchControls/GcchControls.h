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
// 获取矩形的宽度
LONG GcchGetRectWidth(LPCRECT rect);
// 获取矩形的高度
LONG GcchGetRectHeight(LPCRECT rect);
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
// 平移矩形
LPRECT GcchMoveRect(LPRECT rect, LONG deltaX, LONG deltaY);
// 分量相加，矩形的各个分量分别分别加上指定数值
LPRECT GcchRectAdd(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// 缩放矩形，矩形的各个分量分别左移 / 右移指定数值
LPRECT GcchRectShiftLeft(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
LPRECT GcchRectShiftRight(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// 让一个矩形在另一个矩形中居中
LPRECT GcchMakeRectCenter(LPRECT rect, LPCRECT bound);


// 位图, 用于绘图
typedef struct GcchBitmap
{
	HDC hdc;
	HBITMAP bitmap;
	HBITMAP oldBitmap;
	HFONT font;
	HFONT oldFont;
	int width;
	int height;
	DWORD* pBits;		// 位图数据
}GcchBitmap;

// 创建指定大小的位图
GcchBitmap* GcchCreateBitmap(int width, int height);
// 从文件或资源加载位图, 只支持bmp格式
GcchBitmap* GcchLoadBitmap(LPCTSTR filename);
// 销毁位图
void GcchDestroyBitmap(GcchBitmap** pBitmap);
// 绘制矩形区域
BOOL GcchDrawRectangle(GcchBitmap* bitmap, LPCRECT rect, COLORREF color);
// 填充矩形区域
BOOL GcchFillRectangle(GcchBitmap* bitmap, LPCRECT rect, COLORREF color);
// 绘制位图
// 将 srcBitmap 从 (srcX, srcY) 处的 rect 大小的像素数据绘制到 bitmap 中的 rect 区域
BOOL GcchDrawBitmap(GcchBitmap *bitmap, LPCRECT rect, GcchBitmap *srcBitmap, LONG srcX, LONG srcY);
// 将 srcBitmap 的 srcRect 区域绘制到 bitmap 的rect 区域
BOOL GcchDrawBitmapEx(GcchBitmap *bitmap, LPCRECT rect, GcchBitmap *srcBitmap, LPCRECT srcRect);
// 九宫格填充
// 将一个矩形横竖各切两下，分割成9份，
// 位于 4 个角的部分直接绘制到目标区域的 4 个角
// 位于 4 条边的进行该边上的拉伸绘制到目标区域的 4 条边上
// 位于 中间 的部分拉伸到目标区域的剩余部分
// srcRect ：位图源矩形
// sliceRect ：对位图源矩形按九宫格划分
BOOL GcchDrawSliceBitmap(GcchBitmap* bitmap, LPCRECT rect, GcchBitmap *srcBmp, LPCRECT srcRect, LPCRECT sliceRect);
// 在目标位置绘制字符串
BOOL GcchDrawString(GcchBitmap *bitmap, LPCTSTR text, LONG x, LONG y, COLORREF color);
// 在目标区域内绘制字符串
// format 是 DT_ 开头的宏
// count 是字符的数量，如果为 -1 则表示绘制全部字符
BOOL GcchDrawStringEx(GcchBitmap *bitmap, LPCTSTR text, int count, LPRECT rect, COLORREF color, UINT format);
// 测量绘制字符串需要的大小
// rect 用于返回绘制字符串需要的区域大小
// 返回区域的宽度
LONG GcchMeasureString(GcchBitmap *bitmap, LPCTSTR text, int count, LPRECT rect, UINT format);
// 将 DIB 的色调将为一半
BOOL GcchHalfTone(GcchBitmap* bitmap, LPCRECT rect);

// 创建窗口控件，窗口控件是狭义的窗口，仅是指容纳其他控件的容器
HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style,
	int width, int height,
	GcchControlFunc func,
	LPVOID data);
// 显示窗口
void GcchShowWindow(HWND hWnd, int nCmdShow);
// 设置窗口的标题
void GcchSetWindowTitle(HWND hWnd, LPCTSTR title);


