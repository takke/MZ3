/**
 * MZ3��ˑ����[�e�B���e�B
 */
#pragma once

/// MZ3 �p���[�e�B���e�B
namespace util
{
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
 * �t�@�C�����J��
 */
inline bool OpenByShellExecute( LPCTSTR target )
{
	// �t�@�C�����J��
	SHELLEXECUTEINFO sei;
	sei.cbSize       = sizeof(sei);
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd         = 0;
	sei.lpVerb       = _T("open");
	sei.lpFile       = target;
	sei.lpParameters = NULL;
	sei.lpDirectory  = NULL;
	sei.nShow        = SW_NORMAL;
	return ShellExecuteEx( &sei ) == TRUE;
}

/**
 * �t�@�C���̑��݃`�F�b�N
 *
 * �t�@�C�������݂���� true�A���݂��Ȃ���� false ��Ԃ�
 */
inline bool ExistFile( LPCTSTR szFilepath )
{
	CFileStatus rStatus;

	if( CFile::GetStatus(szFilepath, rStatus) != FALSE ) {
		return true;
	}else{
		return false;
	}
}

/**
 * �t�@�C�������݂���΍폜����
 */
inline bool RemoveWhenExist( LPCTSTR szFilepath )
{
	if( util::ExistFile( szFilepath ) ) {
		CFile::Remove( szFilepath );
	}else{
		return false;
	}
	return true;
}

/**
 * �w�肳�ꂽ URL ������̃u���E�U�ŊJ��
 */
inline void OpenUrlByBrowser( LPCTSTR url )
{
	SHELLEXECUTEINFO sei;

	sei.cbSize       = sizeof(sei);
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd         = 0;
	sei.lpVerb       = _T("open");
	sei.lpFile       = url;
	sei.lpParameters = NULL;
	sei.lpDirectory  = NULL;
	sei.nShow        = SW_NORMAL;

	ShellExecuteEx(&sei);
}

/// �A�N�Z�X��ʂ𕶎���ɕϊ�����
inline LPCTSTR AccessType2Message( ACCESS_TYPE type )
{
	LPCTSTR text = L"";
	switch( type ) {
	case ACCESS_LOGIN:				text = L"���O�C��";				break;
	case ACCESS_MAIN:				text = L"���C��";				break;
	case ACCESS_DIARY:				text = L"���L���e";				break;
	case ACCESS_NEWS:				text = L"�j���[�X";				break;
	case ACCESS_BBS:				text = L"�R�~������";			break;
	case ACCESS_ENQUETE:			text = L"�A���P�[�g";			break;
	case ACCESS_EVENT:				text = L"�C�x���g";				break;
	case ACCESS_MYDIARY:			text = L"���L";					break;
	case ACCESS_ADDDIARY:			text = L"���L���e";				break;
	case ACCESS_MESSAGE:			text = L"���b�Z�[�W";			break;
	case ACCESS_IMAGE:				text = L"�摜";					break;
	case ACCESS_MOVIE:				text = L"����";					break;
	case ACCESS_DOWNLOAD:			text = L"�_�E�����[�h";			break;
	case ACCESS_PROFILE:			text = L"�v���t�B�[��";			break;
	case ACCESS_COMMUNITY:			text = L"�R�~���j�e�B";			break;
	case ACCESS_LIST_INTRO:			text = L"�Љ";				break;

	case ACCESS_LIST_DIARY:			text = L"���L�ꗗ";				break;
	case ACCESS_LIST_NEW_COMMENT:	text = L"�V���R�����g�ꗗ";		break;
	case ACCESS_LIST_COMMENT:		text = L"�R�����g�ꗗ";			break;
	case ACCESS_LIST_NEWS:			text = L"�j���[�X�ꗗ";			break;
	case ACCESS_LIST_FAVORITE:		text = L"���C�ɓ���";			break;
	case ACCESS_LIST_FRIEND:		text = L"�}�C�~�N�ꗗ";			break;
	case ACCESS_LIST_COMMUNITY:		text = L"�R�~���j�e�B�ꗗ";		break;
	case ACCESS_LIST_NEW_BBS_COMMENT:text = L"�R�~���R�����g�L������";		break;
	case ACCESS_LIST_NEW_BBS:		text = L"�R�~�������ꗗ";		break;
	case ACCESS_LIST_BBS:			text = L"�g�s�b�N�ꗗ";			break;
	case ACCESS_LIST_MYDIARY:		text = L"���L�ꗗ";				break;
	case ACCESS_LIST_FOOTSTEP:		text = L"������";				break;
	case ACCESS_LIST_MESSAGE_IN:	text = L"���b�Z�[�W(��M��)";	break;
	case ACCESS_LIST_MESSAGE_OUT:	text = L"���b�Z�[�W(���M��)";	break;
	case ACCESS_LIST_BOOKMARK:		text = L"�u�b�N�}�[�N";			break;
	case ACCESS_LIST_CALENDAR:		text = L"�J�����_�[";			break;  //icchu�ǉ�

	case ACCESS_GROUP_COMMUNITY:	text = L"�R�~���j�e�BG";		break;
	case ACCESS_GROUP_MESSAGE:		text = L"���b�Z�[�WG";			break;
	case ACCESS_GROUP_MYDIARY:		text = L"���LG";				break;
	case ACCESS_GROUP_NEWS:			text = L"�j���[�XG";			break;
	case ACCESS_GROUP_OTHERS:		text = L"���̑�G";				break;

	// POST �n
	case ACCESS_POST_CONFIRM_COMMENT:		text = L"�R�����g���e�i�m�F�j";		break;
	case ACCESS_POST_ENTRY_COMMENT:			text = L"�R�����g���e�i�����j";		break;
	case ACCESS_POST_CONFIRM_REPLYMESSAGE:	text = L"���b�Z�[�W�ԐM�i�m�F�j";	break;
	case ACCESS_POST_ENTRY_REPLYMESSAGE:	text = L"���b�Z�[�W�ԐM�i�����j";	break;
	case ACCESS_POST_CONFIRM_NEWMESSAGE:	text = L"�V�K���b�Z�[�W�i�m�F�j";	break;
	case ACCESS_POST_ENTRY_NEWMESSAGE:		text = L"�V�K���b�Z�[�W�i�����j";	break;
	case ACCESS_POST_CONFIRM_NEWDIARY:		text = L"���L���e�i�m�F�j";			break;
	case ACCESS_POST_ENTRY_NEWDIARY:		text = L"�R�����g���e�i�����j";		break;

	case ACCESS_INVALID:			text = L"<invalid>";			break;
	default:						text = L"<unknown>";			break;
	}
	return text;
}

/// int �^���l�𕶎���ɕϊ�����
inline CStringW int2str( int n )
{
	CString s;
	s.Format( L"%d", n );
	return s;
}

/// int �^���l�𕶎���ɕϊ�����
inline CStringA int2str_a( int n )
{
	CStringA s;
	s.Format( "%d", n );
	return s;
}

/**
 * ������ szLeft �ƕ����� szRight �ň͂܂ꂽ������������擾���AszRight �̎��̕����̃C���f�b�N�X��Ԃ��B
 *
 * @return szRight �̎��̕����̈ʒu��Ԃ��B�����񂪌�����Ȃ��Ƃ��� -1 ��Ԃ��B
 */
inline int GetBetweenSubString( const CString& str, LPCTSTR szLeft, LPCTSTR szRight, CString& result )
{
	int pos1 = str.Find( szLeft );
	if( pos1 == -1 )
		return -1;

	// ����������̊J�n�ʒu=szLeft�J�n�ʒu+szLeft�̒���
	int start = pos1+wcslen(szLeft);

	int pos2 = str.Find( szRight, start );
	if( pos2 == -1 ) 
		return -1;

	// ����������̒���
	int len = pos2-start;
	if( len <= 0 ) {
		return -1;
	}

	// ����������̎擾
	result = str.Mid( start, len );

	// szRight �̎��̕����̈ʒu��Ԃ�
	return pos2+wcslen(szRight);
}

/**
 * ������ szKey�i�̎��̕����j�ȍ~�̕�����������擾���AszKey �̃C���f�b�N�X��Ԃ��B
 *
 * @return szKey �̈ʒu��Ԃ��B�����񂪌�����Ȃ��Ƃ��� -1 ��Ԃ��B
 */
inline int GetAfterSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos1 = str.Find( szKey );
	if( pos1 == -1 )
		return -1;

