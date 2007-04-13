#pragma once


// COptionTabMInfo ダイアログ

class COptionTabMInfo : public CPropertyPage
{
  DECLARE_DYNAMIC(COptionTabMInfo)

public:
  COptionTabMInfo();
  virtual ~COptionTabMInfo();

// ダイアログ データ
  enum { IDD = IDD_OPTIONTAB_GENERAL };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

  DECLARE_MESSAGE_MAP()

public:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnBnClickedResetTabButton();
  afx_msg void OnBnClickedSelectFontButton();
};
