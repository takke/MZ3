// HtmlArray.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "HtmlArray.h"
#include "XmlParser.h"
#include "MixiData.h"
#include "IniFile.h"
#include "util.h"

#include "MixiParser.h"

//#include <nled.h>			// LED 制御

// CHtmlArray

/**
 * コンストラクタ
 */
CHtmlArray::CHtmlArray()
{
}

/**
 * デストラクタ
 */
CHtmlArray::~CHtmlArray()
{
	RemoveAll();
}

/**
 * ファイル読み込み
 *
 * 取得したＨＴＭＬファイルを読み込む
 */
void CHtmlArray::Load( LPCTSTR szHtmlFilename )
{
	RemoveAll();

	FILE* fp = _wfopen(szHtmlFilename, _T("r"));
	if( fp != NULL ) {
		TCHAR buf[4096];

		while (fgetws(buf, 4096, fp) != NULL) {
			this->Add(buf);
		}
		fclose(fp);
	}
}

/**
 * Confirmデータの解析
 *
 * post_key, pack の hidden タグの値を取得する
 */
bool CHtmlArray::GetPostConfirmData(CPostData* data)
{
	INT_PTR count = this->GetCount();

	// 初期化
	data->SetPostKey( L"" );
	data->SetPacked( L"" );

	CString postKey = _T("");
	for (int i=0; i<count; i++) {
		const CString& line = this->GetAt(i);

		// <input type="hidden" name="post_key" value="xxxxxxxxxxxxxxxxxxx...xxxx">
		// から value の値を取得する
		if (line.Find(_T("post_key")) != -1) {

			if( util::GetBetweenSubString( line, L"value=\"", L"\"", postKey ) >= 0 ) {
				// ok.
			}else if( util::GetBetweenSubString( line, L"value=", L">", postKey ) >= 0 ) {
				// ok.
			}else{
				MZ3LOGGER_ERROR( L"post_key がありましたが、value 値が見つかりません。:" + line );
				continue;
			}
			data->SetPostKey(postKey);
			MZ3LOGGER_DEBUG( L"postKey : " + postKey );
			return true;
		}

		// <input type="hidden" name="packed" value="xxxxx...xxxxx">
		// または
		// <input type="hidden" name="packed" value="">
		// から value の値を取得する
		if( line.Find( L"packed" ) != -1 ) {
			CString packed;
			if( line.Find( L"value=\"\"" ) != -1 ) {
				continue;
			}

			if( util::GetBetweenSubString( line, L"value=\"", L"\"", packed ) >= 0 ) {
				// ok.
			}else if( util::GetBetweenSubString( line, L"value=", L">", packed ) >= 0 ) {
				// ok.
			}else{
				MZ3LOGGER_ERROR( L"packed がありましたが、value 値が見つかりません。:" + line );
				continue;
			}
			MZ3LOGGER_DEBUG( L"packed : " + packed );
			data->SetPacked( packed );
		}

	}

	// NOT FOUND
	CString msg;
	msg.Format( L"post_key が見つかりませんでした。post_key[%s], packed[%s]\n", 
		data->GetPostKey(),
		data->GetPacked() );
	MZ3LOGGER_ERROR( msg );

	return false;
}

/**
 * POST 完了のチェック
 */
BOOL CHtmlArray::IsPostSucceeded( WRITEVIEW_TYPE type )
{
	INT_PTR count = this->GetCount();

	for (int i=0; i<count; i++) {
		const CString& line = this->GetAt(i);

		switch( type ) {
		case WRITEVIEW_TYPE_REPLYMESSAGE:
		case WRITEVIEW_TYPE_NEWMESSAGE:
			if (line.Find(_T("<td nowrap><b>送信完了</b>しました。</td>")) != -1) {
				return TRUE;
			}
			break;

		default:
			if (line.Find(_T("反映に時間がかかることがありますので、表示されていない場合は少々お待ちください。")) != -1) {
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

