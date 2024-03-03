#pragma once
#include <Windows.h>

// ��ʼ��, ���ڴ�����Ҫ����Դ�������Ƿ��ʼ���ɹ�
BOOL GcchInitialize(HINSTANCE hInstance);
// ����ʼ�����ͷ���Դ
void GcchUninitialize();
// ���� bytes ��С���ڴ�
LPVOID GcchMalloc(size_t bytes);
// �ͷ��ڴ�
void GcchFree(LPVOID* ptr);

// ��Ϣ����������
typedef LRESULT(*GcchControlFunc)(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam,
	LPVOID data
	);

// �ؼ�
typedef struct GcchControl
{
	HWND hWnd;		// ���ھ��
	GcchControlFunc func;		// ��Ϣ������
	LPVOID data;		// �������ݣ�������չ
}GcchControl;


// �����ؼ�, �� CreateWindowEx �ķ�װ
HWND GcchCreateControl(DWORD exStyle, LPCWSTR text, DWORD style,
	int x, int y, int width, int height, HWND parent, HMENU menu,
	GcchControlFunc func,
	LPVOID data);


// �ؼ���Ĭ����Ϣ������
LRESULT GcchDefControlFunc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam,
	LPVOID data);
