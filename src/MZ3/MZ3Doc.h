/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// MZ3Doc.h : CMZ3Doc �N���X�̃C���^�[�t�F�C�X
//


#pragma once

class CMZ3Doc : public CDocument
{
protected: // �V���A��������̂ݍ쐬���܂��B
	CMZ3Doc();
	DECLARE_DYNCREATE(CMZ3Doc)

// ����
public:

// ����
public:

// �I�[�o�[���C�h
public:
	virtual BOOL OnNewDocument();
#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
	virtual void Serialize(CArchive& ar);
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT

// ����
public:
	virtual ~CMZ3Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	DECLARE_MESSAGE_MAP()
};


