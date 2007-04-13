//バージョン取得 (100 = Ver1.00)
int APIENTRY GetCDQVer (
	int type);					//(i):種類 0=ALL 1=File&Folder 2=FolderTree 3=Color 4=Font

// ファイル選択ダイアログ
int APIENTRY FileDialog (		//戻り値：IDOK/IDCANCEL
	HWND hWnd, 					//(i):親ウインドウ・ハンドル
	TCHAR* szTitle,				//(i):タイトル名
	TCHAR* szFileName,			//(i/o):ファイル名
	int    fLive);				//(i):存在するファイルのみ=1

// フォルダ選択ダイアログ
int APIENTRY FolderDialog (		//戻り値：IDOK/IDCANCEL
	HWND hWnd, 					//(i):親ウインドウ・ハンドル
	TCHAR* szTitle, 			//(i):タイトル名
	TCHAR* szFileName,			//(i/o):フォルダ名
	int    fLive);				//(i):存在するファイルのみ=1

// フォルダツリー選択ダイアログ
int APIENTRY FolderTree (		//戻り値：IDOK/IDCANCEL
	HWND hWnd, 					//(i):親ウインドウ・ハンドル
	LPCTSTR szTitle,			//(i):タイトル名
	LPTSTR szPath); 			//(i/o):フォルダ名

