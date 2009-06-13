/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// LoginMng.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "LoginMng.h"
#include "bf.h"
#include "Winsock2.h"
#include "util.h"

/// オプションデータ
namespace option {

#define CRYPT_KEY	_T("Mixi_browser_for_W-ZERO3")

// Login

Login::Login()
{
	m_mixiOwnerId = _T("");
}

Login::~Login()
{
}

/**
 * ファイルからログイン情報を取得
 */
void Login::Read()
{
	// ----------------------------------------
	// IDとパスワードを取得
	// ----------------------------------------

	// 初期値設定
	m_idList.clear();

	// 実行ファイルのパスからデータファイル名を作成
	CString fileName = theApp.GetAppDirPath() + _T("\\user.dat");

	// ファイルが存在していればそのファイルを読み込む
	if( util::ExistFile(fileName) ) {

		// ----------------------------------------
		// デコード処理
		// ----------------------------------------
		FILE* fp = _wfopen(fileName, _T("rb"));
		if (fp == NULL) {
			return;
		}

		// mixi
		CString s;
		SetMixiEmail( ReadItem(fp) );
		SetMixiPassword( ReadItem(fp) );
		CString dummy = ReadItem(fp);		// 旧オーナーID（互換性確保のためのパディング）

		// Twitter
		SetTwitterId( ReadItem(fp) );
		SetTwitterPassword( ReadItem(fp) );

		// Wassr
		SetWassrId( ReadItem(fp) );
		SetWassrPassword( ReadItem(fp) );

		// gooホーム
		SetGooId( ReadItem(fp) );
		SetGoohomeQuoteMailAddress( ReadItem(fp) );

		//--- 以上の9項目は旧バージョンとの互換用データ。
		//--- v1.0.0 以降は下記の汎用データ構造を正とし、上書きする。

		// その他の汎用ID
		// シリアライズした形式で保存されているのでデコードする
		CString serializedId = ReadItem(fp, true);

		UnserializeId(serializedId);

//		MZ3LOGGER_DEBUG(serializedId);
//		MZ3LOGGER_DEBUG(util::FormatString(L"serializedId len: %d", serializedId.GetLength()));

		fclose(fp);
	}
}

/**
 * ファイルからログイン情報を出力
 */
void Login::Write()
{
	// 実行ファイルのパスからデータファイル名を作成
	CString fileName = theApp.GetAppDirPath() + _T("\\user.dat");

	// ----------------------------------------
	// エンコード処理
	// ----------------------------------------
	FILE* fp = _wfopen(fileName, _T("wb"));
	if (fp == NULL) {
		return;
	}

	// mixi
	WriteItem( fp, GetMixiEmail() );
	WriteItem( fp, GetMixiPassword() );
	CString dummyId;
	WriteItem( fp, dummyId );

	// Twitter
	WriteItem( fp, GetTwitterId() );
	WriteItem( fp, GetTwitterPassword() );

	// Wassr
	WriteItem( fp, GetWassrId() );
	WriteItem( fp, GetWassrPassword() );

	// gooホーム
	WriteItem( fp, GetGooId() );
	WriteItem( fp, GetGoohomeQuoteMailAddress() );

	//--- 以上の9項目は旧バージョンとの互換用データ。
	//--- v1.0.0 以降は下記の汎用データ構造を正する。

	// その他の汎用ID
	CString serializedId = SerializeId();
	WriteItem( fp, serializedId, true );

	fclose(fp);
}

CString Login::ReadItem(FILE* fp, bool bSupportOver255)
{
	// blowfishの初期化
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, CRYPT_KEY, 255); // 暗号化キー

	bf::bf bf;
	bf.init((unsigned char*)key, strlen(key)); // 初期化処理

	CString ret = _T("");

	DWORD len = 0;
	if (bSupportOver255) {
		int lo = fgetc(fp);
		if (lo==EOF) {
			MZ3LOGGER_INFO(L"ファイル終端のため終了します");
			return L"";
		}
		int hi = fgetc(fp);
		if (hi==EOF) {
			MZ3LOGGER_INFO(L"ファイル終端のため終了します");
			return L"";
		}
		len = MAKEWORD((BYTE)lo, (BYTE)hi);

//		MZ3LOGGER_DEBUG(util::FormatString(L" ReadItem, len[%d]", len));
	} else {
		len = fgetc(fp);
		if (len==EOF) {
			MZ3LOGGER_INFO(L"ファイル終端のため終了します");
			return L"";
		}
	}

