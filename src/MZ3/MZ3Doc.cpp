/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// MZ3Doc.cpp : CMZ3Doc �N���X�̎���
//

#include "stdafx.h"
#include "MZ3.h"

#include "MZ3Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMZ3Doc

IMPLEMENT_DYNCREATE(CMZ3Doc, CDocument)

BEGIN_MESSAGE_MAP(CMZ3Doc, CDocument)
END_MESSAGE_MAP()

// CMZ3Doc �R���X�g���N�V����/�f�X�g���N�V����

CMZ3Doc::CMZ3Doc()
{
	// TODO: ���̈ʒu�� 1 �x�����Ă΂��\�z�p�̃R�[�h��ǉ����Ă��������B

}

CMZ3Doc::~CMZ3Doc()
{
}

BOOL CMZ3Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���̈ʒu�ɍď�����������ǉ����Ă��������B
	// (SDI �h�L�������g�͂��̃h�L�������g���ė��p���܂��B)

	return TRUE;
}

// CMZ3Doc �V���A����

#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
void CMZ3Doc::Serialize(CArchive& ar)
{
	(ar);
}
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT


// CMZ3Doc �f�f

#ifdef _DEBUG
void CMZ3Doc::AssertValid() const
{
	CDocument::AssertValid();
}
#endif //_DEBUG


// CMZ3Doc �R�}���h

