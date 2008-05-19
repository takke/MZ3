/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

/*
 * TouchListCtrl.cpp : CTouchListCtrl�N���X����
 */
#include "StdAfx.h"
#include "TouchListCtrl.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "MZ3View.h"

IMPLEMENT_DYNAMIC(CTouchListCtrl, CListCtrl)

CTouchListCtrl::CTouchListCtrl(void)
	: m_offsetPixelY(0)
	, m_bDragging(false)
#ifndef WINCE
	, m_memBMP(NULL)
	, m_memDC(NULL)
#endif
	, m_bTimerRedraw(false)
	, m_bDrawBk(true)
{
}

CTouchListCtrl::~CTouchListCtrl(void)
{
}

BEGIN_MESSAGE_MAP(CTouchListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
//#ifndef WINCE
	ON_WM_VSCROLL()
//#endif
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, &CTouchListCtrl::OnLvnDeleteallitems)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CTouchListCtrl::OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, &CTouchListCtrl::OnLvnInsertitem)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/**
 * PreTranslateMessage
 *  �E�g���u���V���[�g�p
 */
BOOL CTouchListCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_MOUSEMOVE:
		break;

	case WM_PAINT:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_PAINT, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_MOUSEWHEEL:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_MOUSEWHEEL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_VSCROLL:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_VSCROLL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
//	case WM_PAINT:
		// ������h�~�̂��ߎՒf
//		return TRUE;

	default:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(0x%04X, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

/**
 * WM_LBUTTONDOWN ���b�Z�[�W�n���h��
 *  �E�h���b�O���J�n����
 */
void CTouchListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	MZ3_TRACE( L"OnLButtonDown()\n");

	// �t�H�[�J�X��ݒ肷��
	SetFocus();

	// �I�t�Z�b�g�̂���𒲐�
	MyAdjustDrawOffset();
	// �����X�N���[����~
	MyResetAutoScrollTimer();

#ifdef WINCE
	// �^�b�v�������Ń\�t�g�L�[���j���[�\��
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		ClientToScreen(&point);
		PopupContextMenu(point);
		return;
	}
#endif

	// �h���b�O�J�n
	m_bDragging = true;
	m_ptDragStart = point;
	m_iDragLine = 0;
	m_offsetPixelY = 0;

	// �L���v�`���J�n
	SetCapture();

	// �����X�N���[�����擾
	m_autoScrollInfo.push( GetTickCount(), point );

	// �W���������ĂԂƃL���v�`���Ƃ�WM_LBUTTONUP���������̂ŌĂ΂Ȃ�
	// �Ƃ肠�����N���b�N�����Ƃ���
	//  �E�A�C�e���̑I��
	//  �E�t�H�[�J�X�̐ݒ�
	//  �E�N���b�N�ʒm���b�Z�[�W��e�ɑ���
	// ������Ă邪�A����Ȃ�������ǉ����܂���
	//CListCtrl::OnLButtonDown(nFlags, point);
}

/**
 * WM_LBUTTONUP ���b�Z�[�W�n���h��
 *  �E�h���b�O���Ȃ�΃h���b�O���I������
 */
void CTouchListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	MZ3_TRACE( L"OnLButtonUp(0x%X,%d,%d)\n", nFlags, point.x, point.y);

	if (m_bDragging) {
		// �h���b�O�I������

		// �L���v�`���I��
		ReleaseCapture();
		// �t���O�N���A
		m_bDragging = false;

		// �x���`��^�C�}�[�̃��Z�b�g
		MyResetRedrawTimer();

		// �}�E�X�J�[�\�������ɖ߂�
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );

		// �N���b�N�ʒm���b�Z�[�W��e�ɑ���
		int nItem = util::MyGetListCtrlSelectedItemIndex( *this );
		NMLISTVIEW nmlv;
		nmlv.hdr.hwndFrom = m_hWnd;
		nmlv.hdr.idFrom = 0 ;
		nmlv.hdr.code = NM_CLICK ;
		nmlv.iItem = nItem ;
		nmlv.iSubItem = 0 ;
		nmlv.ptAction = point ;
		nmlv.uChanged = 0;
		nmlv.uNewState = 0;
		nmlv.uOldState = LVIS_FOCUSED | LVIS_SELECTED;
		nmlv.lParam = NULL;
		GetParent()->SendMessage( WM_NOTIFY , NM_CLICK , (LPARAM)&nmlv );

		// �X�N���[���\���H
		if ( GetItemCount() > GetCountPerPage() ) {
//#ifndef WINCE
//			// Win32�ł͓Ǝ������ŕ`�悷��
//			// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
//			DrawDetail();
//			UpdateWindow();
//#else
//#ifndef TOUCHLIST_SCROLLWITHBK
//			// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͍ĕ`��
//			Invalidate();
//#endif
//#endif

			// �����X�N���[�����擾
			m_autoScrollInfo.push( GetTickCount(), point );
			double speed = m_autoScrollInfo.calcMouseMoveSpeedY();
			MZ3_TRACE( L"! speed   : %5.3f [px/msec]\n", speed );
			if( abs(point.y - m_ptDragStart.y) > m_iItemHeight ){
				// 1�s�ȏ�h���b�O���Ă���Ί����X�N���[���J�n
				m_dwAutoScrollStartTick = GetTickCount();
				m_yAutoScrollMax = 0;
				MySetAutoScrollTimer( TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL );
				return;
			} else {
				// 1�s�����̃h���b�O�Ȃ�΂����Ɏ~�߂�
				MyAdjustDrawOffset();
				m_autoScrollInfo.clear();
			}
		}
	}
	//CListCtrl::OnLButtonUp(nFlags, point);
}

/**
 * WM_MOUSEMOVE ���b�Z�[�W�n���h��
 *  �E�h���b�O���Ȃ�Γ��I�X�N���[���������s��
 */
void CTouchListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging ) {
		// �X�N���[���\���H
		if ( GetItemCount() > GetCountPerPage() ) {
			// �X�N���[���\
			// �O�[�̃J�[�\���ɕύX
			::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );

			// �c�h���b�O����
			ScrollByMoveY( point.y );
#ifdef WINCE
#ifndef TOUCHLIST_SCROLLWITHBK
			// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͒x���ĕ`��
			MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW );
#endif
#endif
			// �����X�N���[�����擾
			m_autoScrollInfo.push( GetTickCount(), point );
		} else {
			::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		}

	}

	//CListCtrl::OnMouseMove(nFlags, point);
}

/**
 * WM_RESIZE ���b�Z�[�W�n���h��
 *	�E�`��̈�̑傫���i�N���C�A���g�̈悩��w�b�_�̈�������j�̎擾
 *  �E����ʃo�b�t�@�̊m��
 */
void CTouchListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	CRect rctItem;								// 0�Ԗڂ̃A�C�e���̗̈�
	CRect rctHeader;							// �w�b�_�̈�

	// �N���C�A���g�̈���擾����
	GetClientRect( &m_viewRect );
	// �w�b�_�̈���擾����
	GetHeaderCtrl()->GetWindowRect( &rctHeader );

	// �`��̈��top���w�b�_�̍������k�߂�
	ScreenToClient( &rctHeader );
	m_viewRect.top = rctHeader.bottom ;

	// �A�C�e��1�̍������擾����
	if( GetItemCount() > 0 ){
		// �A�C�e������ł�����΍������擾����
		GetItemRect( 0 , &rctItem , LVIR_BOUNDS);
		m_iItemHeight = rctItem.Height() ;
	} else {
		// �Ȃ����͎擾���Ȃ��i�����̂��H
		m_iItemHeight = 1;
	}

	// �`��̈�̕��ƍ����������o�ϐ��ɐݒ肷��
	m_screenWidth = cx;
	m_screenHeight = cy - rctHeader.Height();

#ifndef WINCE
	// ����ʃo�b�t�@��������΂����Ŋm�ۂ���
	if( m_memBMP == NULL ) {
		CPaintDC	cdc(this);
		MyMakeBackBuffers(cdc);
	} else {
		// ����ʃo�b�t�@�̃T�C�Y���������ꍇ�͍Đ���
		BITMAP bmp;
		GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
		if (bmp.bmWidth < m_screenWidth ||
			bmp.bmHeight < m_screenHeight*2) 
		{
			CPaintDC	cdc(this);
			MyMakeBackBuffers(cdc);
		}
	}
#endif
}

