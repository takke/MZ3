// OptionSheet.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionSheet.h"


// COptionSheet

IMPLEMENT_DYNAMIC(COptionSheet, CPropertySheet)

COptionSheet::COptionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

COptionSheet::COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

COptionSheet::~COptionSheet()
{
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
END_MESSAGE_MAP()


// COptionSheet ���b�Z�[�W �n���h��

void COptionSheet::SetPage()
{
	AddPage( &m_userTab );			// ���A�h�A�p�X���[�h
	AddPage( &m_infoTab );
	AddPage( &m_proxyTab );			// �v���L�V
	AddPage( &m_pageTab );			// �f�[�^�擾
}
