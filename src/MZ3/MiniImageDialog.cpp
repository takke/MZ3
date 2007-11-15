// MiniImageDialog.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3View.h"
#include "MiniImageDialog.h"
#include "util_gui.h"


// CMiniImageDialog �_�C�A���O

IMPLEMENT_DYNAMIC(CMiniImageDialog, CDialog)

CMiniImageDialog::CMiniImageDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMiniImageDialog::IDD, pParent)
	, m_image( L"" )
{
	Create(CMiniImageDialog::IDD,pParent);
}

CMiniImageDialog::~CMiniImageDialog()
{
}

void CMiniImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMiniImageDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CMiniImageDialog ���b�Z�[�W �n���h��

void CMiniImageDialog::PostNcDestroy()
{
//	CDialog::PostNcDestroy();
	delete this;
}

void CMiniImageDialog::DrawImageFile(LPCTSTR szImageFile)
{
	// �t�@�C�������ω����Ă���΃��[�h����
	// �i�����[�h�E������΍�j
	if (szImageFile != m_szLastImageFile) {
		if (m_image.load( szImageFile )) {
			m_szLastImageFile = szImageFile;
			Invalidate( TRUE );
		}
	}
}

/// �A�X�y�N�g��Œ�Ŋg��E�k��
CSize makeAspectFixedFitSize( int w0, int h0, int w1, int h1 )
{
	if (w0==0 || h0==0 || w1==0 || h1==0) {
		return CSize();
	}

	// �c,����Ŋg��E�k��
	int w = (int)(h1/(double)h0 * w0);
	int h = (int)(w1/(double)w0 * h0);

	if (w>w1) {
		return CSize(w1, h);
	} else {
		return CSize(w, h1);
	}
}

void CMiniImageDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (m_image.isEnableImage()) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		// �Ƃ肠�����h��Ԃ�
		COLORREF bgColor = RGB(255,255,255);
		dc.FillSolidRect( rectClient, bgColor );

		// �A�X�y�N�g��Œ�Ŋg��E�k��
		int x = rectClient.left;
		int y = rectClient.top;
		int w = rectClient.Width();
		int h = rectClient.Height();
		CSize sizeSrc  = m_image.getBitmapSize();

		CSize sizeDest = makeAspectFixedFitSize( sizeSrc.cx, sizeSrc.cy, w, h );

		// �Z���^�����O�`��
		x = (w - sizeDest.cx)/2;
		y = (h - sizeDest.cy)/2;

#ifdef WINCE
		util::DrawBitmap( dc.GetSafeHdc(), m_image.getHandle(), 0, 0, sizeSrc.cx, sizeSrc.cy, x, y, sizeDest.cx, sizeDest.cy );
#else
		m_image.m_gdiPlusImage.BitBlt( dc.GetSafeHdc(), x, y, sizeDest.cx, sizeDest.cy, 0, 0 );
#endif
	}
}

void CMiniImageDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	// BodyList �� WM_MOUSEMOVE �𓊂���
	ClientToScreen( &point );

	::ScreenToClient( theApp.m_pMainView->m_bodyList.m_hWnd, &point );
	::PostMessage( theApp.m_pMainView->m_bodyList.m_hWnd, WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y) );

//	CDialog::OnMouseMove(nFlags, point);
}

BOOL CMiniImageDialog::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}
