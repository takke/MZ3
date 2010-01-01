// DetailView.cpp : �����t�@�C��
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


// CDetailView �f�f

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


// CDetailView ���b�Z�[�W �n���h��

void CDetailView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// �t�H���g�̕ύX
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
	// �O��̒l��ۑ����A(0,0) �̏ꍇ�͂��̒l�𗘗p����
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

	// ����ʃo�b�t�@��������΂����Ŋm�ۂ���
	if( m_memBMP == NULL ) {
		CDC* pDC = GetDC();
		MyMakeBackBuffers(pDC);
		ReleaseDC(pDC);
	} else {
		// ����ʃo�b�t�@�̃T�C�Y���������ꍇ�͍Đ���
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
	// �ʐM���͖���
	if (theApp.m_access) {
		return;
	}

	// �߂�
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/**
 * �\���C�x���g
 */
LRESULT CDetailView::OnFit(WPARAM wParam, LPARAM lParam)
{
	// �R���g���[����Ԃ̍X�V
//	MyUpdateControls();

	// �t�H�[�J�X�ݒ�
//	GetDlgItem(IDC_EXIT_BUTTON)->SetFocus();

	return TRUE;
}

void CDetailView::OnDraw(CDC* pDC)
{
	// �`��͈͎擾
	CRect rect;
	GetClientRect(&rect);

	//----------------------------------------------
	// �o�b�N�o�b�t�@�ɑ΂���`��
	//----------------------------------------------

	// �r�b�g�}�b�v�̏������ƕ`��
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
		// �X�e�[�^�X�o�[�̔w�i�F�ŕ`��
		m_memDC->FillRect(rect, &theApp.m_brushMainStatusBar);
	}

	// �t�H���g�ύX
	CFont* pOriginalFont = m_memDC->SelectObject(&theApp.m_font);

	// ���߃��[�h�ɐݒ�
	m_memDC->SetBkMode(TRANSPARENT);

	// Lua �ɂ��`�揈��
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

	// �t�H���g��߂�
	if (pOriginalFont!=NULL) {
		m_memDC->SelectObject(pOriginalFont);
		pOriginalFont = NULL;
	}

	//----------------------------------------------
	// ��ʂɓ]��
	//----------------------------------------------
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), m_memDC, 0, 0, SRCCOPY);

}

BOOL CDetailView::OnEraseBkgnd(CDC* pDC)
{
	MZ3_TRACE( L"CDetailView::OnEraseBkgnd()\n");

/*	pDC->SetBkMode( TRANSPARENT );

	CRect rectClient;
	this->GetClientRect( &rectClient );

	// �r�b�g�}�b�v�̏������ƕ`��
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

	// �X�e�[�^�X�o�[�̔w�i�F�ŕ`��
	m_memDC->FillRect(rectClient, &theApp.m_brushMainStatusBar);

	//----------------------------------------------
	// ��ʂɓ]��
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

/// �E�N���b�N���j���[
bool CDetailView::PopupDetailMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	MZ3Data& data = m_data;

	// MZ3 API : �t�b�N�֐��Ăяo��
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
 * ����ʃo�b�t�@�̐���
 */
bool CDetailView::MyMakeBackBuffers(CDC* pdc)
{
	//--- ���
	// ����ʃo�b�t�@�̉��
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
	
	//--- �o�b�t�@����
	// ����ʃo�b�t�@�̊m��
	CRect rect;
	GetWindowRect(&rect);
	if (m_memBMP->CreateCompatibleBitmap( pdc , rect.Width(), rect.Height() ) != TRUE) {
		CString msg;
		msg.Format(TEXT("CreateCompatibleBitmap error! w[%d], h[%d]"), rect.Width(), rect.Height());
		MZ3LOGGER_FATAL(msg);
		return false;
	}

	// DC�𐶐�
	m_memDC->CreateCompatibleDC(pdc);
	m_memDC->SetBkMode(OPAQUE);					// ���߃��[�h�ɐݒ肷��
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	return true;
}

void CDetailView::OnDestroy()
{
	CFormView::OnDestroy();

	//--- ���
	// ����ʃo�b�t�@�̉��
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
		delete m_memDC;
	}

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
}
