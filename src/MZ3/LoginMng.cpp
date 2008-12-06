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
	m_ownerId = _T("");
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
	m_loginMail = _T("");
	m_loginPwd = _T("");
	m_ownerId = _T("");

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
		m_loginMail   = Read(fp);
		m_loginPwd    = Read(fp);
		CString dummy = Read(fp);		// 旧オーナーID（互換性確保のためのパディング）

		// Twitter
		m_twitterId	  = Read(fp);
		m_twitterPwd  = Read(fp);

		// Wassr
		m_wassrId	  = Read(fp);
		m_wassrPwd    = Read(fp);

		// gooホーム
		m_gooId					  = Read(fp);
		m_goohomeQuoteMailAddress = Read(fp);

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
	if (m_loginMail.GetLength() == 0 && m_loginPwd.GetLength() == 0 &&
		m_twitterId.GetLength() == 0 && m_twitterPwd.GetLength() == 0 &&
		m_wassrId.GetLength() == 0 && m_wassrPwd.GetLength() == 0
		)
	{
		return;
	}

	FILE* fp = _wfopen(fileName, _T("wb"));
	if (fp == NULL) {
		return;
	}

	// mixi
	Write( fp, m_loginMail );
	Write( fp, m_loginPwd  );
	CString dummyId;
	Write( fp, dummyId );

	// Twitter
	Write( fp, m_twitterId );
	Write( fp, m_twitterPwd );

	// Wassr
	Write( fp, m_wassrId );
	Write( fp, m_wassrPwd );

	// gooホーム
	Write( fp, m_gooId );
	Write( fp, m_goohomeQuoteMailAddress );

	fclose(fp);
}

CString Login::Read(FILE* fp)
{
	// blowfishの初期化
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, CRYPT_KEY, 255); // 暗号化キー

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
	wcstombs(key, CRYPT_KEY, 255); // 暗号化キー

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
