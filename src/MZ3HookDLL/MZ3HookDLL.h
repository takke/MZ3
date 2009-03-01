// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための 
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された MZ3HOOKDLL_EXPORTS
// シンボルでコンパイルされます。このシンボルは、この DLL を使うプロジェクトで定義することはできません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、 
// MZ3HOOKDLL_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef MZ3HOOKDLL_EXPORTS
#define MZ3HOOKDLL_API __declspec(dllexport)
#else
#define MZ3HOOKDLL_API __declspec(dllimport)
#endif

MZ3HOOKDLL_API int MZ3Hook_StartHook(void);
MZ3HOOKDLL_API int MZ3Hook_SetMainWindow(HWND);
MZ3HOOKDLL_API int MZ3Hook_StopHook(void);
LRESULT CALLBACK MZ3Hook_HookProc(int, WPARAM, LPARAM);
