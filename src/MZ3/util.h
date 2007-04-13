/**
 * ���̑��̃��[�e�B���e�B�֐�
 */
#pragma once

/// MZ3 �p���[�e�B���e�B
namespace util
{

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

/**
 * �w�肳�ꂽURL���u���E�U�ŊJ���i�m�F��ʕt���j
 */
inline void OpenBrowserForUrl( LPCTSTR url )
{
	// �m�F���
	CString msg;
	msg.Format( 
		L"���L��URL���u���E�U�ŊJ���܂��B\n\n"
		L"%s\n\n"
		L"��낵���ł����H", url );
	if( MessageBox( NULL, msg, L"MZ3", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES �{�^���ȊO�Ȃ̂ŏI��
		return;
	}

	// �u���E�U�ŊJ��
	util::OpenUrlByBrowser( url );
}

/**
 * gsgetfile.dll ��p���ăt�@�C���I����ʂ�\������
 *
 * �������� IDOK ���Ԃ�
 */
inline int GetOpenFileNameEx(OPENFILENAME* pofn)
{
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
}

/**
 * FDQ.DLL ��p���ăt�H���_�I����ʂ�\������
 *
 * @param hWnd			[in] �e���
 * @param szTitle		[in] �t�H���_�I����ʂ̃^�C�g��
 * @param strFolderPath	[in/out] �����I���t�H���_���I���ς݃t�H���_�p�X
 * @return �������� true�A���s���� false ��Ԃ��B
 */
inline bool GetOpenFolderPath( HWND hWnd, LPCTSTR szTitle, CString& szFolderPath )
{
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
	wcsncpy( szFileName, theApp.m_filepath.logFolder, 255 );
	int rc = FolderTree (hWnd, _T("���O�t�H���_�̕ύX"), szFileName);
	FreeLibrary (hInst);

	if( rc == IDOK && util::ExistFile(szFileName) ) {
		szFolderPath = szFileName;
		return true;
	}else{
		return false;
	}
}

/**
 * �w�肳�ꂽ���[�U�̃v���t�B�[���y�[�W���u���E�U�ŊJ���i�m�F��ʕt���j
 */
inline void OpenBrowserForUser( LPCTSTR url, LPCTSTR szUserName )
{
	// �m�F���
	CString msg;
	msg.Format( 
		L"%s ����̃v���t�B�[���y�[�W���u���E�U�ŊJ���܂��B\n\n"
		L"%s\n\n"
		L"��낵���ł����H", szUserName, url );
	if( MessageBox( NULL, msg, L"MZ3", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES �{�^���ȊO�Ȃ̂ŏI��
		return;
	}

	// �u���E�U�ŊJ��
	util::OpenUrlByBrowser( url );
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
	case ACCESS_LIST_NEW_BBS:		text = L"�R�~�������ꗗ";		break;
	case ACCESS_LIST_BBS:			text = L"�g�s�b�N�ꗗ";			break;
	case ACCESS_LIST_MYDIARY:		text = L"���L�ꗗ";				break;
	case ACCESS_LIST_FOOTSTEP:		text = L"������";				break;
	case ACCESS_LIST_MESSAGE_IN:	text = L"���b�Z�[�W(��M��)";	break;
	case ACCESS_LIST_MESSAGE_OUT:	text = L"���b�Z�[�W(���M��)";	break;
	case ACCESS_LIST_BOOKMARK:		text = L"�u�b�N�}�[�N";			break;

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

/**
 * ��ʉ����̏��̈�Ƀ��b�Z�[�W��ݒ肷��
 */
inline void MySetInformationText( HWND hWnd, LPCTSTR szMessage )
{
	CString text;
	if( theApp.m_inet.IsConnecting() ) {
		// �ʐM���Ȃ̂ŁA�A�N�Z�X��ʂ𓪂ɕt����
		text = AccessType2Message(theApp.m_accessType);

		// ���_�C���N�g�񐔂��P��ȏ�Ȃ�A������\�����Ă���
		int nRedirect = theApp.m_inet.GetRedirectCount();
		if( nRedirect > 0 ) {
			CString s;
			s.Format( L"[%d]", nRedirect );
			text += s;
		}

		text += L"�F";
		text += szMessage;
	}else{
		text = szMessage;
	}
	::SendMessage( hWnd, WM_MZ3_ACCESS_INFORMATION, NULL, (LPARAM)&text );
}

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
 * �X�g�b�v�E�H�b�`
 *
 * �������Ԍv���p
 */
class StopWatch
{
	DWORD dwStart;		///< �v���J�n����
	DWORD dwStop;		///< �v����~����
public:
	// �R���X�g���N�^
	StopWatch() {
		start();
	}

	/// ��~�A�o�ߎ��Ԃ��擾����
	DWORD stop() {
		dwStop = ::GetTickCount();
		return getElapsedMilliSecUntilStoped();
	}

	/// �v���J�n
	void start() {
		dwStop = dwStart = ::GetTickCount();
	}

	/// ��~���܂ł̌o�ߎ��Ԃ� msec �P�ʂŎ擾����
	DWORD getElapsedMilliSecUntilStoped()
	{
		return dwStop - dwStart;
	}

	/// ���݂̌o�ߎ��Ԃ� msec �P�ʂŎ擾����
	DWORD getElapsedMilliSecUntilNow()
	{
		return ::GetTickCount() - dwStart;
	}
};

/**
 * CMixiData �ɑΉ����郍�O�t�@�C���̃p�X�𐶐�����
 */
inline CString MakeLogfilePath( const CMixiData& data )
{
	// �A�N�Z�X��ʂɉ����ăp�X�𐶐�����
	switch( data.GetAccessType() ) {

	// ���O�C���y�[�W
	case ACCESS_LOGIN:				return theApp.m_filepath.logFolder + L"\\login.html";
	
	// ���X�g�n
	case ACCESS_LIST_DIARY:			return theApp.m_filepath.logFolder + L"\\new_friend_diary.html";
	case ACCESS_LIST_NEW_COMMENT:	return theApp.m_filepath.logFolder + L"\\new_comment.html";
	case ACCESS_LIST_COMMENT:		return theApp.m_filepath.logFolder + L"\\list_comment.html";
	case ACCESS_LIST_NEW_BBS:
		{
			// �y�[�W�ԍ����ɕ���
			CString page = L"1";
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\new_bbs_%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	case ACCESS_LIST_MESSAGE_IN:	return theApp.m_filepath.logFolder + L"\\list_message_in.html";
	case ACCESS_LIST_MESSAGE_OUT:	return theApp.m_filepath.logFolder + L"\\list_message_out.html";
	case ACCESS_LIST_FOOTSTEP:		return theApp.m_filepath.logFolder + L"\\show_log.html";
	case ACCESS_LIST_MYDIARY:		return theApp.m_filepath.logFolder + L"\\list_diary.html";
	case ACCESS_LIST_NEWS:
		{
			// �J�e�S���A�y�[�W�ԍ����ɕ���
			// url : http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
			// url : http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1
			// url : http://news.mixi.jp/list_news_category.pl?page=2&id=pickup&type=bn
			// url : http://news.mixi.jp/list_news_category.pl?page=2&sort=1&id=1&type=bn

			CString url = data.GetURL();
			url += L"&";

			// �J�e�S���A�y�[�W�ԍ����o
			CString category;
			util::GetBetweenSubString( url, L"id=", L"&", category );
			CString page;
			util::GetBetweenSubString( url, L"page=", L"&", page );

			CString path;
			path.Format( L"%s\\list_news_category_%s_%s.html", theApp.m_filepath.logFolder, category, page );
			return path;
		}

	case ACCESS_LIST_BBS:
		{
			// url : list_bbs.pl?id=xxx
			CString id;
			util::GetAfterSubString( data.GetURL(), L"id=", id );

			CString path;
			path.Format( L"%s\\list_bbs_%s.html", theApp.m_filepath.logFolder, id );
			return path;
		}

	case ACCESS_LIST_FAVORITE:
		{
			// url(1) : http://mixi.jp/list_bookmark.pl
			// url(2) : http://mixi.jp/list_bookmark.pl?page=2&kind=friend
			// url(3) : http://mixi.jp/list_bookmark.pl?kind=community
			// url(4) : http://mixi.jp/list_bookmark.pl?page=2&kind=community

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"?", page );

			page.Replace( L"&", L"_" );
			page.Replace( L"=", L"_" );
			if( !page.IsEmpty() ) {
				page.Insert( 0, '_' );
			}

			CString path;
			path.Format( L"%s\\list_bookmark%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	case ACCESS_LIST_FRIEND:
		{
			// url(1) : http://mixi.jp/list_friend.pl
			// url(2) : http://mixi.jp/list_friend.pl?page=2

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\list_friend%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_COMMUNITY:
		{
			// url(1) : http://mixi.jp/list_community.pl?id=xxx
			// url(2) : http://mixi.jp/list_community.pl?id=xxx&page=2

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\list_community%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_INTRO:
		{
			// url(1) : http://mixi.jp/show_intro.pl
			// url(2) : http://mixi.jp/show_intro.pl?page=2

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\show_intro%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	// �A�C�e���n
	case ACCESS_MYDIARY:
	case ACCESS_DIARY:
		{
			CString owner_id, id;
			// url(1) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy
			// url(2) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy&comment_count=8
			CString url = data.GetURL() + L"&";	// url(1) �̏ꍇ�ɑΏ����邽�߁A���� & ��t���Ă���
			util::GetBetweenSubString( url, L"?id=", L"&", id );
			util::GetBetweenSubString( url, L"owner_id=", L"&", owner_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.diaryFolder, owner_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_BBS:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_bbs.pl?id=9360823&comm_id=1198460
			// url(2) : http://mixi.jp/view_bbs.pl?page=4&comm_id=1198460&id=9360823
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=9360823&comm_id=1198460&
			// url(2) : &page=4&comm_id=1198460&id=9360823&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.bbsFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_ENQUETE:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_enquete.pl?id=13632077&comment_count=1&comm_id=1198460
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=13632077&comment_count=1&comm_id=1198460&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.enqueteFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_EVENT:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=xxx&comment_count=20&comm_id=xxx&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.eventFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_MESSAGE:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// url(2) : http://mixi.jp/view_message.pl?id=xxx&box=outbox
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=xxx&box=inbox&
			// url(2) : &id=xxx&box=outbox&
			util::GetBetweenSubString( url, L"&id=", L"&", id );

			// url �� inbox ������� inbox �f�B���N�g���ɁB
			CString path;
			if( url.Find( L"box=inbox" ) != -1 ) {
				path.Format( L"%s\\inbox", theApp.m_filepath.messageFolder );
			}else{
				path.Format( L"%s\\outbox", theApp.m_filepath.messageFolder );
			}
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_NEWS:
		{
			CString media_id, id;
			// url(1) : http://news.mixi.jp/view_news.pl?id=132704&media_id=4
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=132704&media_id=4&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"&media_id=", L"&", media_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.newsFolder, media_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	default:
		// ��L�ȊO�Ȃ�A�t�@�C�����Ȃ�
		return L"";
	}
}

/**
 * URL ����A�N�Z�X��ʂ𐄒肷��
 */
inline ACCESS_TYPE EstimateAccessTypeByUrl( const CString& url ) 
{
	// �Ƃ肠���� view �n�̂�
	if( url.Find( L"home.pl" ) != -1 ) 			{ return ACCESS_MAIN;      } // ���C��
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_DIARY;     } // ���L���e
	if( url.Find( L"view_bbs.pl" ) != -1 ) 		{ return ACCESS_BBS;       } // �R�~���j�e�B���e
	if( url.Find( L"view_enquete.pl" ) != -1 ) 	{ return ACCESS_ENQUETE;   } // �A���P�[�g
	if( url.Find( L"view_event.pl" ) != -1 ) 	{ return ACCESS_EVENT;     } // �C�x���g
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_MYDIARY;   } // �����̓��L���e
	if( url.Find( L"view_message.pl" ) != -1 ) 	{ return ACCESS_MESSAGE;   } // ���b�Z�[�W
	if( url.Find( L"view_news.pl" ) != -1 ) 	{ return ACCESS_NEWS;      } // �j���[�X���e
	if( url.Find( L"show_friend.pl" ) != -1 ) 	{ return ACCESS_PROFILE;   } // �l�y�[�W
	if( url.Find( L"view_community.pl" ) != -1 ){ return ACCESS_COMMUNITY; } // �R�~���j�e�B�y�[�W

	// �s���Ȃ̂� INVALID �Ƃ���
	return ACCESS_INVALID;
}

/// UNICODE -> ANSI
inline std::string my_wcstombs( const std::wstring& wide_string ) {
	static std::vector<char> ansi_string(1024);
	memset( &ansi_string[0], 0x00, sizeof(char) * 1024 );
	wcstombs( &ansi_string[0], wide_string.c_str(), 1023 );
	return &ansi_string[0];
}

/// ANSI -> UNICODE
inline std::wstring my_mbstowcs( const std::string& ansi_string ) {
	std::vector<wchar_t> wide_string(1024);
	memset( &wide_string[0], 0x00, sizeof(wchar_t) * 1024 );
	mbstowcs( &wide_string[0], ansi_string.c_str(), 1023 );
	return &wide_string[0];
}

/// �J���}��؂�ŕ����񃊃X�g������B
inline bool split_by_comma( std::vector<std::string>& values, const std::string& value )
{
	values.clear();

	size_t idxFrom = 0;
	while( idxFrom < value.length() ) {
		size_t at = value.find( ',', idxFrom );
		if( at == std::string::npos ) {
			// not found.
			// idxFrom �ȍ~��ǉ����ďI���B
			values.push_back( value.substr(idxFrom) );
			return true;
		}

		// �J���}�����B
		// idxFrom ����J���}�̑O�܂Łi[idxFrom,at-1]�j��ǉ��B
		values.push_back( value.substr(idxFrom,at-idxFrom) );

		// �����J�n�ʒu�X�V
		idxFrom = at+1;
	}

	return true;
}

/**
 * url �쐬�ihttp://mixi.jp/ �̕⊮�j
 */
inline CString CreateMixiUrl(LPCTSTR str)
{
	if( wcsstr( str, L"mixi.jp" ) == NULL ) {
		CString uri;
		uri.Format(_T("http://mixi.jp/%s\n"), str);
		return uri;
	}else{
		return str;
	}
}

/**
 * line �ɁA
 * �w�肳�ꂽ�S�Ă̕����񂪏��ɑ��݂���� true ��Ԃ��B
 * �啶���������͋�ʂ��Ȃ��B
 */
inline bool LineHasStringsNoCase( 
				const CString& line, 
				LPCTSTR str1, LPCTSTR str2=NULL, LPCTSTR str3=NULL, LPCTSTR str4=NULL, LPCTSTR str5=NULL )
{
	// �啶���ɕϊ������������ΏۂƂ���
	CString target( line );
	target.MakeUpper();

	// ����������Q
	LPCTSTR findStrings[] = { str1, str2, str3, str4, str5 };

	int idx = 0;
	for( int i=0; i<5; i++ ) {
		if( findStrings[i] != NULL ) {
			CString str = findStrings[i];
			str.MakeUpper();

			idx = target.Find( str, idx );
			if( idx < 0 ) {
				return false;
			}
			idx += str.GetLength();
		}
	}

	return true;
}

}
