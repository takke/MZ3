#pragma once
/**
 * GUI �֘A���[�e�B���e�B
 */

/// ���[�e�B���e�B
namespace util
{
/**
 * gsgetfile.dll ��p���ăt�@�C���I����ʂ�\������
 *
 * �������� IDOK ���Ԃ�
 */
/*
inline int GetOpenFileNameEx(OPENFILENAME* pofn)
{
#ifdef WINCE
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		BOOL (*gsGetOpenFileName)(OPENFILENAME* pofn);
		(FARPROC&)gsGetOpenFileName = GetProcAddress(hInst, _T("gsGetOpenFileName"));
		if (gsGetOpenFileName) {
			BOOL ret = gsGetOpenFileName(pofn);
			FreeLibrary(hInst);
			return ret;
		}
		FreeLibrary(hInst);
	}
	return GetOpenFileName(pofn);
#else
	// for win32
	// TODO �������邱��
	return 0;
#endif
}
*/

/**
 * �w�肳�ꂽ�r�b�g�}�b�v hBitmap �� hdc �ɕ`�悷��
 */
inline bool DrawBitmap( HDC hdc, HBITMAP hBitmap, int x, int y, int w, int h, int tox, int toy )
{
	if( hBitmap == NULL ) {
		return false;
	}

	BITMAP	bmp;
	GetObject( hBitmap, sizeof(bmp), &bmp );

	HDC hdc1 = CreateCompatibleDC(NULL);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc1,hBitmap);

	// �]��
//	TRACE( L"DrawBitmap [%d,%d,%d,%d,%d,%d]\n", x, y, w, h, tox, toy );
	BitBlt( hdc, x, y, w, h, hdc1, tox, toy, SRCCOPY );

	SelectObject( hdc1, hBitmapOld );

	DeleteDC( hdc1 );
	return true;
}

/**
 * �w�肳�ꂽID�̃E�B���h�E���ړ�����
 */
inline void MoveDlgItemWindow( CWnd* pParent, int idc, int x, int y, int nWidth, int nHeight )
{
	if (pParent==NULL) {
		return;
	}

	CWnd* pDlgItem = pParent->GetDlgItem(idc);
	if (pDlgItem==NULL) {
		return;
	}

	pDlgItem->MoveWindow( x, y, nWidth, nHeight );
}

/**
 * ���X�g�R���g���[���̃t�H�[�J�X��ԂƑI����Ԃ�ݒ肷��B
 */
inline void MySetListCtrlItemFocusedAndSelected( CListCtrl& listCtrl, int idx, bool bFocusedAndSelected )
{
	UINT nState = 0;
	if( bFocusedAndSelected ) {
		nState |= LVIS_FOCUSED | LVIS_SELECTED;
	}

	listCtrl.SetItemState( idx, nState, LVIS_FOCUSED | LVIS_SELECTED );
}

/**
 * ���X�g�R���g���[�����Ŏw�肳�ꂽ�C���f�b�N�X���\����Ԃɂ��邩�ǂ�����Ԃ��B
 */
inline bool IsVisibleOnListBox( CListCtrl& listCtrl, int idx )
{
	int idxTop  = listCtrl.GetTopIndex();				// ��ʂ̈�ԏ�̕\�����ڂ̃C���f�b�N�X
	int idxLast = idxTop + listCtrl.GetCountPerPage();	// ��ʂ̈�ԉ��̕\�����ڂ̃C���f�b�N�X
	if( idx >= idxLast ) {
		// �����ɂ���
		return false;
	}
	if( idx <= idxTop ) {
		// �������A�擪���ڂ̏ꍇ�͏���
		if( idx==0 ) {
			return true;
		}

		// ����ɂ���
		return false;
	}
	return true;
}

/**
 * �t�H���_�I����ʂ�\������
 *
 * @param hWnd			[in] �e���
 * @param szTitle		[in] �t�H���_�I����ʂ̃^�C�g��
 * @param strFolderPath	[in/out] �����I���t�H���_���I���ς݃t�H���_�p�X
 * @return �������� true�A���s���� false ��Ԃ��B
 */
inline bool GetOpenFolderPath( HWND hWnd, LPCTSTR szTitle, CString& strFolderPath )
{
#ifdef WINCE
	// FDQ.DLL ��p���ăt�H���_�I����ʂ�\������

	// DLL���擾����iWindows�f�B���N�g�ɖ����Ƃ��̓t���p�X�Ŏw�肷��j
	HINSTANCE hInst = LoadLibrary (_T("FDQ.DLL"));
	if( hInst == NULL ) {
		MessageBox( NULL, L"FDQ.DLL not found", L"", MB_OK );
		return false;
	}
	int (pascal *FolderTree) (HWND,TCHAR*,TCHAR*);

	FolderTree = (int (pascal *) (HWND,TCHAR*,TCHAR*)) GetProcAddress (hInst, TEXT("FolderTree"));
	if (FolderTree == 0) {
		FreeLibrary (hInst);
		return false;	//�G���[
	}

	// �ۑ���ύX��ʂ̕\��
	TCHAR szFileName[256] = L"";
	wcsncpy( szFileName, strFolderPath, 255 );
	TCHAR szTitleBuf[256] = L"";
	wcsncpy( szTitleBuf, szTitle, 255 );
	int rc = FolderTree (hWnd, szTitleBuf, szFileName);
	FreeLibrary (hInst);

	if( rc == IDOK && util::ExistFile(szFileName) ) {
		strFolderPath = szFileName;
		return true;
	}else{
		return false;
	}
#else
    BROWSEINFO bInfo;
    LPITEMIDLIST pIDList;
    TCHAR szDisplayName[MAX_PATH];
    
    // BROWSEINFO�\���̂ɒl��ݒ�
    bInfo.hwndOwner             = hWnd;						// �_�C�A���O�̐e�E�C���h�E�̃n���h��
    bInfo.pidlRoot              = NULL;                     // ���[�g�t�H���_������ITEMIDLIST�̃|�C���^ (NULL�̏ꍇ�f�X�N�g�b�v�t�H���_���g���܂��j
    bInfo.pszDisplayName        = szDisplayName;            // �I�����ꂽ�t�H���_�����󂯎��o�b�t�@�̃|�C���^
    bInfo.lpszTitle             = szTitle;					// �c���[�r���[�̏㕔�ɕ\������镶���� 
    bInfo.ulFlags               = BIF_RETURNONLYFSDIRS;     // �\�������t�H���_�̎�ނ������t���O
    bInfo.lpfn                  = NULL;                     // BrowseCallbackProc�֐��̃|�C���^
    bInfo.lParam                = (LPARAM)0;                // �R�[���o�b�N�֐��ɓn���l

    // �t�H���_�I���_�C�A���O��\��
    pIDList = ::SHBrowseForFolder(&bInfo);
    if(pIDList == NULL){

        // �t�H���_���I�����ꂸ�Ƀ_�C�A���O������ꂽ
		return false;

    }else{

        // ItemIDList���p�X���ɕϊ����܂�
        if(!::SHGetPathFromIDList(pIDList, szDisplayName)){
            // �G���[����
			return false;
        }

        // szDisplayName�ɑI�����ꂽ�t�H���_�̃p�X�������Ă��܂�
		strFolderPath = szDisplayName;

        // �Ō��pIDList�̃|�C���g���Ă��郁�������J�����܂�
        ::CoTaskMemFree( pIDList );
    }
	return true;
#endif
}


}