	// ����������̊J�n�ʒu = szKey�J�n�ʒu + szKey�̒���
	int start = pos1+wcslen(szKey);

	// ����������̎擾
	result = str.Mid( start );

	// szKey �̈ʒu��Ԃ�
	return pos1;
}

/**
 * ������ szKey ���O�̕�����������擾���AszKey �̃C���f�b�N�X��Ԃ��B
 *
 * @return szKey �̈ʒu��Ԃ��B�����񂪌�����Ȃ��Ƃ��� -1 ��Ԃ��B
 */
inline int GetBeforeSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos = str.Find( szKey );
	if( pos == -1 )
		return -1;

	// ����������̎擾
	result = str.Left( pos );

	// szKey �̈ʒu��Ԃ�
	return pos;
}

/**
 * ���l��3����؂蕶����ɕϊ�����
 */
inline CString int2comma_str(int n)
{
	if( n == 0 ) {
		return L"0";
	}

	CString s;

	int figure = 0;		// �����i1���Ȃ�0�j

	while( n > 0 ) {
		if( figure>0 && (figure % 3) == 0 ) {
			s = util::int2str( n % 10 ) + L"," + s;
		}else{
			s = util::int2str( n % 10 ) + s;
		}
		figure ++;
		n = n / 10;
	}

	return s;
}

}