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
	AddPage( &m_userTab );			// ���O�C���^�u
	AddPage( &m_infoTab );			// ��ʃ^�u
	AddPage( &m_uiTab );			// UI�^�u
	AddPage( &m_proxyTab );			// �v���L�V�^�u
	AddPage( &m_pageTab );			// �擾�^�u
	AddPage( &m_logTab );			// ���O�^�u
}
