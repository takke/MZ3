// ChooseAccessTypeDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util_mixi.h"
#include "ChooseAccessTypeDlg.h"


// CChooseAccessTypeDlg ダイアログ

IMPLEMENT_DYNAMIC(CChooseAccessTypeDlg, CDialog)

CChooseAccessTypeDlg::CChooseAccessTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseAccessTypeDlg::IDD, pParent)
{

}

CChooseAccessTypeDlg::~CChooseAccessTypeDlg()
{
}

void CChooseAccessTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACCESS_TYPE_LIST, mc_listAccessType);
}


BEGIN_MESSAGE_MAP(CChooseAccessTypeDlg, CDialog)
	ON_LBN_DBLCLK(IDC_ACCESS_TYPE_LIST, &CChooseAccessTypeDlg::OnLbnDblclkAccessTypeList)
END_MESSAGE_MAP()


// CChooseAccessTypeDlg メッセージ ハンドラ

BOOL CChooseAccessTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ACCESS_TYPE accessTypeList[] = { 
		ACCESS_PLAIN,
		ACCESS_DIARY, 
		ACCESS_BBS, 
		ACCESS_ENQUETE, 
		ACCESS_EVENT, 
		ACCESS_MYDIARY, 
		ACCESS_MESSAGE, 
		ACCESS_NEWS, 
		ACCESS_INVALID 
	};

	for (int i=0; accessTypeList[i] != ACCESS_INVALID; i++ ) {
		int idx = mc_listAccessType.InsertString( i, util::AccessType2Message(accessTypeList[i]) );
		mc_listAccessType.SetItemData( i, accessTypeList[i] );
	}
	mc_listAccessType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CChooseAccessTypeDlg::OnOK()
{
	int idx = mc_listAccessType.GetCurSel();
	if (0 <= idx && idx < mc_listAccessType.GetCount()) {
		m_selectedAccessType = (ACCESS_TYPE) mc_listAccessType.GetItemData(idx);

	}

	CDialog::OnOK();
}

void CChooseAccessTypeDlg::OnLbnDblclkAccessTypeList()
{
	OnOK();
}
