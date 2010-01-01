// DetailView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "DetailView.h"
#include "MainFrm.h"
#include "util.h"
#include "util_gui.h"


// CDetailView

IMPLEMENT_DYNCREATE(CDetailView, CFormView)

CDetailView::CDetailView()
	: CFormView(CDetailView::IDD)
	, m_memBMP(NULL)
	, m_memDC(NULL)
{

}

CDetailView::~CDetailView()
{
}

void CDetailView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDetailView, CFormView)
	ON_WM_SIZE()
    ON_MESSAGE(WM_MZ3_FIT, OnFit)
//	ON_BN_CLICKED(IDC_EXIT_BUTTON, &CDetailView::OnBnClickedExitButton)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDetailView 診断

#ifdef _DEBUG
void CDetailView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDetailView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDetailView メッセージ ハンドラ

void CDetailView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// フォントの変更
	SetFont( &theApp.m_font );

/*	CWnd* pItem = GetDlgItem(IDC_EXIT_BUTTON);
	if (pItem) {
//		pItem->SetFocus();
		pItem->SetFont( &theApp.m_font );
	}
*/
}

void CDetailView::OnSize(UINT nType, int cx, int cy)
{
	// 前回の値を保存し、(0,0) の場合はその値を利用する
	static int s_cx = 0;
	static int s_cy = 0;
	if (cx==0 && cy==0) {
		cx = s_cx;
		cy = s_cy;
	} else {
		s_cx = cx;
		s_cy = cy;
	}

	CFormView::OnSize(nType, cx, cy);

	int fontHeight = theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI());
	if( fontHeight == 0 ) {
		fontHeight = 12;
	}

	// 裏画面バッファが無ければここで確保する
	if( m_memBMP == NULL ) {
		CDC* pDC = GetDC();
		MyMakeBackBuffers(pDC);
		ReleaseDC(pDC);
	} else {
		// 裏画面バッファのサイズが小さい場合は再生成
		BITMAP bmp;
		GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
		if (bmp.bmWidth < cx ||
			bmp.bmHeight < cy) 
		{
			CDC* pDC = GetDC();
			MyMakeBackBuffers(pDC);
			ReleaseDC(pDC);
		}
	}
}

void CDetailView::OnBnClickedExitButton()
{
	// 通信中は無効
	if (theApp.m_access) {
		return;
	}

	// 戻る
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/**
 * 表示イベント
 */
LRESULT CDetailView::OnFit(WPARAM wParam, LPARAM lParam)
{
	// コントロール状態の更新
//	MyUpdateControls();

	// フォーカス設定
//	GetDlgItem(IDC_EXIT_BUTTON)->SetFocus();

	return TRUE;
}

void CDetailView::OnDraw(CDC* pDC)
{
	// 描画範囲取得
	CRect rect;
	GetClientRect(&rect);

	//----------------------------------------------
	// バックバッファに対する描画
	//----------------------------------------------

	// ビットマップの初期化と描画
	bool bBgCleared = false;
	if( theApp.m_optionMng.IsUseBgImage() ) {
		theApp.m_bgImageMainBodyCtrl.load();

		if (theApp.m_bgImageMainBodyCtrl.isEnableImage()) {
			int x = rect.left;
			int y = rect.top;
			int w = rect.Width();
			int h = rect.Height();
			util::DrawBitmap( m_memDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y );
			bBgCleared = true;
		}
	}
	if (!bBgCleared) {
		// ステータスバーの背景色で描画
		m_memDC->FillRect(rect, &theApp.m_brushMainStatusBar);
	}

	// フォント変更
	CFont* pOriginalFont = m_memDC->SelectObject(&theApp.m_font);

	// 透過モードに設定
	m_memDC->SetBkMode(TRANSPARENT);

	// Lua による描画処理
	util::MyLuaDataList rvals;
	if (util::CallMZ3ScriptHookFunctions2("draw_detail_view", &rvals, 
			util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_data.GetAccessType())),
			util::MyLuaData((void*)&m_data),
			util::MyLuaData((void*)m_memDC),
			util::MyLuaData(rect.Width()),
			util::MyLuaData(rect.Height())
			))
	{
	}

	// フォントを戻す
	if (pOriginalFont!=NULL) {
		m_memDC->SelectObject(pOriginalFont);
		pOriginalFont = NULL;
	}

	//----------------------------------------------
	// 画面に転送
	//----------------------------------------------
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), m_memDC, 0, 0, SRCCOPY);

}

BOOL CDetailView::OnEraseBkgnd(CDC* pDC)
{
	MZ3_TRACE( L"CDetailView::OnEraseBkgnd()\n");

/*	pDC->SetBkMode( TRANSPARENT );

	CRect rectClient;
	this->GetClientRect( &rectClient );

	// ビットマップの初期化と描画
	if( theApp.m_optionMng.IsUseBgImage() ) {

		theApp.m_bgImageMainBodyCtrl.load();

		if (theApp.m_bgImageMainBodyCtrl.isEnableImage()) {
			int x = rectClient.left;
			int y = rectClient.top;
			int w = rectClient.Width();
			int h = rectClient.Height();
			util::DrawBitmap( m_memDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y );
			return TRUE;
		}
	}

	// ステータスバーの背景色で描画
	m_memDC->FillRect(rectClient, &theApp.m_brushMainStatusBar);

	//----------------------------------------------
	// 画面に転送
	//----------------------------------------------
	pDC->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), m_memDC, 0, 0, SRCCOPY);
*/
	return TRUE;
}

BOOL CDetailView::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_KEYDOWN:
		{
			util::MyLuaDataList rvals;
			util::CallMZ3ScriptHookFunctions2("keydown_detail_view", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_data.GetAccessType())),
				util::MyLuaData((void*)&m_data),
				util::MyLuaData(pMsg->wParam)
				);
		}
		break;

	case WM_LBUTTONDOWN:
		{
			int x = LOWORD(pMsg->lParam);
			int y = HIWORD(pMsg->lParam);
			CRect rect;
			GetWindowRect(&rect);
			int cx = rect.Width();
			int cy = rect.Height();
			util::MyLuaDataList rvals;
			util::CallMZ3ScriptHookFunctions2("click_detail_view", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_data.GetAccessType())),
				util::MyLuaData((void*)&m_data),
				util::MyLuaData(x),
				util::MyLuaData(y),
				util::MyLuaData(cx),
				util::MyLuaData(cy)
				);
		}
		break;

	case WM_RBUTTONDOWN:
		{
			int x = LOWORD(pMsg->lParam);
			int y = HIWORD(pMsg->lParam);
			CRect rect;
			GetWindowRect(&rect);
			int cx = rect.Width();
			int cy = rect.Height();
			util::MyLuaDataList rvals;
			util::CallMZ3ScriptHookFunctions2("rclick_detail_view", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_data.GetAccessType())),
				util::MyLuaData((void*)&m_data),
				util::MyLuaData(x),
				util::MyLuaData(y)
				);
		}
		break;

#ifndef WINCE
	case WM_MOUSEWHEEL:
		{
			int x = LOWORD(pMsg->lParam);
			int y = HIWORD(pMsg->lParam);
			int zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
			int vkey   = GET_KEYSTATE_WPARAM(pMsg->wParam);
			CRect rect;
			GetWindowRect(&rect);
			int cx = rect.Width();
			int cy = rect.Height();
			util::MyLuaDataList rvals;
			util::CallMZ3ScriptHookFunctions2("mousewheel_detail_view", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_data.GetAccessType())),
				util::MyLuaData((void*)&m_data),
				util::MyLuaData(zDelta),
				util::MyLuaData(vkey),
				util::MyLuaData(x),
				util::MyLuaData(y)
				);
		}
		break;
#endif
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/// 右クリックメニュー
bool CDetailView::PopupDetailMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	MZ3Data& data = m_data;

	// MZ3 API : フック関数呼び出し
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(data.GetAccessType()));
	if (util::CallMZ3ScriptHookFunctions2("popup_detail_menu", &rvals, 
			util::MyLuaData(serializeKey), 
			util::MyLuaData(&data), 
			util::MyLuaData(this)))
	{
		int rval = rvals[0].m_number;
		return rval!=0 ? true : false;
	}

	return true;
}

/**
 * 裏画面バッファの生成
 */
bool CDetailView::MyMakeBackBuffers(CDC* pdc)
{
	//--- 解放
	// 裏画面バッファの解放
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
		delete m_memDC;
	}
	m_memDC = new CDC();

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
	m_memBMP = new CBitmap();
	
	//--- バッファ生成
	// 裏画面バッファの確保
	CRect rect;
	GetWindowRect(&rect);
	if (m_memBMP->CreateCompatibleBitmap( pdc , rect.Width(), rect.Height() ) != TRUE) {
		CString msg;
		msg.Format(TEXT("CreateCompatibleBitmap error! w[%d], h[%d]"), rect.Width(), rect.Height());
		MZ3LOGGER_FATAL(msg);
		return false;
	}

	// DCを生成
	m_memDC->CreateCompatibleDC(pdc);
	m_memDC->SetBkMode(OPAQUE);					// 透過モードに設定する
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	return true;
}

void CDetailView::OnDestroy()
{
	CFormView::OnDestroy();

	//--- 解放
	// 裏画面バッファの解放
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
		delete m_memDC;
	}

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
}