#ifndef WINCE
/**
 * ����ʃo�b�t�@�̐���
 */
bool CTouchListCtrl::MyMakeBackBuffers(CPaintDC& cdc)
{
	//--- ���
	// ����ʃo�b�t�@�̉��
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
	}
	m_memDC = new CDC();

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
	m_memBMP = new CBitmap();
	
	//--- �o�b�t�@����
	// ����ʃo�b�t�@�̊m��
	// ��ʂ̍�����2�{���ė]�T���������Ă݂�
	if (m_memBMP->CreateCompatibleBitmap( &cdc , m_screenWidth , m_screenHeight*2 ) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}
	// ���ۂɕ`�悷��̈�̐擪�I�t�Z�b�g
	m_drawStartTopOffset = m_screenHeight/2;

	// DC�𐶐�
	m_memDC->CreateCompatibleDC(&cdc);
	m_memDC->SetBkMode(OPAQUE);					// ���߃��[�h�ɐݒ肷��
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	return true;
}
#endif

/**
 * WM_DESTROY ���b�Z�[�W�n���h��
 *  �E����ʃo�b�t�@�̉��
 */
void CTouchListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();
#ifndef WINCE
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
#endif
}

#ifndef WINCE
/**
 * DrawToScreen() �`��
 *  ����ʃo�b�t�@�����ʕ����f�o�C�X�֓]��
 */
void CTouchListCtrl::DrawToScreen(CDC* pDC)
{
	MZ3_TRACE( L"DrawToScreen(0x%X)\n", pDC!=NULL ? pDC->m_hDC : 0);

	// �ύX���ʂ�m_offsetPixelY�����炵�ĕ\������
	pDC->BitBlt( 
		0				, m_viewRect.top,
		m_screenWidth	, m_screenHeight, 
		m_memDC,
		0				, m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY,
		SRCCOPY );
}
#endif

#ifndef WINCE
/**
 * DrawDetail()
 *  ����ʃo�b�t�@�Ƀ��X�g���ڂ�`�悷��
 */
int	CTouchListCtrl::DrawDetail( bool bForceDraw )
{
	MZ3_TRACE( L"DrawDetail(%s)\n", bForceDraw ? L"true" : L"false");

	// �O�̂��ߒx���`��^�C�}�[���~����
	MyResetRedrawTimer();

	// �w�i��h��Ԃ�
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	HBITMAP hBgBitmap = GetBgBitmapHandle();
	if( !theApp.m_optionMng.IsUseBgImage() || hBgBitmap == NULL ) {
		// �w�i�摜�Ȃ��̏ꍇ
		m_memDC->FillSolidRect( 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255,255,255) );
	}else{
		// �w�i�r�b�g�}�b�v�̕`��
		CRect rectViewClient;
		this->GetClientRect( &rectViewClient );
		rectViewClient.OffsetRect( 0 , m_drawStartTopOffset-m_offsetPixelY );
		int x = rectViewClient.left;
		int y = rectViewClient.top;
		int w = rectViewClient.Width();
		int h = rectViewClient.Height() + m_iItemHeight * 2;
#ifdef TOUCHLIST_SCROLLWITHBK
		int offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
#else
		int offset = 0;
#endif
		util::DrawBitmap( m_memDC->GetSafeHdc(), hBgBitmap, x, y, w, h, 0, 0 + offset );
	}

	MZ3_TRACE( L"Top=%5d,offset=%5d\n" , GetTopIndex() , m_offsetPixelY );

	// ���߃��[�h�ɐݒ�
	m_memDC->SetBkMode(TRANSPARENT);

	// �t�H���g�����݂̃t�H���g�ɒu������
	CFont* oldFont = (CFont*)m_memDC->SelectObject( GetFont() );

	// ��ʂɎ��܂�Item�����Ԃɕ`�悷��
	// �I�t�Z�b�g�X�N���[���p��1�s�]���ɕ`�悷��B
	const int N_OVER_OFFSET_LINES = 1;
	for(int i=-N_OVER_OFFSET_LINES; i <= GetCountPerPage()+N_OVER_OFFSET_LINES ; i++){
		int nItem = GetTopIndex() + i;

		// �͈͂��z������X���[
		if (nItem < 0) {
			continue;
		}
		if( GetItemCount() <= nItem ){
			break;
		}

		// DRAWITEMSTRUCT���ł��������Ĕh���N���X��DrawItem()�����܂�
		DRAWITEMSTRUCT dis;
		CRect rctItem;
		GetItemRect( nItem , &rctItem , LVIR_BOUNDS );
		// �`��̈�̓o�b�t�@�擪����I�t�Z�b�g�����ʒu�ɂ���
		rctItem.OffsetRect( 0 , m_drawStartTopOffset );

		dis.CtlType		= ODT_LISTVIEW;
		dis.CtlID		= 0;
		dis.itemAction	= ODA_DRAWENTIRE;
		dis.hwndItem	= m_hWnd;
		dis.hDC			= m_memDC->GetSafeHdc();
		dis.rcItem		= rctItem;
		dis.itemID		= nItem;
		dis.itemData	= GetItemData( nItem );

		// �w�i�͕`��ς݂Ȃ̂ŕ`�悵�Ȃ�
		// �iDrawItem()�Ŕw�i�`�悳���ƁA�Y����������̂Łj
		SetDrawBk( false );

		// �{���̃A�C�e���`����g�p���邱�ƂŎ��ۂ̉�ʂƕ`����e����v������
		DrawItem( &dis );

		SetDrawBk( true );
	}

	// �t�H���g�����ɖ߂�
	m_memDC->SelectObject( oldFont );

	// ��ʂɓ]������
	if (bForceDraw) {
		// �����I�ɕ`�悷��
		CDC* pDC = GetDC();
		DrawToScreen(pDC);
		ReleaseDC(pDC);

		ValidateRect( &m_viewRect );
	}

	return(1);
}
#endif

