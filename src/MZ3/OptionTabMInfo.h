#pragma once


// COptionTabMInfo �_�C�A���O

class COptionTabMInfo : public CPropertyPage
{
  DECLARE_DYNAMIC(COptionTabMInfo)

public:
  COptionTabMInfo();
  virtual ~COptionTabMInfo();

// �_�C�A���O �f�[�^
  enum { IDD = IDD_OPTIONTAB_GENERAL };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

  DECLARE_MESSAGE_MAP()

public:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnBnClickedResetTabButton();
  afx_msg void OnBnClickedSelectFontButton();
};
