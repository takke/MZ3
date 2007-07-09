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

// Login

Login::Login()
{
  m_ownerId = _T("");
}

Login::~Login()
{
}


// Login �����o�֐�
void Login::Read()
{
	// ----------------------------------------
	// ID�ƃp�X���[�h���擾
	// ----------------------------------------

	// ���s�t�@�C���̃p�X����f�[�^�t�@�C�������쐬
	CString tmp;
	CString fileName;
	fileName.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("user.dat"));

	// �t�@�C�������݂��Ă���΂��̃t�@�C����ǂݍ���
	FILE* fp;

	if( util::ExistFile(fileName) ) {

		// ----------------------------------------
		// �f�R�[�h����
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
	// ���s�t�@�C���̃p�X����f�[�^�t�@�C�������쐬
	CString fileName;
	fileName.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("user.dat"));

	FILE* fp;

	// ----------------------------------------
	// �G���R�[�h����
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
	// blowfish�̏�����
	char key[256];
	memset(key, 0x00, 256);
	wcstombs(key, _T("Mixi_browser_for_W-ZERO3"), 255); // �Í����L�[

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
	wcstombs(key, _T("Mixi_browser_for_W-ZERO3"), 255); // �Í����L�[

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
