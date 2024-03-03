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
HWND GcchCreateControl(DWORD exStyle, LPCTSTR text, DWORD style,
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

// ʹ�� ���ϣ��ң��� �ĸ�ֵ������
LPRECT GcchRect(LPRECT rect, LONG left, LONG top, LONG right, LONG bottom);
// ʹ�� ���Ͻ�����ʹ�С������
LPRECT GcchRectBySize(LPRECT rect, LONG x, LONG y, LONG width, LONG height);
// ʹ�� ���Ͻ� �� ���½� ������������
LPRECT GcchRectByPoints(LPRECT rect, POINT leftTop, POINT rightBottom);
// �жϾ����Ƿ���Ч��Ҳ���Ƕ�Ӧ�������������0
BOOL GcchRectValid(LPCRECT rect);
// �������Σ�ʹ�þ��ε���߲������ұߣ��ϱ߲������±�
LPRECT GcchRectCorrect(LPRECT rect);
// �����������εĽ����������Ƿ��ཻ, outRect �����Ϊ�գ��򷵻ؽ���
BOOL GcchRectAnd(LPRECT outRect, LPCRECT rect1, LPCRECT rect2);
// �����������εĲ���, outRect ������ز���������Ϊ��
LPRECT GcchRectOr(LPRECT outRect, LPCRECT rect1, LPCRECT rect2);
// �жϵ��Ƿ��ھ�����
BOOL GcchRectContains(LPCRECT rect, LONG x, LONG y);
// ƽ�ƾ���, ���εĸ��������ֱ�ֱ����ָ����ֵ
LPRECT GcchRectAdd(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// ���ž��Σ����εĸ��������ֱ����� / ����ָ����ֵ
LPRECT GcchRectShiftLeft(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
LPRECT GcchRectShiftRight(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// ��һ����������һ�������о���
LPRECT GcchMakeRectCenter(LPRECT rect, LPCRECT bound);


// �������ڿؼ������ڿؼ�������Ĵ��ڣ�����ָ���������ؼ�������
HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style,
	int width, int height,
	GcchControlFunc func,
	LPVOID data);
// ��ʾ����
void GcchShowWindow(HWND hWnd, int nCmdShow);