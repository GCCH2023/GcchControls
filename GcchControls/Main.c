#include "GcchControls.h"
#include <tchar.h>


LRESULT GcchWindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID data)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	HWND hWnd;
	MSG msg = { 0 };

	if (GcchInitialize(hInstance) == FALSE)
		return 0;

	hWnd = GcchCreateControl(0, _T("Window"), WS_OVERLAPPEDWINDOW,
		300, 100, 800, 600, NULL, NULL, GcchWindowFunc, NULL);

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	GcchUninitialize();
	return 0;
}