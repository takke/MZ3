/**
 * MZ3非依存ユーティリティ
 */
#pragma once

/// MZ3 用ユーティリティ
namespace util
{
/**
 * 指定されたIDのウィンドウを移動する
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
 * リストコントロールのフォーカス状態と選択状態を設定する。
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
 * リストコントロール内で指定されたインデックスが表示状態にあるかどうかを返す。
 */
inline bool IsVisibleOnListBox( CListCtrl& listCtrl, int idx )
{
	int idxTop  = listCtrl.GetTopIndex();				// 画面の一番上の表示項目のインデックス
	int idxLast = idxTop + listCtrl.GetCountPerPage();	// 画面の一番下の表示項目のインデックス
	if( idx >= idxLast ) {
		// 下方にある
		return false;
	}
	if( idx <= idxTop ) {
		// ただし、先頭項目の場合は除く
		if( idx==0 ) {
			return true;
		}

		// 上方にある
		return false;
	}
	return true;
}

/**
 * ファイルを開く
 */
inline bool OpenByShellExecute( LPCTSTR target )
{
	// ファイルを開く
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
 * ファイルの存在チェック
 *
 * ファイルが存在すれば true、存在しなければ false を返す
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
 * ファイルが存在すれば削除する
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
 * 指定された URL を既定のブラウザで開く
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

/// アクセス種別を文字列に変換する
inline LPCTSTR AccessType2Message( ACCESS_TYPE type )
{
	LPCTSTR text = L"";
	switch( type ) {
	case ACCESS_LOGIN:				text = L"ログイン";				break;
	case ACCESS_MAIN:				text = L"メイン";				break;
	case ACCESS_DIARY:				text = L"日記内容";				break;
	case ACCESS_NEWS:				text = L"ニュース";				break;
	case ACCESS_BBS:				text = L"コミュ書込";			break;
	case ACCESS_ENQUETE:			text = L"アンケート";			break;
	case ACCESS_EVENT:				text = L"イベント";				break;
	case ACCESS_MYDIARY:			text = L"日記";					break;
	case ACCESS_ADDDIARY:			text = L"日記投稿";				break;
	case ACCESS_MESSAGE:			text = L"メッセージ";			break;
	case ACCESS_IMAGE:				text = L"画像";					break;
	case ACCESS_MOVIE:				text = L"動画";					break;
	case ACCESS_DOWNLOAD:			text = L"ダウンロード";			break;
	case ACCESS_PROFILE:			text = L"プロフィール";			break;
	case ACCESS_COMMUNITY:			text = L"コミュニティ";			break;
	case ACCESS_LIST_INTRO:			text = L"紹介文";				break;

	case ACCESS_LIST_DIARY:			text = L"日記一覧";				break;
	case ACCESS_LIST_NEW_COMMENT:	text = L"新着コメント一覧";		break;
	case ACCESS_LIST_COMMENT:		text = L"コメント一覧";			break;
	case ACCESS_LIST_NEWS:			text = L"ニュース一覧";			break;
	case ACCESS_LIST_FAVORITE:		text = L"お気に入り";			break;
	case ACCESS_LIST_FRIEND:		text = L"マイミク一覧";			break;
	case ACCESS_LIST_COMMUNITY:		text = L"コミュニティ一覧";		break;
	case ACCESS_LIST_NEW_BBS_COMMENT:text = L"コミュコメント記入履歴";		break;
	case ACCESS_LIST_NEW_BBS:		text = L"コミュ書込一覧";		break;
	case ACCESS_LIST_BBS:			text = L"トピック一覧";			break;
	case ACCESS_LIST_MYDIARY:		text = L"日記一覧";				break;
	case ACCESS_LIST_FOOTSTEP:		text = L"足あと";				break;
	case ACCESS_LIST_MESSAGE_IN:	text = L"メッセージ(受信箱)";	break;
	case ACCESS_LIST_MESSAGE_OUT:	text = L"メッセージ(送信箱)";	break;
	case ACCESS_LIST_BOOKMARK:		text = L"ブックマーク";			break;
	case ACCESS_LIST_CALENDAR:		text = L"カレンダー";			break;  //icchu追加

	case ACCESS_GROUP_COMMUNITY:	text = L"コミュニティG";		break;
	case ACCESS_GROUP_MESSAGE:		text = L"メッセージG";			break;
	case ACCESS_GROUP_MYDIARY:		text = L"日記G";				break;
	case ACCESS_GROUP_NEWS:			text = L"ニュースG";			break;
	case ACCESS_GROUP_OTHERS:		text = L"その他G";				break;

	// POST 系
	case ACCESS_POST_CONFIRM_COMMENT:		text = L"コメント投稿（確認）";		break;
	case ACCESS_POST_ENTRY_COMMENT:			text = L"コメント投稿（書込）";		break;
	case ACCESS_POST_CONFIRM_REPLYMESSAGE:	text = L"メッセージ返信（確認）";	break;
	case ACCESS_POST_ENTRY_REPLYMESSAGE:	text = L"メッセージ返信（書込）";	break;
	case ACCESS_POST_CONFIRM_NEWMESSAGE:	text = L"新規メッセージ（確認）";	break;
	case ACCESS_POST_ENTRY_NEWMESSAGE:		text = L"新規メッセージ（書込）";	break;
	case ACCESS_POST_CONFIRM_NEWDIARY:		text = L"日記投稿（確認）";			break;
	case ACCESS_POST_ENTRY_NEWDIARY:		text = L"コメント投稿（書込）";		break;

	case ACCESS_INVALID:			text = L"<invalid>";			break;
	default:						text = L"<unknown>";			break;
	}
	return text;
}

/// int 型数値を文字列に変換する
inline CStringW int2str( int n )
{
	CString s;
	s.Format( L"%d", n );
	return s;
}

/// int 型数値を文字列に変換する
inline CStringA int2str_a( int n )
{
	CStringA s;
	s.Format( "%d", n );
	return s;
}

/**
 * 文字列 szLeft と文字列 szRight で囲まれた部分文字列を取得し、szRight の次の文字のインデックスを返す。
 *
 * @return szRight の次の文字の位置を返す。文字列が見つからないときは -1 を返す。
 */
inline int GetBetweenSubString( const CString& str, LPCTSTR szLeft, LPCTSTR szRight, CString& result )
{
	int pos1 = str.Find( szLeft );
	if( pos1 == -1 )
		return -1;

	// 部分文字列の開始位置=szLeft開始位置+szLeftの長さ
	int start = pos1+wcslen(szLeft);

	int pos2 = str.Find( szRight, start );
	if( pos2 == -1 ) 
		return -1;

	// 部分文字列の長さ
	int len = pos2-start;
	if( len <= 0 ) {
		return -1;
	}

	// 部分文字列の取得
	result = str.Mid( start, len );

	// szRight の次の文字の位置を返す
	return pos2+wcslen(szRight);
}

/**
 * 文字列 szKey（の次の文字）以降の部分文字列を取得し、szKey のインデックスを返す。
 *
 * @return szKey の位置を返す。文字列が見つからないときは -1 を返す。
 */
inline int GetAfterSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos1 = str.Find( szKey );
	if( pos1 == -1 )
		return -1;

	// 部分文字列の開始位置 = szKey開始位置 + szKeyの長さ
	int start = pos1+wcslen(szKey);

	// 部分文字列の取得
	result = str.Mid( start );

	// szKey の位置を返す
	return pos1;
}

/**
 * 文字列 szKey より前の部分文字列を取得し、szKey のインデックスを返す。
 *
 * @return szKey の位置を返す。文字列が見つからないときは -1 を返す。
 */
inline int GetBeforeSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos = str.Find( szKey );
	if( pos == -1 )
		return -1;

	// 部分文字列の取得
	result = str.Left( pos );

	// szKey の位置を返す
	return pos;
}

/**
 * 数値を3桁区切り文字列に変換する
 */
inline CString int2comma_str(int n)
{
	if( n == 0 ) {
		return L"0";
	}

	CString s;

	int figure = 0;		// 桁数（1桁なら0）

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