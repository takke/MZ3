#if !defined(AFX_RAN2IMAGE_H__185C28F5_417B_4C7B_8F56_3015E3F45646__INCLUDED_)
#define AFX_RAN2IMAGE_H__185C28F5_417B_4C7B_8F56_3015E3F45646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WINCE
	#include "MZ3.h"
	#include <gdiplus.h>
	using namespace Gdiplus;

class Ran2Image : public Image
{
public:
	Ran2Image(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE);
	~Ran2Image();
public:
	void	Draw(CDC* pDC);
	CSize	GetSize();

	bool	IsAnimatedGIF(){ return(m_nFrameCount > 1); }
	void	SetPause(bool bPause);
	bool	IsPaused(){ return(m_bPause); }

	bool	InitAnimation(Graphics* graphics, CPoint pt,int lineHeight);
	bool	PrepareAnimation();
	void	Initialize();
	bool	DrawFrameGIF();
	void	Destroy();
	void	AnimationThread();

	static UINT WINAPI _GoRan2Proc(LPVOID pParam);

	int				charHeight;
	HANDLE			m_hThread;
	HANDLE			m_hPause;
	HANDLE			m_hExitEvent;
	HINSTANCE		m_hInst;
	UINT			m_nFrameCount;
	UINT			m_nFramePosition;
	bool			m_isInitialized;
	bool			m_bPause;
	PropertyItem*	m_pPropertyItem;
	CPoint			m_pt;
	Graphics*		m_graphics;
};
#endif	// #ifndef WINCE


#endif // !defined(AFX_RAN2IMAGE_H__185C28F5_417B_4C7B_8F56_3015E3F45646__INCLUDED_)
