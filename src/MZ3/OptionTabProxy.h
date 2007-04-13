#pragma once
#include "afxwin.h"


// COptionTabProxy �_�C�A���O

class COptionTabProxy : public CPropertyPage
{
  DECLARE_DYNAMIC(COptionTabProxy)

public:
  COptionTabProxy();
  virtual ~COptionTabProxy();

// �_�C�A���O �f�[�^
  enum { IDD = IDD_OPTIONTAB_PROXY };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

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

};