/**
 * WM_VSCROLL ���b�Z�[�W�n���h��
 * (�c�X�N���[���o�[�ɂ��X�N���[��)
 *  �E��������Ȃ������߂ɗ���ʃo�b�t�@�ɂ��`����s��
 */
void CTouchListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	MZ3_TRACE( L"OnVScroll(%d)\n", nPos );

	// �I�t�Z�b�g�̃Y���𒲐�
	MyAdjustDrawOffset();
	// �����X�N���[����~
	MyResetAutoScrollTimer();
	
	// �X�N���[�����̕`����֎~
#ifndef WINCE
	LockWindowUpdate();
#endif

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

	// �X�N���[�����̕`����ĊJ
#ifndef WINCE
	UnlockWindowUpdate();
#endif

#ifndef WINCE
	// Win32�ł͓Ǝ������ŕ`�悷��
	// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
	DrawDetail();
	UpdateWindow();
#else
#ifndef TOUCHLIST_SCROLLWITHBK
	// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͒x���ĕ`��
	MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW );
#endif
#endif
}

/**
 * WM_MOUSEWHEEL ���b�Z�[�W�n���h��
 * (�}�E�X�z�C�[���ɂ��N���[��)
 *  �E��������Ȃ������߂ɗ���ʃo�b�t�@�ɂ��`����s��
 *   �������A���̎��_�ŃX�N���[���ʒu���m�肵�Ă��Ȃ��̂Œx���`����s��
 */
BOOL CTouchListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// �I�t�Z�b�g�̃Y���𒲐�
	MyAdjustDrawOffset();
	// �����X�N���[����~
	MyResetAutoScrollTimer();

	// �x���`����s��
	// �{���̓X�N���[���ʒu���m�肷�郁�b�Z�[�W�ŕ`�揈�����s���ׂ�
	if( !m_bTimerRedraw ){
		// �`����~
#ifndef WINCE
		LockWindowUpdate();
#else
		SetRedraw( FALSE );
#endif
		MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW );
	}

	MZ3_TRACE( L"CTouchListCtrl::OnMouseWheel( %5d ),Top=%5d\n", zDelta, GetTopIndex());
	//return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
	// �E�N���b�N�{�}�E�X�z�C�[�������̂��߂ɐe�Ăяo��
	return theApp.m_pMainView->OnMouseWheel(nFlags, zDelta, pt);
}

/**
 * WM_TIMER ���b�Z�[�W�n���h��
 *  �ETIMERID_TOUCHLIST_SCROLLREDRAW
 *    �x���`�揈��
 *  �ETIMERID_TOUCHLIST_AUTOSCROLL
 *    �����X�N���[������
 */
void CTouchListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	MZ3_TRACE( L"**OnTimer(0x%04X)\n", nIDEvent );
	switch( nIDEvent ){
/****************************************************
*			 �x���`�揈��
****************************************************/
		case TIMERID_TOUCHLIST_SCROLLREDRAW:
			// �x���`�揈��
			// �x���`��^�C�}�[���~
			MyResetRedrawTimer();

			// �`����ĊJ
#ifndef WINCE
			UnlockWindowUpdate();
#else
			SetRedraw( TRUE );
#endif
#ifndef WINCE
			// Win32�ł͓Ǝ������ŕ`�悷��
			// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
			DrawDetail();
			UpdateWindow();
#else
			Invalidate();
			UpdateWindow();
#endif
			break;

/****************************************************
*			 �����X�N���[������
****************************************************/
		case TIMERID_TOUCHLIST_AUTOSCROLL:
			{
				// �����X�N���[��
				// �����r���[�̊ۃp�N��

				// �x���`��^�C�}�[���~
				MyResetRedrawTimer();

				// ���z�I�Ȉړ��ʎZ�o
				int dt = GetTickCount() - m_dwAutoScrollStartTick;
				MZ3_TRACE( L"*************AUTOSCROLL:dt=%d\n", dt );

				// �[���I�ȃ}�C�i�X�̉����x�Ƃ���B
#ifdef WINCE
				double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.06;	// �}�C�i�X�̉����x, �X�P�[�����O�͓K����
#else
				double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.06;	// �}�C�i�X�̉����x, �X�P�[�����O�͓K����
#endif
				double speed = m_autoScrollInfo.calcMouseMoveSpeedY();

				int dyByAccel    = (int)((accel*dt*dt)/2.0);		// �}�C�i�X�̉����x
				int dyByVelocity = (int)(dt * speed);				// �����ɂ��ړ�	
				int dyAutoScroll = dyByAccel + dyByVelocity;		// LButtonUp ����̈ړ���

				MZ3_TRACE( L"*************AUTOSCROLL:dyAutoScroll=%d\n", dyAutoScroll );
				MZ3_TRACE( L"*************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );

				// �ő�ʒu���߂����i�ɓ_�𒴂����j�A
				// �����x���������l��菬�����A
				// �܂���N�b�o�߂����Ȃ�I��
				if (speed == 0.0 ||
					(speed < 0 && dyAutoScroll > m_yAutoScrollMax) ||
					(speed > 0 && dyAutoScroll < m_yAutoScrollMax) ||
					(fabs(accel)<0.00005) ||
					dt > 5 * 1000)
				{
					if( m_offsetPixelY != 0 ){
						// �I�t�Z�b�g�̃Y���𒲐�
						MyAdjustDrawOffset();
					} else {
						// ��~��ԂōŌ�̕`��
#ifndef WINCE
						// Win32�ł͓Ǝ������ŕ`�悷��
						// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
						DrawDetail();
						UpdateWindow();
#else
#ifndef TOUCHLIST_SCROLLWITHBK
						// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͍ĕ`��
						Invalidate();
#endif
#endif
					}

					// �����X�N���[���̒�~
					MZ3_TRACE( L"*************AUTOSCROLL:KillTimer#1\n" );
					MyResetAutoScrollTimer();

				} else {
					// dyAutoScroll �������ړ�����B
					CPoint lastPoint = m_autoScrollInfo.getLastPoint();

					int dy = lastPoint.y + dyAutoScroll;

					MZ3_TRACE( L"*************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
					if( ScrollByMoveY( dy ) ){
						// �擪���Ō���ɒB����
						// �����X�N���[���̒�~
						MZ3_TRACE( L"*************AUTOSCROLL:KillTimer#2\n" );
						MyResetAutoScrollTimer();
					}
				}

				m_yAutoScrollMax = dyAutoScroll;
				MZ3_TRACE( L"*************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );
				break;
			}
		default:
			CListCtrl::OnTimer(nIDEvent);
			break;
	}
}

/**
 * ScrollByMoveY(const int dy)
 * �h���b�O���̕`�揈��
 * ����
 *   dy�F�}�E�X��Y���W�ipoint.y�l�ACTouchListCtrl�R���g���[���̃��[�J�����W�A��Βl�j
 * �߂�l
 *   �`��ʒu���擪���Ō���ɂȂ����ꍇ��true�A�����łȂ��ꍇ��false��Ԃ�
 * �֘A���郁���o�ϐ�
 *   m_ptDragStart	�F(I)	�F�h���b�O�J�n�_
 *   m_iItemHeight	�F(I)	�F�A�C�e��1������̍����i�s�N�Z���j
 *   m_iDragLine	�F(I/O)	�F�h���b�O�J�n���猻�݂܂łɃX�N���[�������s��
 *   m_offsetPixelY	�F(O)	�F�s�N�Z���P�ʃX�N���[���̂��߂̃I�t�Z�b�g�l
 * �@�\
 * �E�h���b�O�J�n�_�im_ptDragStart.y�j����}�E�X�̌��݈ʒu�idy�j�̈ړ��ʂ����
 *   ��ʂɕ\������Item�̈ʒu�����肵�A�X�N���[�����ĕ\������
 */
bool CTouchListCtrl::ScrollByMoveY(const int dy)
{
	// �擪or�Ō���t���O
	bool bLimitOver = false;

	// �����_�ł̕\���A�C�e���擪�ʒu�A�I�t�Z�b�g��ۑ�����
	int iTop = GetTopIndex();
	int iOffset = m_offsetPixelY;

	// Drag�J�n�_����̈ړ��s�������߂�
	int iScrollLine = (( dy - m_ptDragStart.y ) / m_iItemHeight ) ;

#ifndef WINCE
	// PIXEL�P�ʂŃX�N���[��������
	m_offsetPixelY = (( dy - m_ptDragStart.y ) % m_iItemHeight );
#else
	// WM�ł�Item�P�ʂŃX�N���[��������
	m_offsetPixelY = 0;
#endif

	// �O��̈ړ��s������̍��������X�N���[������
	//  Scroll()���\�b�h�̓s�N�Z�����w�肾���A�s�N�Z���w�肷���
	//  1�s���̔������炢��1�s�ړ������肵�ă}�E�X�̓����Ɠ��������Ȃ��̂�
	//  �s���~�����œ�����
	if( abs(iScrollLine - m_iDragLine) > 0 ){
		CSize szScroll( 0 , -(( iScrollLine - m_iDragLine ) * m_iItemHeight) );

		MZ3_TRACE( L"ScrollByMoveY, Scroll(%5d)\n" , szScroll.cy );

		// Win32�ł͓Ǝ������ŕ`�悷��
		// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
		// �X�N���[�����̍ĕ`����֎~
#ifndef WINCE
		LockWindowUpdate();
#endif

		// �X�N���[�����s
		Scroll( szScroll );
				
		// �X�N���[�����̍ĕ`����ĊJ
#ifndef WINCE
		UnlockWindowUpdate();
#endif
	}

	// �X�N���[����̐擪�ʒu���擾����
	int iNextTop = GetTopIndex();

	// ��[����܂��͉��[��艺�Ȃ�΃I�t�Z�b�g���������Ȃ�
	if( ( iNextTop < 0 ) ||
		( iNextTop  == 0 && m_offsetPixelY > 0) ||
		( GetItemCount() - GetCountPerPage() == iNextTop && m_offsetPixelY < 0) ||
		( GetItemCount() - GetCountPerPage() < iNextTop ) ){
		m_offsetPixelY = 0;
		// �擪or�Ō���t���O
		bLimitOver = true;
	}

	if( iTop != iNextTop ) {
		// Item�\���ʒu���ς����
		// �g�[�^���ړ��s����~�ς���
		m_iDragLine += iTop - iNextTop;

#ifndef WINCE
		// �ĕ`��
		MZ3_TRACE( L"ScrollByMoveY, DrawDetail() req1\n" );
		// Win32�ł͓Ǝ������ŕ`�悷��
		// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
		DrawDetail();
		UpdateWindow();
#endif
	} else {
		// Item�\���ʒu�͕ς��Ȃ�
		if( iOffset != m_offsetPixelY ){
			// �I�t�Z�b�g���ς����

#ifndef WINCE
			// �ĕ`��
			MZ3_TRACE( L"ScrollByMoveY, DrawDetail() req2\n" );
			// Win32�ł͓Ǝ������ŕ`�悷��
			// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
			DrawDetail();
			UpdateWindow();
#endif
		}
	}

	return bLimitOver;
}

/*
 * �^�C�}�[��~����
*/
void CTouchListCtrl::ResetAllTimer()
{
	// �x���`��^�C�}�[���~
	MyResetRedrawTimer();
	// �I�t�Z�b�g�̃Y���𒲐�
	MyAdjustDrawOffset();
	// �����X�N���[���^�C�}�[���~
	MyResetAutoScrollTimer();
}

// ���ڂɕω�������΃^�C�}�[�������~����
// �v�����܂ܕ��ׂĂ݂����A�e����ResetAllTimer()���Ă񂾕�����������

void CTouchListCtrl::OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	ResetAllTimer();
	*pResult = 0;
}

void CTouchListCtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	ResetAllTimer();
	*pResult = 0;
}

