/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once


// CChooseClientTypeDlg �_�C�A���O

class CChooseClientTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CChooseClientTypeDlg)

public:
	CChooseClientTypeDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CChooseClientTypeDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_CHOOSE_CLIENT_TYPE_DIALOG };

	Mz3GroupData::InitializeType m_initType;

	CListCtrl mc_serviceList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
