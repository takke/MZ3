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
	,m_iLastPage(1)
{

}

COptionSheet::~COptionSheet()
{
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// COptionSheet ���b�Z�[�W �n���h��

void COptionSheet::SetPage()
{
	AddPage( &m_tabGeneral );	// ��ʃ^�u
	AddPage( &m_tabDisplay );	// �\���^�u
	AddPage( &m_tabUi );		// ����^�u
	AddPage( &m_tabMainView );	// ���C����ʃ^�u
	AddPage( &m_tabReport );	// ���|�[�g��ʃ^�u
	AddPage( &m_tabGet );		// �擾�^�u
	AddPage( &m_tabLog );		// ���O�^�u
	AddPage( &m_tabProxy );		// �v���L�V�^�u
}

void COptionSheet::OnDestroy()
{
	CPropertySheet::OnDestroy();

	m_iLastPage = GetActiveIndex();
}