	// 一文字目を抜き出す
	std::vector<char> buf;
	int p_len = ((len/8)+1)*8;
	if (p_len==0) {
		MZ3LOGGER_ERROR(L"長さ不正のため終了します");
		return L"";
	}
	buf.resize(p_len);

	char* p = &buf[0];
	memset(p, 0x00, sizeof(char) * p_len);
	int num = (int)fread(p, sizeof(char), p_len, fp);
	if (num==0) {
		MZ3LOGGER_INFO(L"ファイル終端のため終了します");
		return L"";
	}

	CStringA ret_mbs;
	for (int i=0; i<(int)(len / 8) + 1; i++) {
		char dBuf[9];
		memset(dBuf, 0x00, sizeof(char) * 9);
		memcpy(dBuf, p+i*8, 8);
		bf.decrypt((unsigned char*)dBuf);

		ret_mbs += dBuf;
	}
	ret = CString(ret_mbs.Mid(0, len));
	return ret;
}

void Login::WriteItem(FILE* fp, LPCTSTR tmp, bool bSupportOver255)
{
	CStringA str_mbs(tmp);

	// blowfishの初期化
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, CRYPT_KEY, 255); // 暗号化キー

	bf::bf bf;
	bf.init((unsigned char*)key, strlen(key)); // 初期化処理

	int len = str_mbs.GetLength();//wcslen(tmp);
	if (bSupportOver255) {
//		MZ3LOGGER_DEBUG(util::FormatString(L" WriteItem, len[%d]", len));

		BYTE l = LOBYTE(len);
		BYTE h = HIBYTE(len);
		fputc(l, fp);
		fputc(h, fp);
	} else {
		fputc(len, fp);
	}

	// パティング処理
	for (int i=0; i<(((len / 8) + 1) * 8) - len; i++) {
		str_mbs += _T("0");
	}

	for (int i=0; i<(int)(len / 8) + 1; i++) {
		CStringA buf = str_mbs.Mid(i * 8, 8);

		char mchar[256];
		memset(mchar, '\0', sizeof(char) * 9);
		memcpy(mchar, buf, 8);

		bf.encrypt((unsigned char*)mchar);

		fwrite(mchar, sizeof(char), 8, fp);
	}
}

inline CString my_simple_url_encode(CString s)
{
	s.Replace(L"%", L"%25");
	s.Replace(L"<", L"%3C");
	s.Replace(L">", L"%3E");
	s.Replace(L",", L"%2C");

//	MZ3LOGGER_DEBUG(s);
	return s;
}

inline CString my_simple_url_decode(CString s)
{
	s.Replace(L"%2C", L",");
	s.Replace(L"%3E", L">");
	s.Replace(L"%3C", L"<");
	s.Replace(L"%25", L"%");

	return s;
}

/// m_idList をシリアライズする
CString Login::SerializeId()
{
	// test 
/*	m_idList.clear();
	Data d;
	d.strServiceName = L"mixi";
	d.strId          = L"takke@takke.com";
	d.strPassword    = L"pa55W04d";
	d.strDummy1      = L"";
	d.strDummy2      = L"";
	m_idList.push_back(d);
	d.strServiceName = L"gooホーム";
	d.strId          = L"<>----%22";
	d.strPassword    = L"__%,XX";
	d.strDummy1      = L"aaa";
	d.strDummy2      = L"bbb";
	m_idList.push_back(d);
	d.strServiceName = L"GMail";
	d.strId          = L"takke30@gmail.com";
	d.strPassword    = L"--__abc%%%<,>";
	d.strDummy1      = L"%3C%3E%2C%25";
	d.strDummy2      = L"%3C%3E%2C%25";
	m_idList.push_back(d);
*/
	/*
	 * シリアライズ形式：
	 *   "<service_name,id,pw,d1,d2><service_name,id,pw,d1,d2>..."
	 *   ただし、service_name 等の要素内の "<>,%" はそれぞれ "%3C%3E%2C%25" (URL Encoding) に変換する。
	 */
	CString result = L"";
	for (size_t i=0; i<m_idList.size(); i++) {
		Data d = m_idList[i];
		result += L"<";
		result += my_simple_url_encode(d.strServiceName);
		result += L",";
		result += my_simple_url_encode(d.strId);
		result += L",";
		result += my_simple_url_encode(d.strPassword);
		result += L",";
		result += my_simple_url_encode(d.strDummy1);
		result += L",";
		result += my_simple_url_encode(d.strDummy2);
		result += L">";
	}

//	MessageBox(NULL, result, NULL, MB_OK);

	return result;
}

