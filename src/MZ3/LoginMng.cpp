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

// Login

Login::Login()
{
  m_ownerId = _T("");
}

Login::~Login()
{
}


// Login メンバ関数
void Login::Read()
{
	// ----------------------------------------
	// IDとパスワードを取得
	// ----------------------------------------

	// 実行ファイルのパスからデータファイル名を作成
	CString tmp;
	CString fileName;
	fileName.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("user.dat"));

	// ファイルが存在していればそのファイルを読み込む
	FILE* fp;

	if( util::ExistFile(fileName) ) {

		// ----------------------------------------
		// デコード処理
		// ----------------------------------------
		fp = _wfopen(fileName, _T("rb"));
		if (fp == NULL) {
			return;
		}

		m_loginMail = Read(fp);
		m_loginPwd = Read(fp);
		m_ownerId = Read(fp);
		TRACE(_T("Mail = %s\n"), m_loginMail);
		TRACE(_T("Password = %s\n"), m_loginPwd);
		TRACE(_T("OwnerId = %s\n"), m_ownerId);
		if (m_ownerId == _T("ERROR")) {
			m_ownerId = _T("");
		}
		fclose(fp);

	}
	else {
		m_loginMail = _T("");
		m_loginPwd = _T("");
		m_ownerId = _T("");
	}
}

void Login::Write()
{
	// 実行ファイルのパスからデータファイル名を作成
	CString fileName;
	fileName.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("user.dat"));

	FILE* fp;

	// ----------------------------------------
	// エンコード処理
	// ----------------------------------------
	if (m_loginMail.GetLength() == 0 && m_loginPwd.GetLength() == 0) {
		return;
	}
	if (m_ownerId.GetLength() == 0) {
		m_ownerId = _T("");
	}

	fp = _wfopen(fileName, _T("wb"));
	if (fp == NULL) {
		return;
	}

	Write(fp, m_loginMail);
	Write(fp, m_loginPwd);
	Write(fp, m_ownerId);

	fclose(fp);
}

LPCTSTR Login::Read(FILE* fp)
{
	// blowfishの初期化
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, _T("Mixi_browser_for_W-ZERO3"), 255); // 暗号化キー

	bf::bf bf;
	bf.init((unsigned char*)key, strlen(key)); // 初期化処理

	CString ret = _T("");

	int len = (int)fgetc(fp);

	// 一文字目を抜き出す
	char buf[256];
	memset(buf, 0x00, sizeof(char) * 256);
	int num = (int)fread(buf, sizeof(char), ((len/8)+1)*8, fp);

	for (int i=0; i<(int)(len / 8) + 1; i++) {
		char dBuf[9];
		memset(dBuf, 0x00, sizeof(char) * 9);
		memcpy(dBuf, buf+i*8, 8);
		bf.decrypt((unsigned char*)dBuf);

		TCHAR str[256];
		memset(str, 0x00, sizeof(TCHAR) * 256);
		mbstowcs(str, dBuf, 8);
		ret += str;
	}

	return ret.Mid(0, len);
}

void Login::Write(FILE* fp, LPCTSTR tmp)
{
	CString str = tmp;

	// blowfishの初期化
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, _T("Mixi_browser_for_W-ZERO3"), 255); // 暗号化キー

	bf::bf bf;
	bf.init((unsigned char*)key, strlen(key)); // 初期化処理

	int len = wcslen(tmp);
	fputc(len, fp);

	// パティング処理
	for (int i=0; i<(((len / 8) + 1) * 8) - len; i++) {
		str += _T("0");
	}
	for (int i=0; i<(int)(len / 8) + 1; i++) {
		CString buf = str.Mid(i * 8, 8);

		char mchar[256];
		memset(mchar, '\0', sizeof(char) * 9);
		wcstombs(mchar, buf, 8);

		bf.encrypt((unsigned char*)mchar);

		fwrite(mchar, sizeof(char), 8, fp);
	}
}

}// namespace option
