//�o�[�W�����擾 (100 = Ver1.00)
int APIENTRY GetCDQVer (
	int type);					//(i):��� 0=ALL 1=File&Folder 2=FolderTree 3=Color 4=Font

// �J���[�_�C�A���O
COLORREF APIENTRY ColorDialog (	//�߂�l�F�I��F
	HWND hWnd,					//(i):�e�E�C���h�E�E�n���h��
	COLORREF col);				//(i):�����F

COLORREF APIENTRY ColorDialog2 (//�߂�l�F�I��F
	HWND hWnd,					//(i):�e�E�C���h�E�E�n���h��
	COLORREF col);				//(i):�����F

COLORREF APIENTRY ColorDialogT (//�߂�l�F�I��F
	HWND hWnd,					//(i):�e�E�C���h�E�E�n���h��
	HBITMAP hBitmap,			//(i):�p���b�g�J���[�C���[�W
	COLORREF col);				//(i):�����F

// �t�@�C���I���_�C�A���O
int APIENTRY FileDialog (		//�߂�l�FIDOK/IDCANCEL
	HWND hWnd, 					//(i):�e�E�C���h�E�E�n���h��
	TCHAR* szTitle,				//(i):�^�C�g����
	TCHAR* szFileName,			//(i/o):�t�@�C����
	int    fLive);				//(i):���݂���t�@�C���̂�=1

// �t�H���_�I���_�C�A���O
int APIENTRY FolderDialog (		//�߂�l�FIDOK/IDCANCEL
	HWND hWnd, 					//(i):�e�E�C���h�E�E�n���h��
	TCHAR* szTitle, 			//(i):�^�C�g����
	TCHAR* szFileName,			//(i/o):�t�H���_��
	int    fLive);				//(i):���݂���t�@�C���̂�=1

// �t�H���_�I���c���[
int APIENTRY FolderTree (		//�߂�l�FIDOK/IDCANCEL
	HWND hWnd,					//(i):�e�E�C���h�E�E�n���h��
	LPCTSTR szTitle,			//(i):�^�C�g����
	LPTSTR szPath);				//(i/o):�t�H���_��

// �t�H���g�_�C�A���O
int APIENTRY FontDialog (		//�߂�l�FIDOK/IDCANCEL
	HWND hWnd, 					//(i):�e�E�C���h�E�E�n���h��
	TCHAR* FontName,			//(i):�t�H���g��
	int FontSize,				//(i):�t�H���g�T�C�Y
	LOGFONT* plf);				//(o):�t�H���g���
