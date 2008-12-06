/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// LoginMng.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "LoginMng.h"
#include "bf.h"
#include "Winsock2.h"
#include "util.h"

/// �I�v�V�����f�[�^
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
 * �t�@�C�����烍�O�C�������擾
 */
void Login::Read()
{
	// ----------------------------------------
	// ID�ƃp�X���[�h���擾
	// ----------------------------------------

	// �����l�ݒ�
	m_loginMail = _T("");
	m_loginPwd = _T("");
	m_ownerId = _T("");

	// ���s�t�@�C���̃p�X����f�[�^�t�@�C�������쐬
	CString fileName = theApp.GetAppDirPath() + _T("\\user.dat");

	// �t�@�C�������݂��Ă���΂��̃t�@�C����ǂݍ���
	if( util::ExistFile(fileName) ) {

		// ----------------------------------------
		// �f�R�[�h����
		// ----------------------------------------
		FILE* fp = _wfopen(fileName, _T("rb"));
		if (fp == NULL) {
			return;
		}

		// mixi
		m_loginMail   = Read(fp);
		m_loginPwd    = Read(fp);
		CString dummy = Read(fp);		// ���I�[�i�[ID�i�݊����m�ۂ̂��߂̃p�f�B���O�j

		// Twitter
		m_twitterId	  = Read(fp);
		m_twitterPwd  = Read(fp);

		// Wassr
		m_wassrId	  = Read(fp);
		m_wassrPwd    = Read(fp);

		// goo�z�[��
		m_gooId					  = Read(fp);
		m_goohomeQuoteMailAddress = Read(fp);

		fclose(fp);
	}
}

/**
 * �t�@�C�����烍�O�C�������o��
 */
void Login::Write()
{
	// ���s�t�@�C���̃p�X����f�[�^�t�@�C�������쐬
	CString fileName = theApp.GetAppDirPath() + _T("\\user.dat");

	// ----------------------------------------
	// �G���R�[�h����
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

	// goo�z�[��
	Write( fp, m_gooId );
	Write( fp, m_goohomeQuoteMailAddress );

	fclose(fp);
}

CString Login::Read(FILE* fp)
{
	// blowfish�̏�����
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, CRYPT_KEY, 255); // �Í����L�[

	bf::bf bf;
	bf.init((unsigned char*)key, strlen(key)); // ����������

	CString ret = _T("");

	int len = (int)fgetc(fp);

	// �ꕶ���ڂ𔲂��o��
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

	// blowfish�̏�����
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, CRYPT_KEY, 255); // �Í����L�[

	bf::bf bf;
	bf.init((unsigned char*)key, strlen(key)); // ����������

	int len = wcslen(tmp);
	fputc(len, fp);

	// �p�e�B���O����
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
