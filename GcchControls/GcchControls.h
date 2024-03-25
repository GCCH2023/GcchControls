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
// 用于实现控件的逻辑
// 比如如何绘制按钮
typedef LRESULT(*GcchControlFunc)(
	struct GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// 事件处理函数类型
// 用于处理控件产生的事件
// 比如点击按钮时要做什么
typedef LRESULT(*GcchEventFunc)(
struct GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// 触发事件
LRESULT RaiseEvent(struct GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// 附加消息
typedef enum GcchMessage
{
	// 设置背景颜色，wParm 表示颜色
	WM_USER_SET_BACKGROUND = WM_USER + 1,
	// 设置前景颜色，wParm 表示颜色
	WM_USER_SET_FOREGROUND,
	// 设置排列方式，wParam 是 水平排列方式，lParam 是垂直排列方式
	WM_USER_SET_ALIGNMENT,
	// 设置字体，wParam 是 GcchFont 指针
	WM_USER_SET_FONT,
	// 设置控件的大小, wParam 是宽度，lParam 是高度
	WM_USER_SET_SIZE,

	// 按钮被点击，没有参数
	WM_USER_BUTTON_CLICK,
	// 按钮选中状态切换，wParam 是按钮是否选中
	WM_USER_CHECK_CHANGED,
	// 设置选中状态，wParam 表示是否选中
	WM_USER_SET_CHECK,
	// 获取选中状态，没有参数
	WM_USER_GET_CHECK,
	// 切换选中状态，没有参数
	WM_USER_SWITCH_CHECK
}GcchMessage;

// 枚举控件类型
typedef enum GcchControlType
{
	GCCH_CT_NONE,		// 不属于自定义控件
	GCCH_CT_WINDOW,		// 窗口控件
	GCCH_CT_LABEL,		// 标签控件
	GCCH_CT_BUTTON,		// 按钮控件
	GCCH_CT_CHECKBOX,		// 复选框控件
}GcchControlType;

// 水平排列方式枚举
typedef enum GcchHorizontalAlignment
{
	GCCH_HA_LEFT = DT_LEFT,
	GCCH_HA_CENTER = DT_CENTER,
	GCCH_HA_RIGHT = DT_RIGHT,
}GcchHorizontalAlignment;

// 垂直排列方式枚举
typedef enum GcchVerticalAlignment
{
	GCCH_VA_TOP = DT_TOP,
	GCCH_VA_CENTER = DT_VCENTER,
	GCCH_VA_BOTTOM = DT_BOTTOM,
}GcchVerticalAlignment;

// 控件的一些状态枚举
// 比如正常状态，鼠标悬停状态等
typedef enum GcchControlState
{
	GCCH_CS_NORMAL = 0,		// 正常
	GCCH_CS_HOVER = 1,		// 鼠标悬停
	GCCH_CS_PRESSED = 2,	// 鼠标按下
	GCCH_CS_CHECKED = 2,	// 按钮选中
}GcchControlState;

// 窗口句柄
// 消息处理函数
// 附加数据，用于扩展
// 控件id，窗口控件没有id
// 控件结构体占用的字节大小
// 控件类型
// width 和 height 是设计的大小，不一定是最终大小
// 如果它们大于0，怎么就是控件的实际大小
// 否则控件将自动计算大小
#define GCCHCTRL\
	HWND hWnd; \
	GcchControlFunc func; \
	LPVOID data; \
	UINT id; \
	size_t bytes; \
	int type;\
	int width;\
	int height;\
	DWORD state; \
	GcchEventFunc eventHandler;

// 控件
typedef struct GcchControl
{
	GCCHCTRL
}GcchControl;

// 初始化控件的基本字段
void GcchInitControl(GcchControl* control, GcchControlFunc func, GcchEventFunc eventHandler, LPVOID data,
	UINT id, size_t bytes, GcchControlType type);

// 获取控件的类型
GcchControlType GcchGetControlType(HWND hWnd);

// 创建控件, 对 CreateWindowEx 的封装
HWND GcchCreateControl(DWORD exStyle, LPCTSTR text, DWORD style,
	int x, int y, int width, int height, HWND parent, HMENU menu, GcchControl* control);

// 控件的默认消息处理函数
LRESULT GcchDefControlFunc(
	GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// 设置控件的大小
void GcchSetControlSize(HWND hWnd, int width, int height);

// 重绘窗口的全部
BOOL GcchRedraw(HWND hWnd);
// 马上重绘窗口的全部
BOOL GcchRedrawNow(HWND hWnd);

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
// 判断两个矩形是否相等
BOOL GcchRectEqual(LPCRECT rect1, LPCRECT rect2);


// 字体
typedef struct GcchFont
{
	HFONT hFont;
	int width;
	int height;
	int weight;
	TCHAR familyName[32];
}GcchFont;

// 创建字体
GcchFont* GcchCreateFont(int width, int height, int weight, LPCTSTR familyName);
// 销毁字体
void GcchDestroyFont(GcchFont** pFont);

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
// 如果从文件加载，flags 需要设置为 LR_LOADFROMFILE
GcchBitmap* GcchLoadBitmap(LPCTSTR filename, UINT flags);
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
// 设置字体, 必须与UnsetFont配对使用
BOOL GcchSetFont(GcchBitmap* bitmap, GcchFont* font);
// 取消设置字体
BOOL GcchUnsetFont(GcchBitmap* bitmap);


// 窗口控件类
typedef struct GcchWindow
{
	GCCHCTRL
}GcchWindow;

// 创建窗口控件，窗口控件是狭义的窗口，仅是指容纳其他控件的容器
HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style,
	int width, int height,
	GcchControlFunc func, GcchEventFunc eventHandler,
	LPVOID data);
// 显示窗口
void GcchShowWindow(HWND hWnd, int nCmdShow);
// 设置窗口的标题
void GcchSetWindowTitle(HWND hWnd, LPCTSTR title);

// 标签
// 仅支持单行文本
typedef struct GcchLabel
{
	GCCHCTRL
	COLORREF foreground;		// 文本颜色
	COLORREF background;		// 背景颜色
	GcchHorizontalAlignment horizontalAlignment;		// 水平排列方式
	GcchVerticalAlignment verticalAlignment;		// 垂直排列方式
	// 字体
	GcchFont* font;
}GcchLabel;

// 在(x, y, width, height) 指定的区域创建标签
// 如果 width 或 height 为 0，则自动计算
HWND GcchCreateLabel(HWND hWndParent, UINT id, int x, int y, int width, int height, LPCTSTR text);
// 在(x, y, width, height) 指定的区域创建标签
// 如果 width 或 height 为 0，则自动计算
HWND GcchCreateLabelEx(HWND hWndParent, UINT id, int x, int y, int width, int height, LPCTSTR text,
	GcchHorizontalAlignment horizontalAlignment, GcchVerticalAlignment verticalAlignment);
// 设置标签的文本
BOOL GcchSetLabelText(HWND hWnd, LPCTSTR text);
// 设置标签的背景颜色
void GcchSetLabelBackground(HWND hWnd, COLORREF color);
// 设置标签的文本颜色
void GcchSetLabelForeground(HWND hWnd, COLORREF color);
// 设置文本的排列方式
void GcchSetLabelAlignment(HWND hWnd, GcchHorizontalAlignment horizontalAlignment, GcchVerticalAlignment verticalAlignment);
// 设置标签的字体
void GcchSetLabelFont(HWND hWnd, GcchFont* font);

typedef struct GcchButtonBase
{
	GCCHCTRL
}GcchButtonBase;

// 按钮的共同代码封装
LRESULT GcchDefButtonFunc(
	GcchButtonBase* button,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// 按钮
typedef struct GcchButton
{
	GCCHCTRL
}GcchButton;

// 创建一个按钮
HWND GcchCreateButton(LPCTSTR text, int x, int y, int width, int height,
	HWND hWndParent, UINT id, GcchEventFunc eventHandler, LPVOID data);

// 复选框
typedef struct GcchCheckBox
{
	GCCHCTRL
}GcchCheckBox;

// 创建一个复选框
HWND GcchCreateCheckBox(LPCTSTR text, int x, int y, 
	HWND hWndParent, UINT id, GcchEventFunc eventHandler, LPVOID data);
void GcchSetCheck(HWND hWnd, BOOL isCheck);
void GcchSwitchCheck(HWND hWnd);
BOOL GcchGetCheck(HWND hWnd);
