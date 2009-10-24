/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
/// 引用関連

namespace quote {

/// 引用種別
enum QuoteType {
	QUOTETYPE_INVALID,			///< 無効な値
	QUOTETYPE_NONE,				///< 引用なし
	QUOTETYPE_NUM,				///< コメント番号
	QUOTETYPE_NAME,				///< 名前
	QUOTETYPE_NUM_NAME,			///< コメント番号＋名前
	QUOTETYPE_BODY,				///< 本文のみ
	QUOTETYPE_NUM_BODY,			///< コメント番号＋本文
	QUOTETYPE_NAME_BODY,		///< 名前＋本文
	QUOTETYPE_NUM_NAME_BODY,	///< コメント番号＋名前＋本文
};

/**
 * 引用種別 quoteType に応じて、mixi のデータを引用文字列に変換する
 */
inline CString MakeQuoteString( const CMixiData& mixi, QuoteType quoteType )
{
	CString quoteMark = theApp.m_optionMng.GetQuoteMark();

	CString strQuote;
	switch( quoteType ) {
	case QUOTETYPE_NONE:
		return L"";
	case QUOTETYPE_BODY:
		strQuote = L"";
		break;
	case QUOTETYPE_NUM:
		if( mixi.GetCommentIndex() > 0 ) {
			strQuote.Format( L"%s%d\r\n", quoteMark, mixi.GetCommentIndex() );
		}else{
			strQuote.Format( L"%s\r\n", quoteMark );
		}
		return strQuote;
	case QUOTETYPE_NAME:
		strQuote.Format( L"%s%sさん\r\n", quoteMark, mixi.GetAuthor() );
		return strQuote;
	case QUOTETYPE_NUM_NAME:
		if( mixi.GetCommentIndex() > 0 ) {
			strQuote.Format( L"%s%d %sさん\r\n", quoteMark, mixi.GetCommentIndex(), mixi.GetAuthor() );
		}else{
			strQuote.Format( L"%s%sさん\r\n", quoteMark, mixi.GetAuthor() );
		}
		return strQuote;
	case QUOTETYPE_NUM_BODY:
		strQuote.Format( L"%s%d\r\n", quoteMark, mixi.GetCommentIndex() );
		break;
	case QUOTETYPE_NAME_BODY:
		strQuote.Format( L"%s%sさん\r\n", quoteMark, mixi.GetAuthor() );
		break;
	case QUOTETYPE_NUM_NAME_BODY:
		if( mixi.GetCommentIndex() > 0 ) {
			strQuote.Format( L"%s%d %sさん\r\n", quoteMark, mixi.GetCommentIndex(), mixi.GetAuthor() );
		}else{
			strQuote.Format( L"%s%sさん\r\n", quoteMark, mixi.GetAuthor() );
		}
		break;
	default:
		return L"";
	}

	// 本文追加
	for (size_t i=0; i<mixi.GetBodySize(); i++) {
		CString line = mixi.GetBodyItem(i);
		if( !line.IsEmpty() ) {
			// line を \r\n で分離。
			int idx=-1;
			while( (idx=line.Find( L"\r\n" ))>=0 ) {
				// \r\n の左側を挿入
				CString left = line.Left(idx);
				if( !left.IsEmpty() ) {
					strQuote += quoteMark;
					strQuote += left;
					strQuote += L"\r\n";
				}

				// \r\n の右側を使ってさらに検索
				line = line.Mid( idx+2 );
			}
			if( !line.IsEmpty() ) {
				strQuote += quoteMark;
				strQuote += line;
				strQuote += L"\r\n";
			}
		}
	}
	return strQuote;
}

}
