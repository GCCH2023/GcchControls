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
HWND GcchCreateControl(DWORD exStyle, LPCWSTR text, DWORD style,
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
