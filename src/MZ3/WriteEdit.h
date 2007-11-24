#pragma once


// CWriteEdit

class CWriteEdit : public CEdit
{
	DECLARE_DYNAMIC(CWriteEdit)

public:
	CWriteEdit();
	virtual ~CWriteEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


