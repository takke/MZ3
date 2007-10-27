// CategorySettingDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3GroupFileEditor.h"
#include "CategorySettingDlg.h"
#include "../MZ3/util_base.h"


// CCategorySettingDlg ダイアログ

IMPLEMENT_DYNAMIC(CCategorySettingDlg, CDialog)

CCategorySettingDlg::CCategorySettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCategorySettingDlg::IDD, pParent)
	, mc_strName(_T(""))
	, mc_strUrl(_T(""))
{

}

CCategorySettingDlg::~CCategorySettingDlg()
{
}

void CCategorySettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT, mc_strName);
	DDX_Control(pDX, IDC_TYPE_COMBO, mc_comboType);
	DDX_Text(pDX, IDC_URL_EDIT, mc_strUrl);
}


BEGIN_MESSAGE_MAP(CCategorySettingDlg, CDialog)
END_MESSAGE_MAP()


// CCategorySettingDlg メッセージ ハンドラ

BOOL CCategorySettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ロード
	mc_strName = m_item.m_name;

//	std::vector<CCategoryItem> items = CCategoryItem::MakeDefaultItems();
	Mz3GroupDataInifileHelper helper;
	std::vector<ACCESS_TYPE> types = helper.GetCategoryTypeList();
	for (int i=0; i<types.size(); i++) {
//		mc_comboType.InsertString( i, items[i].m_name );
		int idx = mc_comboType.InsertString( i, util::AccessType2Message(types[i]) );

		mc_comboType.SetItemData( idx, types[i] );
		if (types[i]==m_item.m_mixi.GetAccessType()) {
			mc_comboType.SetCurSel(idx);
		}
	}

	mc_strUrl = m_item.m_mixi.GetURL();

	CheckDlgButton( IDC_CRUISE_CHECK, m_item.m_bCruise ? BST_CHECKED : BST_UNCHECKED );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CCategorySettingDlg::OnOK()
{
	UpdateData();

	// 更新
	m_item.m_name = mc_strName;
	m_item.m_mixi.SetAccessType( (ACCESS_TYPE)mc_comboType.GetItemData(mc_comboType.GetCurSel()) );
	m_item.m_mixi.SetURL( mc_strUrl );
	m_item.m_bCruise = (IsDlgButtonChecked(IDC_CRUISE_CHECK)==BST_CHECKED) ? true : false;

	CDialog::OnOK();
}