/// serializedId をアンシリアライズして m_idList を生成する
void Login::UnserializeId(const CString& serializedId)
{
//	MessageBox(NULL, serializedId, NULL, MB_OK);
//	m_idList.clear();
	
	CString s(serializedId);

	// s : "<name,id,pw,d1,d2><name,id,pw,d1,d2>...<name,id,pw,d1,d2>"
	// <> を切り出し、それぞれを , で区切る
	int start = 0;
	int pos = 0;
	while ((pos=s.Find('>', start))!=-1) {
		// s[start] が '<' であること。
		if (s.GetLength()>start && s[start]=='<') {
			CString items = s.Mid(start+1, pos-start-1);

			Data d;

			int idx1 = 0, idx2 = 0;
			idx2 = items.Find(',', idx1);
			if (idx2==-1) {
				MZ3LOGGER_ERROR(L"user.dat形式エラー");
				break;
			}
			d.strServiceName = my_simple_url_decode(items.Mid(idx1, idx2-idx1));
			idx1 = idx2+1;

			idx2 = items.Find(',', idx1);
			if (idx2==-1) {
				MZ3LOGGER_ERROR(L"user.dat形式エラー");
				break;
			}
			d.strId = my_simple_url_decode(items.Mid(idx1, idx2-idx1));
			idx1 = idx2+1;

			idx2 = items.Find(',', idx1);
			if (idx2==-1) {
				MZ3LOGGER_ERROR(L"user.dat形式エラー");
				break;
			}
			d.strPassword = my_simple_url_decode(items.Mid(idx1, idx2-idx1));
			idx1 = idx2+1;

			idx2 = items.Find(',', idx1);
			if (idx2==-1) {
				MZ3LOGGER_ERROR(L"user.dat形式エラー");
				break;
			}
			d.strDummy1 = my_simple_url_decode(items.Mid(idx1, idx2-idx1));
			idx1 = idx2+1;

			d.strDummy2 = my_simple_url_decode(items.Mid(idx1));

/*			MessageBox(NULL, 
				util::FormatString(
					L"sn : [%s]\r\n"
					L"id : [%s]\r\n"
					L"pw : [%s]\r\n"
					L"d1 : [%s]\r\n"
					L"d2 : [%s]", d.strServiceName, d.strId, d.strPassword, d.strDummy1, d.strDummy2), 
				NULL, MB_OK);
*/
			m_idList.push_back(d);

			start = pos+1;
		} else {
			MZ3LOGGER_ERROR(L"user.dat形式エラー");
			break;
		}
	}
}

/// ID 設定
void Login::SetId(LPCTSTR szServiceName, LPCTSTR id)
{
	for (size_t i=0; i<m_idList.size(); i++) {
		if (m_idList[i].strServiceName == szServiceName) {
			m_idList[i].strId = id;
			return;
		}
	}
	m_idList.push_back(Data(szServiceName, id, L""));
}

/// Password 設定
void Login::SetPassword(LPCTSTR szServiceName, LPCTSTR pw)
{
	for (size_t i=0; i<m_idList.size(); i++) {
		if (m_idList[i].strServiceName == szServiceName) {
			m_idList[i].strPassword = pw;
			return;
		}
	}
	m_idList.push_back(Data(szServiceName, L"", pw));
}

/// ID 取得 (未登録時は NULLを返す)
LPCTSTR Login::GetId(LPCTSTR szServiceName)
{
	for (size_t i=0; i<m_idList.size(); i++) {
		if (m_idList[i].strServiceName == szServiceName) {
			return m_idList[i].strId;
		}
	}
	return NULL;
}

/// Password 取得 (未登録時は NULLを返す)
LPCTSTR Login::GetPassword(LPCTSTR szServiceName)
{
	for (size_t i=0; i<m_idList.size(); i++) {
		if (m_idList[i].strServiceName == szServiceName) {
			return m_idList[i].strPassword;
		}
	}
	return NULL;
}


}// namespace option
