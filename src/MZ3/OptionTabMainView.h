#pragma once


// COptionTabMainView �_�C�A���O

class COptionTabMainView : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabMainView)

public:
	COptionTabMainView();
	virtual ~COptionTabMainView();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_MAIN_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CComboBox mc_comboMiniImageSize;
};