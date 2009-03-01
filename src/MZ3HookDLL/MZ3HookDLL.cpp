// MZ3HookDLL.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "MZ3HookDLL.h"
#include "stdio.h"

#pragma data_seg("MY_DATA")
HHOOK g_hMyHook = NULL;
#pragma data_seg()

HINSTANCE g_hInst = NULL;
HWND g_hMainWindow = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
//	case DLL_THREAD_ATTACH:
	case DLL_PROCESS_ATTACH:
		g_hInst = hModule;
		break;

//	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		MZ3Hook_StopHook();
		break;
	}
    return TRUE;
}

/**
 * フック開始
 */
MZ3HOOKDLL_API int MZ3Hook_StartHook(void)
{
//	MessageBox(NULL, L"MZ3Hook_StartHook", L"dll", MB_OK);
	
	g_hMyHook = SetWindowsHookEx(WH_KEYBOARD, MZ3Hook_HookProc, g_hInst, 0);
	if (g_hMyHook==NULL) {
		MessageBox(NULL, L"フック失敗", L"dll", MB_OK);
	} else {
//		MessageBox(NULL, L"フック成功", L"dll", MB_OK);
	}

	return 0;
}

/**
 * フック停止
 */
MZ3HOOKDLL_API int MZ3Hook_StopHook(void)
{
//	MessageBox(NULL, L"MZ3Hook_StopHook", L"dll", MB_OK);

	if (g_hMyHook!=NULL) {
		if (UnhookWindowsHookEx(g_hMyHook) != 0) {
//			MessageBox(NULL, L"フック解除成功", L"dll", MB_OK);
//		} else {
//			MessageBox(NULL, L"フック解除失敗", L"dll", MB_OK);
		}
		g_hMyHook = NULL;
	}

	return 0;
}

/**
 * 
 */
MZ3HOOKDLL_API int MZ3Hook_SetMainWindow(HWND hWnd)
{
	g_hMainWindow = hWnd;

	return 0;
}

/**
 * フック関数
 */
LRESULT CALLBACK MZ3Hook_HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	wprintf(L"MZ3Hook_HookProc (0x%08X, 0x%08X)\n", wParam, lParam);
	if (nCode < 0) {
		return CallNextHookEx(g_hMyHook, nCode, wParam, lParam);
	}
/*
	if (wParam == 'F' && (lParam & 0x80000000)) {
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
			// Ctrl+Alt+F
//			SetWindowPos(g_hMainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//			SetForegroundWindow(g_hMainWindow);
//			SetFocus(g_hMainWindow);
//			SetActiveWindow(g_hMainWindow);

			DWORD nullProcessId = 0;

			// ターゲットとなるハンドルのスレッドIDを取得.
            DWORD targetThreadId        = GetWindowThreadProcessId(g_hMainWindow, &nullProcessId);

			// 現在アクティブとなっているウィンドウのスレッドIDを取得
			DWORD currentActiveThreadId = GetWindowThreadProcessId(GetForegroundWindow(), &nullProcessId);


			if (targetThreadId == currentActiveThreadId) {
				// 現在アクティブなのが自分の場合は前面に持ってくる。
				SetForegroundWindow(g_hMainWindow);
				BringWindowToTop(g_hMainWindow);
			} else {
				// 別のプロセスがアクティブな場合は、そのプロセスにアタッチし、入力を奪う

				if (AttachThreadInput(targetThreadId, currentActiveThreadId, TRUE)) {

//					SetActiveWindow(g_hMainWindow);
//					SetForegroundWindow(g_hMainWindow);

					DWORD backup = 0UL;

					SetActiveWindow(g_hMainWindow);
					SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &backup, 0);
					SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, 0);
					SetActiveWindow(g_hMainWindow);
					SetForegroundWindow(g_hMainWindow);
					SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)backup, 0);

					// アタッチ解除
					AttachThreadInput(targetThreadId, currentActiveThreadId, FALSE);
				}
			}

			ShowWindow(g_hMainWindow, SW_SHOW);
			SetFocus(g_hMainWindow);

			wprintf(L"[MZ3Hook] Hook : %d\n", g_hMainWindow);
//			MessageBox(NULL, L"Hook", L"dll", MB_OK);
			return TRUE;
		}
	}
*/
	return CallNextHookEx(g_hMyHook, nCode, wParam, lParam);


/*
	TCHAR str[256];
	if ('0' <= wParam && wParam <= '9') {
		wsprintf(str, L"hMyHook = %d", g_hMyHook);
		MessageBox(NULL, str, L"MyHookProc", MB_OK);
		
	}
*/
//	wprintf(L"[MZ3Hook] キー入力はインターセプトされました\n　フックハンドル=　%d", g_hMyHook); 
//	wsprintf(str, L"キー入力はインターセプトされました\n　フックハンドル=　%d", g_hMyHook); 
//	MessageBox(NULL, str, L"インターセプト", MB_OK);
//	return TRUE;
}
