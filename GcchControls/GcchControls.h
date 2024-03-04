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
// ��ȡ���εĿ��
LONG GcchGetRectWidth(LPCRECT rect);
// ��ȡ���εĸ߶�
LONG GcchGetRectHeight(LPCRECT rect);
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
// ƽ�ƾ���
LPRECT GcchMoveRect(LPRECT rect, LONG deltaX, LONG deltaY);
// ������ӣ����εĸ��������ֱ�ֱ����ָ����ֵ
LPRECT GcchRectAdd(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// ���ž��Σ����εĸ��������ֱ����� / ����ָ����ֵ
LPRECT GcchRectShiftLeft(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
LPRECT GcchRectShiftRight(LPRECT rect, LONG dLeft, LONG dTop, LONG dRight, LONG dBottom);
// ��һ����������һ�������о���
LPRECT GcchMakeRectCenter(LPRECT rect, LPCRECT bound);


// λͼ, ���ڻ�ͼ
typedef struct GcchBitmap
{
	HDC hdc;
	HBITMAP bitmap;
	HBITMAP oldBitmap;
	HFONT font;
	HFONT oldFont;
	int width;
	int height;
	DWORD* pBits;		// λͼ����
}GcchBitmap;

// ����ָ����С��λͼ
GcchBitmap* GcchCreateBitmap(int width, int height);
// ���ļ�����Դ����λͼ, ֻ֧��bmp��ʽ
GcchBitmap* GcchLoadBitmap(LPCTSTR filename);
// ����λͼ
void GcchDestroyBitmap(GcchBitmap** pBitmap);
// ���ƾ�������
BOOL GcchDrawRectangle(GcchBitmap* bitmap, LPCRECT rect, COLORREF color);
// ����������
BOOL GcchFillRectangle(GcchBitmap* bitmap, LPCRECT rect, COLORREF color);
// ����λͼ
// �� srcBitmap �� (srcX, srcY) ���� rect ��С���������ݻ��Ƶ� bitmap �е� rect ����
BOOL GcchDrawBitmap(GcchBitmap *bitmap, LPCRECT rect, GcchBitmap *srcBitmap, LONG srcX, LONG srcY);
// �� srcBitmap �� srcRect ������Ƶ� bitmap ��rect ����
BOOL GcchDrawBitmapEx(GcchBitmap *bitmap, LPCRECT rect, GcchBitmap *srcBitmap, LPCRECT srcRect);
// �Ź������
// ��һ�����κ����������£��ָ��9�ݣ�
// λ�� 4 ���ǵĲ���ֱ�ӻ��Ƶ�Ŀ������� 4 ����
// λ�� 4 ���ߵĽ��иñ��ϵ�������Ƶ�Ŀ������� 4 ������
// λ�� �м� �Ĳ������쵽Ŀ�������ʣ�ಿ��
// srcRect ��λͼԴ����
// sliceRect ����λͼԴ���ΰ��Ź��񻮷�
BOOL GcchDrawSliceBitmap(GcchBitmap* bitmap, LPCRECT rect, GcchBitmap *srcBmp, LPCRECT srcRect, LPCRECT sliceRect);
// ��Ŀ��λ�û����ַ���
BOOL GcchDrawString(GcchBitmap *bitmap, LPCTSTR text, LONG x, LONG y, COLORREF color);
// ��Ŀ�������ڻ����ַ���
// format �� DT_ ��ͷ�ĺ�
// count ���ַ������������Ϊ -1 ���ʾ����ȫ���ַ�
BOOL GcchDrawStringEx(GcchBitmap *bitmap, LPCTSTR text, int count, LPRECT rect, COLORREF color, UINT format);
// ���������ַ�����Ҫ�Ĵ�С
// rect ���ڷ��ػ����ַ�����Ҫ�������С
// ��������Ŀ��
LONG GcchMeasureString(GcchBitmap *bitmap, LPCTSTR text, int count, LPRECT rect, UINT format);
// �� DIB ��ɫ����Ϊһ��
BOOL GcchHalfTone(GcchBitmap* bitmap, LPCRECT rect);

// �������ڿؼ������ڿؼ�������Ĵ��ڣ�����ָ���������ؼ�������
HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style,
	int width, int height,
	GcchControlFunc func,
	LPVOID data);
// ��ʾ����
void GcchShowWindow(HWND hWnd, int nCmdShow);
// ���ô��ڵı���
void GcchSetWindowTitle(HWND hWnd, LPCTSTR title);


