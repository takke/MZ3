#pragma once


// COptionTabTwitter �_�C�A���O

class COptionTabTwitter : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabTwitter)

public:
	COptionTabTwitter();
	virtual ~COptionTabTwitter();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_TWITTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
