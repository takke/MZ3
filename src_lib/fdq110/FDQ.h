//�o�[�W�����擾 (100 = Ver1.00)
int APIENTRY GetCDQVer (
	int type);					//(i):��� 0=ALL 1=File&Folder 2=FolderTree 3=Color 4=Font

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

// �t�H���_�c���[�I���_�C�A���O
int APIENTRY FolderTree (		//�߂�l�FIDOK/IDCANCEL
	HWND hWnd, 					//(i):�e�E�C���h�E�E�n���h��
	LPCTSTR szTitle,			//(i):�^�C�g����
	LPTSTR szPath); 			//(i/o):�t�H���_��

