// MiniImageDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3View.h"
#include "MiniImageDialog.h"
#include "util_gui.h"


// CMiniImageDialog ダイアログ

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


// CMiniImageDialog メッセージ ハンドラ

void CMiniImageDialog::PostNcDestroy()
{
//	CDialog::PostNcDestroy();
	delete this;
}

void CMiniImageDialog::DrawImageFile(LPCTSTR szImageFile)
{
	// ファイル名が変化していればロードする
	// （リロード・ちらつき対策）
	if (szImageFile != m_szLastImageFile) {
		if (m_image.load( szImageFile )) {
			m_szLastImageFile = szImageFile;
			Invalidate( TRUE );
		}
	}
}

/// アスペクト比固定で拡大・縮小
CSize makeAspectFixedFitSize( int w0, int h0, int w1, int h1 )
{
	if (w0==0 || h0==0 || w1==0 || h1==0) {
		return CSize();
	}

	// 縦,横基準で拡大・縮小
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

		// とりあえず塗りつぶす
		COLORREF bgColor = RGB(255,255,255);
		dc.FillSolidRect( rectClient, bgColor );

		// アスペクト比固定で拡大・縮小
		int x = rectClient.left;
		int y = rectClient.top;
		int w = rectClient.Width();
		int h = rectClient.Height();
		CSize sizeSrc  = m_image.getBitmapSize();

		CSize sizeDest = makeAspectFixedFitSize( sizeSrc.cx, sizeSrc.cy, w, h );

		// センタリング描画
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
	// BodyList に WM_MOUSEMOVE を投げる
	ClientToScreen( &point );

	::ScreenToClient( theApp.m_pMainView->m_bodyList.m_hWnd, &point );
	::PostMessage( theApp.m_pMainView->m_bodyList.m_hWnd, WM_MOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y) );

//	CDialog::OnMouseMove(nFlags, point);
}

BOOL CMiniImageDialog::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}
