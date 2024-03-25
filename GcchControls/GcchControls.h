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
// ����ʵ�ֿؼ����߼�
// ������λ��ư�ť
typedef LRESULT(*GcchControlFunc)(
	struct GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// �¼�����������
// ���ڴ���ؼ��������¼�
// ��������ťʱҪ��ʲô
typedef LRESULT(*GcchEventFunc)(
struct GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// �����¼�
LRESULT RaiseEvent(struct GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// ������Ϣ
typedef enum GcchMessage
{
	// ���ñ�����ɫ��wParm ��ʾ��ɫ
	WM_USER_SET_BACKGROUND = WM_USER + 1,
	// ����ǰ����ɫ��wParm ��ʾ��ɫ
	WM_USER_SET_FOREGROUND,
	// �������з�ʽ��wParam �� ˮƽ���з�ʽ��lParam �Ǵ�ֱ���з�ʽ
	WM_USER_SET_ALIGNMENT,
	// �������壬wParam �� GcchFont ָ��
	WM_USER_SET_FONT,
	// ���ÿؼ��Ĵ�С, wParam �ǿ�ȣ�lParam �Ǹ߶�
	WM_USER_SET_SIZE,

	// ��ť�������û�в���
	WM_USER_BUTTON_CLICK,
	// ��ťѡ��״̬�л���wParam �ǰ�ť�Ƿ�ѡ��
	WM_USER_CHECK_CHANGED,
	// ����ѡ��״̬��wParam ��ʾ�Ƿ�ѡ��
	WM_USER_SET_CHECK,
	// ��ȡѡ��״̬��û�в���
	WM_USER_GET_CHECK,
	// �л�ѡ��״̬��û�в���
	WM_USER_SWITCH_CHECK
}GcchMessage;

// ö�ٿؼ�����
typedef enum GcchControlType
{
	GCCH_CT_NONE,		// �������Զ���ؼ�
	GCCH_CT_WINDOW,		// ���ڿؼ�
	GCCH_CT_LABEL,		// ��ǩ�ؼ�
	GCCH_CT_BUTTON,		// ��ť�ؼ�
	GCCH_CT_CHECKBOX,		// ��ѡ��ؼ�
}GcchControlType;

// ˮƽ���з�ʽö��
typedef enum GcchHorizontalAlignment
{
	GCCH_HA_LEFT = DT_LEFT,
	GCCH_HA_CENTER = DT_CENTER,
	GCCH_HA_RIGHT = DT_RIGHT,
}GcchHorizontalAlignment;

// ��ֱ���з�ʽö��
typedef enum GcchVerticalAlignment
{
	GCCH_VA_TOP = DT_TOP,
	GCCH_VA_CENTER = DT_VCENTER,
	GCCH_VA_BOTTOM = DT_BOTTOM,
}GcchVerticalAlignment;

// �ؼ���һЩ״̬ö��
// ��������״̬�������ͣ״̬��
typedef enum GcchControlState
{
	GCCH_CS_NORMAL = 0,		// ����
	GCCH_CS_HOVER = 1,		// �����ͣ
	GCCH_CS_PRESSED = 2,	// ��갴��
	GCCH_CS_CHECKED = 2,	// ��ťѡ��
}GcchControlState;

// ���ھ��
// ��Ϣ������
// �������ݣ�������չ
// �ؼ�id�����ڿؼ�û��id
// �ؼ��ṹ��ռ�õ��ֽڴ�С
// �ؼ�����
// width �� height ����ƵĴ�С����һ�������մ�С
// ������Ǵ���0����ô���ǿؼ���ʵ�ʴ�С
// ����ؼ����Զ������С
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

// �ؼ�
typedef struct GcchControl
{
	GCCHCTRL
}GcchControl;

// ��ʼ���ؼ��Ļ����ֶ�
void GcchInitControl(GcchControl* control, GcchControlFunc func, GcchEventFunc eventHandler, LPVOID data,
	UINT id, size_t bytes, GcchControlType type);

// ��ȡ�ؼ�������
GcchControlType GcchGetControlType(HWND hWnd);

// �����ؼ�, �� CreateWindowEx �ķ�װ
HWND GcchCreateControl(DWORD exStyle, LPCTSTR text, DWORD style,
	int x, int y, int width, int height, HWND parent, HMENU menu, GcchControl* control);

// �ؼ���Ĭ����Ϣ������
LRESULT GcchDefControlFunc(
	GcchControl* control,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// ���ÿؼ��Ĵ�С
void GcchSetControlSize(HWND hWnd, int width, int height);

// �ػ洰�ڵ�ȫ��
BOOL GcchRedraw(HWND hWnd);
// �����ػ洰�ڵ�ȫ��
BOOL GcchRedrawNow(HWND hWnd);

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
// �ж����������Ƿ����
BOOL GcchRectEqual(LPCRECT rect1, LPCRECT rect2);


// ����
typedef struct GcchFont
{
	HFONT hFont;
	int width;
	int height;
	int weight;
	TCHAR familyName[32];
}GcchFont;

// ��������
GcchFont* GcchCreateFont(int width, int height, int weight, LPCTSTR familyName);
// ��������
void GcchDestroyFont(GcchFont** pFont);

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
// ������ļ����أ�flags ��Ҫ����Ϊ LR_LOADFROMFILE
GcchBitmap* GcchLoadBitmap(LPCTSTR filename, UINT flags);
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
// ��������, ������UnsetFont���ʹ��
BOOL GcchSetFont(GcchBitmap* bitmap, GcchFont* font);
// ȡ����������
BOOL GcchUnsetFont(GcchBitmap* bitmap);


// ���ڿؼ���
typedef struct GcchWindow
{
	GCCHCTRL
}GcchWindow;

// �������ڿؼ������ڿؼ�������Ĵ��ڣ�����ָ���������ؼ�������
HWND GcchCreateWindow(DWORD exStyle, LPCTSTR text, DWORD style,
	int width, int height,
	GcchControlFunc func, GcchEventFunc eventHandler,
	LPVOID data);
// ��ʾ����
void GcchShowWindow(HWND hWnd, int nCmdShow);
// ���ô��ڵı���
void GcchSetWindowTitle(HWND hWnd, LPCTSTR title);

// ��ǩ
// ��֧�ֵ����ı�
typedef struct GcchLabel
{
	GCCHCTRL
	COLORREF foreground;		// �ı���ɫ
	COLORREF background;		// ������ɫ
	GcchHorizontalAlignment horizontalAlignment;		// ˮƽ���з�ʽ
	GcchVerticalAlignment verticalAlignment;		// ��ֱ���з�ʽ
	// ����
	GcchFont* font;
}GcchLabel;

// ��(x, y, width, height) ָ�������򴴽���ǩ
// ��� width �� height Ϊ 0�����Զ�����
HWND GcchCreateLabel(HWND hWndParent, UINT id, int x, int y, int width, int height, LPCTSTR text);
// ��(x, y, width, height) ָ�������򴴽���ǩ
// ��� width �� height Ϊ 0�����Զ�����
HWND GcchCreateLabelEx(HWND hWndParent, UINT id, int x, int y, int width, int height, LPCTSTR text,
	GcchHorizontalAlignment horizontalAlignment, GcchVerticalAlignment verticalAlignment);
// ���ñ�ǩ���ı�
BOOL GcchSetLabelText(HWND hWnd, LPCTSTR text);
// ���ñ�ǩ�ı�����ɫ
void GcchSetLabelBackground(HWND hWnd, COLORREF color);
// ���ñ�ǩ���ı���ɫ
void GcchSetLabelForeground(HWND hWnd, COLORREF color);
// �����ı������з�ʽ
void GcchSetLabelAlignment(HWND hWnd, GcchHorizontalAlignment horizontalAlignment, GcchVerticalAlignment verticalAlignment);
// ���ñ�ǩ������
void GcchSetLabelFont(HWND hWnd, GcchFont* font);

typedef struct GcchButtonBase
{
	GCCHCTRL
}GcchButtonBase;

// ��ť�Ĺ�ͬ�����װ
LRESULT GcchDefButtonFunc(
	GcchButtonBase* button,
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

// ��ť
typedef struct GcchButton
{
	GCCHCTRL
}GcchButton;

// ����һ����ť
HWND GcchCreateButton(LPCTSTR text, int x, int y, int width, int height,
	HWND hWndParent, UINT id, GcchEventFunc eventHandler, LPVOID data);

// ��ѡ��
typedef struct GcchCheckBox
{
	GCCHCTRL
}GcchCheckBox;

// ����һ����ѡ��
HWND GcchCreateCheckBox(LPCTSTR text, int x, int y, 
	HWND hWndParent, UINT id, GcchEventFunc eventHandler, LPVOID data);
void GcchSetCheck(HWND hWnd, BOOL isCheck);
void GcchSwitchCheck(HWND hWnd);
BOOL GcchGetCheck(HWND hWnd);
