// MiniImageDialog.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
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
END_MESSAGE_MAP()


// CMiniImageDialog ���b�Z�[�W �n���h��

void CMiniImageDialog::PostNcDestroy()
{
//	CDialog::PostNcDestroy();
	delete this;
}

void CMiniImageDialog::DrawImageFile(LPCTSTR szImageFile)
{
//	MessageBox( szImageFile );
	m_image.load( szImageFile );

	Invalidate( FALSE );
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

		util::DrawBitmap( dc.GetSafeHdc(), m_image.getHandle(), 0, 0, sizeSrc.cx, sizeSrc.cy, x, y, sizeDest.cx, sizeDest.cy );
	}
}