void CTouchListCtrl::OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	ResetAllTimer();
	*pResult = 0;
}

/*
 * WM_RBUTTONDOWN ���b�Z�[�W�n���h��
 *  �E�����X�N���[�����Ă���Β�~����
*/
void CTouchListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// �I�t�Z�b�g�̃Y���𒲐�
	MyAdjustDrawOffset();
	// �����X�N���[���̒�~
	MyResetAutoScrollTimer();

	CListCtrl::OnRButtonDown(nFlags, point);
}

/*
 * MyAdjustDrawOffset()
 * m_offsetPixelY��0�ɂȂ�悤�ɒ�������
 *  �E�ړ����Ȃ�i�s������1�i�߂Ď~�߂�
 *  �E�ړ����łȂ���΋߂����Ɋ񂹂Ď~�߂�
*/
bool CTouchListCtrl::MyAdjustDrawOffset()
{
	bool bMove = false;
	if( m_offsetPixelY != 0 ){
		// �I�t�Z�b�g���c���Ă���Ȃ�
		bMove = true;

		// ��{�̓I�t�Z�b�g�������߂�
		int dy = m_iDragLine * m_iItemHeight + m_ptDragStart.y;

		if( m_yAutoScrollMax > 0) {
			// �������ړ���
			if( m_offsetPixelY > 0 ){
				// ���ɂ͂ݏo�Ă���΂P�R���ɉ�����
				dy += m_iItemHeight;
			}
		} else if( m_yAutoScrollMax < 0 ) {
			// ������ړ���
			if( m_offsetPixelY < 0 ){
				// ��ɂ͂ݏo�Ă���΂P�R��ɏグ��
				dy -= m_iItemHeight;
			}
		} else {
			// ��~��
			if( m_offsetPixelY > m_iItemHeight / 2 ){
				// 1�s�̔����ȏ�A���ɂ͂ݏo�Ă����1�R���ɉ�����
				dy += m_iItemHeight;
			} else if( m_offsetPixelY < - m_iItemHeight / 2 ){
				// 1�s�̔����ȏ�A��ɂ͂ݏo�Ă����1�R��ɏグ��
				dy -= m_iItemHeight;
			}
		}
		// �X�N���[��
		MZ3_TRACE( L"*************AUTOSCROLL:m_iDragLine=%d,m_ptDragStart.y=%d,m_iItemHeight=%d\n" , m_iDragLine , m_ptDragStart.y , m_iItemHeight );
		MZ3_TRACE( L"*************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
		ScrollByMoveY( dy );
#ifdef WINCE
#ifndef TOUCHLIST_SCROLLWITHBK
		// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͍ĕ`��
		Invalidate();
#endif
#endif
	}

	return bMove;
}

/*
 * WM_LBUTTONDBLCLK ���b�Z�[�W�n���h��
 *  �E�ŏ��̃N���b�N�ʒu�Ɠ��ڂ����ꂽ���̂��߂ɑI�����ڂ���ڂɍ��킹��
 */
void CTouchListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// �I��ύX
	int nItem = HitTest(point);
	if (nItem>=0) {
		int idx = util::MyGetListCtrlSelectedItemIndex( *this );
		if( GetItemCount() > 0 && idx >= 0 ) {
			util::MySetListCtrlItemFocusedAndSelected( *this, idx, false );
			util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
		}
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}
