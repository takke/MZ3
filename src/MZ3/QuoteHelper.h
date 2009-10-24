/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
/// ���p�֘A

namespace quote {

/// ���p���
enum QuoteType {
	QUOTETYPE_INVALID,			///< �����Ȓl
	QUOTETYPE_NONE,				///< ���p�Ȃ�
	QUOTETYPE_NUM,				///< �R�����g�ԍ�
	QUOTETYPE_NAME,				///< ���O
	QUOTETYPE_NUM_NAME,			///< �R�����g�ԍ��{���O
	QUOTETYPE_BODY,				///< �{���̂�
	QUOTETYPE_NUM_BODY,			///< �R�����g�ԍ��{�{��
	QUOTETYPE_NAME_BODY,		///< ���O�{�{��
	QUOTETYPE_NUM_NAME_BODY,	///< �R�����g�ԍ��{���O�{�{��
};

/**
 * ���p��� quoteType �ɉ����āAmixi �̃f�[�^�����p������ɕϊ�����
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
		strQuote.Format( L"%s%s����\r\n", quoteMark, mixi.GetAuthor() );
		return strQuote;
	case QUOTETYPE_NUM_NAME:
		if( mixi.GetCommentIndex() > 0 ) {
			strQuote.Format( L"%s%d %s����\r\n", quoteMark, mixi.GetCommentIndex(), mixi.GetAuthor() );
		}else{
			strQuote.Format( L"%s%s����\r\n", quoteMark, mixi.GetAuthor() );
		}
		return strQuote;
	case QUOTETYPE_NUM_BODY:
		strQuote.Format( L"%s%d\r\n", quoteMark, mixi.GetCommentIndex() );
		break;
	case QUOTETYPE_NAME_BODY:
		strQuote.Format( L"%s%s����\r\n", quoteMark, mixi.GetAuthor() );
		break;
	case QUOTETYPE_NUM_NAME_BODY:
		if( mixi.GetCommentIndex() > 0 ) {
			strQuote.Format( L"%s%d %s����\r\n", quoteMark, mixi.GetCommentIndex(), mixi.GetAuthor() );
		}else{
			strQuote.Format( L"%s%s����\r\n", quoteMark, mixi.GetAuthor() );
		}
		break;
	default:
		return L"";
	}

	// �{���ǉ�
	for (size_t i=0; i<mixi.GetBodySize(); i++) {
		CString line = mixi.GetBodyItem(i);
		if( !line.IsEmpty() ) {
			// line �� \r\n �ŕ����B
			int idx=-1;
			while( (idx=line.Find( L"\r\n" ))>=0 ) {
				// \r\n �̍�����}��
				CString left = line.Left(idx);
				if( !left.IsEmpty() ) {
					strQuote += quoteMark;
					strQuote += left;
					strQuote += L"\r\n";
				}

				// \r\n �̉E�����g���Ă���Ɍ���
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
