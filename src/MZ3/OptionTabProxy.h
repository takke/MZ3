#pragma once
#include "afxwin.h"

// this file is not used in smartphone edition
#ifndef SMARTPHONE2003_UI_MODEL


// COptionTabProxy ダイアログ

class COptionTabProxy : public CPropertyPage
{
  DECLARE_DYNAMIC(COptionTabProxy)

public:
  COptionTabProxy();
  virtual ~COptionTabProxy();

// ダイアログ データ
  enum { IDD = IDD_OPTIONTAB_PROXY };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

  DECLARE_MESSAGE_MAP()

public:
  void Load();
  void Save();

public:
  virtual BOOL OnInitDialog();
  virtual void OnOK();

private:
  CButton m_useCheck;
  CEdit m_serverEdit;
  CEdit m_portEdit;
  CEdit m_userEdit;
  CEdit m_passwordEdit;

public:
	bool DoUpdateControlViewStatus(void);
	afx_msg void OnBnClickedProxyUseCheck();
	afx_msg void OnBnClickedUseGlobalProxyRadio();
	afx_msg void OnBnClickedUseManualProxyRadio();
};

#endif