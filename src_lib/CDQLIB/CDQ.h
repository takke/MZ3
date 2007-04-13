//バージョン取得 (100 = Ver1.00)
int APIENTRY GetCDQVer (
	int type);					//(i):種類 0=ALL 1=File&Folder 2=FolderTree 3=Color 4=Font

// カラーダイアログ
COLORREF APIENTRY ColorDialog (	//戻り値：選択色
	HWND hWnd,					//(i):親ウインドウ・ハンドル
	COLORREF col);				//(i):初期色

COLORREF APIENTRY ColorDialog2 (//戻り値：選択色
	HWND hWnd,					//(i):親ウインドウ・ハンドル
	COLORREF col);				//(i):初期色

COLORREF APIENTRY ColorDialogT (//戻り値：選択色
	HWND hWnd,					//(i):親ウインドウ・ハンドル
	HBITMAP hBitmap,			//(i):パレットカラーイメージ
	COLORREF col);				//(i):初期色

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

// フォルダ選択ツリー
int APIENTRY FolderTree (		//戻り値：IDOK/IDCANCEL
	HWND hWnd,					//(i):親ウインドウ・ハンドル
	LPCTSTR szTitle,			//(i):タイトル名
	LPTSTR szPath);				//(i/o):フォルダ名

// フォントダイアログ
int APIENTRY FontDialog (		//戻り値：IDOK/IDCANCEL
	HWND hWnd, 					//(i):親ウインドウ・ハンドル
	TCHAR* FontName,			//(i):フォント名
	int FontSize,				//(i):フォントサイズ
	LOGFONT* plf);				//(o):フォント情報
