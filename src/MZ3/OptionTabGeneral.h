#pragma once


// COptionTabGeneral �_�C�A���O

class COptionTabGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabGeneral)

public:
	COptionTabGeneral();
	virtual ~COptionTabGeneral();

	// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
