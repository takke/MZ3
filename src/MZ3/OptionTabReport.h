#pragma once


// COptionTabReport �_�C�A���O

class COptionTabReport : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabReport)

public:
	COptionTabReport();
	virtual ~COptionTabReport();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
