// HtmlArray.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "HtmlArray.h"
#include "XmlParser.h"
#include "MixiData.h"
#include "IniFile.h"
#include "util.h"

#include "MixiParser.h"

//#include <nled.h>			// LED ����

// CHtmlArray

/**
 * �R���X�g���N�^
 */
CHtmlArray::CHtmlArray()
{
}

/**
 * �f�X�g���N�^
 */
CHtmlArray::~CHtmlArray()
{
	RemoveAll();
}

/**
 * �t�@�C���ǂݍ���
 *
 * �擾�����g�s�l�k�t�@�C����ǂݍ���
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
 * Confirm�f�[�^�̉��
 *
 * post_key, pack �� hidden �^�O�̒l���擾����
 */
bool CHtmlArray::GetPostConfirmData(CPostData* data)
{
	INT_PTR count = this->GetCount();

	// ������
	data->SetPostKey( L"" );
	data->SetPacked( L"" );

	CString postKey = _T("");
	for (int i=0; i<count; i++) {
		const CString& line = this->GetAt(i);

		// <input type="hidden" name="post_key" value="xxxxxxxxxxxxxxxxxxx...xxxx">
		// ���� value �̒l���擾����
		if (line.Find(_T("post_key")) != -1) {

			if( util::GetBetweenSubString( line, L"value=\"", L"\"", postKey ) >= 0 ) {
				// ok.
			}else if( util::GetBetweenSubString( line, L"value=", L">", postKey ) >= 0 ) {
				// ok.
			}else{
				MZ3LOGGER_ERROR( L"post_key ������܂������Avalue �l��������܂���B:" + line );
				continue;
			}
			data->SetPostKey(postKey);
			MZ3LOGGER_DEBUG( L"postKey : " + postKey );
			return true;
		}

		// <input type="hidden" name="packed" value="xxxxx...xxxxx">
		// �܂���
		// <input type="hidden" name="packed" value="">
		// ���� value �̒l���擾����
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
				MZ3LOGGER_ERROR( L"packed ������܂������Avalue �l��������܂���B:" + line );
				continue;
			}
			MZ3LOGGER_DEBUG( L"packed : " + packed );
			data->SetPacked( packed );
		}

	}

	// NOT FOUND
	CString msg;
	msg.Format( L"post_key ��������܂���ł����Bpost_key[%s], packed[%s]\n", 
		data->GetPostKey(),
		data->GetPacked() );
	MZ3LOGGER_ERROR( msg );

	return false;
}

/**
 * POST �����̃`�F�b�N
 */
BOOL CHtmlArray::IsPostSucceeded( WRITEVIEW_TYPE type )
{
	INT_PTR count = this->GetCount();

	for (int i=0; i<count; i++) {
		const CString& line = this->GetAt(i);

		switch( type ) {
		case WRITEVIEW_TYPE_REPLYMESSAGE:
		case WRITEVIEW_TYPE_NEWMESSAGE:
			if (line.Find(_T("<td nowrap><b>���M����</b>���܂����B</td>")) != -1) {
				return TRUE;
			}
			break;

		default:
			if (line.Find(_T("���f�Ɏ��Ԃ������邱�Ƃ�����܂��̂ŁA�\������Ă��Ȃ��ꍇ�͏��X���҂����������B")) != -1) {
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

