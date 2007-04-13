// BookmarkMng.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "BookmarkMng.h"
#include "MixiData.h"
#include "util.h"
#include "IniFile.h"

/// �I�v�V�����f�[�^
namespace option {

// Bookmark

Bookmark::Bookmark()
{
}

Bookmark::~Bookmark()
{
}


// Bookmark �����o�֐�

void Bookmark::Load( CMixiDataList& bookmark )
{
	// �t�@�C������ǂݏo��

	CString fileName;
	fileName.Format( _T("%s\\favorite.dat"), theApp.GetAppDirPath());

	FILE* fp;
	fp = _wfopen(fileName, _T("r"));

	std::vector<TCHAR> line(4096);
	while (fgetws(&line[0], 4096, fp) != NULL) {
		// �P�s�ǂݏo��
		// �������i�[
		Devide( &line[0], bookmark );
	}
	fclose(fp);
}

void Bookmark::Save( CMixiDataList& bookmark )
{
	CString fileName;
	fileName.Format( _T("%s\\favorite.dat"), theApp.GetAppDirPath());

	FILE* fp = _wfopen(fileName, _T("w"));

	INT_PTR count = bookmark.size();
	CString buf;
	CString type;

	for (int i=0; i<count; i++) {
		CMixiData& data = bookmark[i];
		switch (data.GetAccessType()) {
		case ACCESS_BBS:
			type = _T("b");
			break;
		case ACCESS_ENQUETE:
			type = _T("e");
			break;
		case ACCESS_EVENT:
			type = _T("v");
			break;
		}
		buf.Format(_T("%s<>%d<>%s<>%s\n"), type, data.GetID(), data.GetName(), data.GetTitle());
		fputws(buf, fp);
	}

	fclose(fp);
}

void Bookmark::Devide( LPCTSTR line, CMixiDataList& bookmark )
{
	const CString& str = line;
	int index = 0;

	CMixiData data;

	{
		CString type = str.Tokenize(_T("<>"), index);
		if (type == _T("b")) {
			data.SetAccessType( ACCESS_BBS );
		}else if (type == _T("e")) {
			data.SetAccessType( ACCESS_ENQUETE );
		}else if (type == _T("v")) {
			data.SetAccessType( ACCESS_EVENT );
		}
	}
	{
		CString id = str.Tokenize(_T("<>"), index);
		data.SetID( _wtoi(id) );
	}
	{
		CString name = str.Tokenize(_T("<>"), index);
		data.SetName( name );
	}
	{
		CString title = str.Tokenize(_T("<>"), index);
		title.Replace(_T("\n"), _T(""));
		data.SetTitle( title );
	}


	{
		// URL ����
		CString uri;
		switch (data.GetAccessType()) {
		case ACCESS_BBS:
			uri.Format( _T("view_bbs.pl?id=%d"), data.GetID() );
			break;
		case ACCESS_ENQUETE:
			uri.Format( _T("view_enquete.pl?id=%d"), data.GetID() );
			break;
		case ACCESS_EVENT:
			uri.Format( _T("view_event.pl?id=%d"), data.GetID() );
			break;
		}

		data.SetURL(uri);
	}

	// last index �̐���
	{
		// URI�𕪉�
		CString buf = data.GetURL();

		switch (data.GetAccessType()) {
		case ACCESS_DIARY:
		case ACCESS_MYDIARY:
			buf = buf.Mid(buf.Find(_T("id="))+ wcslen(_T("id=")));
			buf = buf.Left(buf.Find(_T("&")));
			buf.Format(_T("d%s"), buf);
			break;
		case ACCESS_BBS:
			buf.Format(_T("b%d"), data.GetID());
			break;
		case ACCESS_EVENT:
			buf.Format(_T("v%d"), data.GetID());
			break;
		case ACCESS_ENQUETE:
			buf.Format(_T("e%d"), data.GetID());
			break;
		}

		char idKey[24];
		memset(idKey, 0x00, sizeof(char) * 24);
		wcstombs(idKey, buf, 24);

		// �L�[�擾
		int lastIndex = -1;
		{
			if (theApp.m_logfile.RecordExists(idKey, "Log") != false) {
				// ���R�[�h���݂�΂�����g��
				lastIndex = atoi(theApp.m_logfile.GetValue(idKey, "Log").c_str());
			}
			else {
				lastIndex = -1;
			}
		}
		data.SetLastIndex(lastIndex);
	}

	bookmark.push_back( data );
}

BOOL Bookmark::Add( CMixiData* master, CMixiDataList& bookmark )
{
	// ���ꍀ�ڂ̃`�F�b�N
	INT_PTR count = bookmark.size();
	for (int i=0; i<count; i++) {
		CMixiData& data = bookmark[i];
		if (data.GetID() == master->GetID()) {
			// �������ڂ��������̂œo�^���Ȃ�
			return FALSE;
		}
	}

	CMixiData data;
	data.SetName(master->GetName());
	data.SetID(master->GetID());
	data.SetAccessType(master->GetAccessType());
	data.SetLastIndex(master->GetLastIndex());
	data.SetCommentCount(master->GetCommentCount());
	data.SetURL(master->GetURL());

	TCHAR uri[256];
	memset(uri, 0x00, sizeof(TCHAR) * 256);
	switch (data.GetAccessType()) {
	case ACCESS_BBS:
		swprintf(uri, _T("view_bbs.pl?id=%d"), data.GetID());
		break;
	case ACCESS_ENQUETE:
		swprintf(uri, _T("view_enquete.pl?id=%d"), data.GetID());
		break;
	case ACCESS_EVENT:
		swprintf(uri, _T("view_event.pl?id=%d"), data.GetID());
		break;
	}

	data.SetURL(master->GetURL());

	data.SetTitle(master->GetTitle());

	CString buf = master->GetTitle();
	int index = buf.ReverseFind(_T('(')) - 1;
	data.SetTitle(buf.Left(index));

	// �ǉ�
	bookmark.push_back( data );

	// �ۑ�
	Save( bookmark );

	return TRUE;
}

BOOL Bookmark::Delete( CMixiData* data, CMixiDataList& bookmark )
{
	BOOL bRet = FALSE;
	INT_PTR count = bookmark.size();
	for (int i=0; i<count; i++) {
		if (data->GetID() == bookmark[i].GetID()) {
			bookmark.erase( bookmark.begin() + i );
			bRet = TRUE;
			break;
		}
	}

	Save( bookmark );

	return bRet;
}

}// namespace option
