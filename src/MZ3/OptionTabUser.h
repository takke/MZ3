#pragma once

// this file is not used in smartphone edition
#ifndef SMARTPHONE2003_UI_MODEL


// COptionTabUser �_�C�A���O

class COptionTabUser : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabUser)

public:
	COptionTabUser();
	virtual ~COptionTabUser();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

#endif