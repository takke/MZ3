// OpenUrlDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OpenUrlDlg.h"


// COpenUrlDlg �_�C�A���O

IMPLEMENT_DYNAMIC(COpenUrlDlg, CDialog)

COpenUrlDlg::COpenUrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenUrlDlg::IDD, pParent)
	, mc_strUrl(_T(""))
{

}

COpenUrlDlg::~COpenUrlDlg()
{
}

void COpenUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_URL_EDIT, mc_strUrl);
}


BEGIN_MESSAGE_MAP(COpenUrlDlg, CDialog)
END_MESSAGE_MAP()


// COpenUrlDlg ���b�Z�[�W �n���h��
