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
#include "MouseGestureManager.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "MZ3View.h"

IMPLEMENT_DYNAMIC(CTouchListCtrl, CListCtrl)

CTouchListCtrl::CTouchListCtrl(void)
	: m_bStopDraw(false)
	, m_bBlackScrollMode(false)
	, m_offsetPixelY(0)
	, m_bDragging(false)
	, m_memBMP(NULL)
	, m_memDC(NULL)
	, m_bTimerRedraw(false)
	, m_bDrawBk(true)
	, m_bScrollWithBk(true)
	, m_bAutoScrolling(false)
	, m_bUseHorizontalDragMove(true)
	, m_bPanDragging(false)
	, m_bScrollDragging(false)
	, m_bCanSlide(false)
	, m_iDragStartItem(-1)
	, m_bUsePanScrollAnimation(true)
	, m_offsetPixelX(0)
	, m_drPanScrollDirection( PAN_SCROLL_DIRECTION_NONE )
	, m_bCanPanScroll(false)
	, m_hPanScrollEvent(NULL)
	, m_bStopDragging(false)
{
	// �C�x���g�I�u�W�F�N�g�쐬
	m_hPanScrollEvent = CreateEvent( NULL , TRUE , TRUE , NULL );
}

CTouchListCtrl::~CTouchListCtrl(void)
{
	// �C�x���g�I�u�W�F�N�g�N���[�Y
	if(	m_hPanScrollEvent ){
		CloseHandle( m_hPanScrollEvent );
	}

}

BEGIN_MESSAGE_MAP(CTouchListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, &CTouchListCtrl::OnLvnDeleteallitems)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CTouchListCtrl::OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, &CTouchListCtrl::OnLvnInsertitem)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DRAWITEM()
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
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_PAINT, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_MOUSEWHEEL:
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_MOUSEWHEEL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_VSCROLL:
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_VSCROLL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
//	case WM_PAINT:
		// ������h�~�̂��ߎՒf
//		return TRUE;

	case WM_DRAWITEM:
//		MZ3_TRACE(L"CTouchListCtrl::PreTranslateMessage(WM_VSCROLL, 0x%04X, 0x%04x)\n", pMsg->wParam, pMsg->lParam);
		break;

	default:
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(0x%04X, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
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
	if (!m_bDragging && m_bStopDragging) {
		return;
	}

	MZ3_TRACE( L"OnLButtonDown()\n");

	// �t�H�[�J�X��ݒ肷��
	SetFocus();

	// �I�t�Z�b�g�̂���𒲐�
	MyAdjustDrawOffset();
	// �����X�N���[����~
	MyResetAutoScrollTimer();

	m_iDragStartItem = HitTest( point );

	//if( !m_bCanSlide ){
	//	SetSelectItem( m_iDragStartItem );
	//}
#ifdef WINCE
	// �t�H�[�J�X��`��\��
	DrawItemFocusRect( m_iDragStartItem );
	// �^�b�v�������Ń\�t�g�L�[���j���[�\��
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		// ����������
		// �t�H�[�J�X��`������
		DrawItemFocusRect( m_iDragStartItem );
		// ���ڂ�I������
		SetSelectItem( m_iDragStartItem );
		RedrawItems( m_iDragStartItem , m_iDragStartItem );
		// ���j���[���|�b�v�A�b�v
		ClientToScreen(&point);
		PopupContextMenu(point);
		return;
	}
	// �t�H�[�J�X��`������
	DrawItemFocusRect( m_iDragStartItem );
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
	if (!m_bDragging && m_bStopDragging) {
		return;
	}

	MZ3_TRACE( L"OnLButtonUp(0x%X,%d,%d)\n", nFlags, point.x, point.y);
	int dx = point.x - m_ptDragStart.x;
	int dy = point.y - m_ptDragStart.y;

	if (m_bDragging) {
		// �h���b�O�I������

		// �L���v�`���I��
		ReleaseCapture();

		// dx,dy�̃h���b�O�ʂɉ����āA�h���b�O�J�n���ǂ����𔻒肷��
		// m_bPanDragging, m_bScrollDragging, m_drPanScrollDirection ���ݒ肳���
		MySetDragFlagWhenMovedPixelOverLimit(dx,dy);

		// �x���`��^�C�}�[�̃��Z�b�g
		MyResetRedrawTimer();

		// �}�E�X�J�[�\�������ɖ߂�
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );

		if( !m_bPanDragging && !m_bScrollDragging ) {
			// �㉺���E�̃h���b�O���Ȃ���΃A�C�e����I������
			// �I��
			SetSelectItem( m_iDragStartItem );
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
		}

		// �X�N���[�������H
		if ( m_bScrollDragging ) {

			// �����X�N���[�����擾
			m_autoScrollInfo.push( GetTickCount(), point );
			double speed = m_autoScrollInfo.calcMouseMoveSpeedY();
			MZ3_TRACE( L"! speed   : %5.3f [px/msec]\n", speed );
			if( abs(point.y - m_ptDragStart.y) > m_iItemHeight ){
				// 1�s�ȏ�h���b�O���Ă���̂Ŋ����X�N���[���J�n
				MZ3_TRACE( L" 1�s�ȏ�h���b�O���Ă���̂Ŋ����X�N���[���J�n\n" );
				m_dwAutoScrollStartTick = GetTickCount();
				m_yAutoScrollMax = 0;
				MySetAutoScrollTimer( TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL );
			} else {
				// 1�s�����̃h���b�O�Ȃ�΂����Ɏ~�߂�
				MyAdjustDrawOffset(MyAdjustDrawOffset_ADJUST_ONLY);
				m_autoScrollInfo.clear();

				// �������ĕ`��
				MZ3_TRACE( L" �������ĕ`��#0\n" );
				m_bDragging = false;
				m_bScrollDragging = false;
				DrawDetail(true);
				UpdateWindow();
			}
		} else if( m_bPanDragging ){ 
			// �������Ƀh���b�O
			switch( m_drPanScrollDirection ) {
			case PAN_SCROLL_DIRECTION_RIGHT:
				MoveSlideRight();
				break;
			case PAN_SCROLL_DIRECTION_LEFT:
				MoveSlideLeft();
				break;
			}
		}
		// �t���O�N���A
		m_bDragging = false;
		m_bPanDragging = false;
		m_bScrollDragging = false;
		m_drPanScrollDirection = PAN_SCROLL_DIRECTION_NONE;
	}
	//CListCtrl::OnLButtonUp(nFlags, point);
}

/**
 * WM_MOUSEMOVE ���b�Z�[�W�n���h��
 *  �E�h���b�O���Ȃ�Γ��I�X�N���[���������s��
 */
void CTouchListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bDragging && m_bStopDragging) {
		return;
	}

	if( m_bDragging ) {
		int dx = point.x - m_ptDragStart.x;
		int dy = point.y - m_ptDragStart.y;

		MZ3_TRACE(L"CTouchListCtrl::OnMouseMove\n");

		// dx,dy�̃h���b�O�ʂɉ����āA�h���b�O�J�n���ǂ����𔻒肷��
		// m_bPanDragging, m_bScrollDragging, m_drPanScrollDirection ���ݒ肳���
		MySetDragFlagWhenMovedPixelOverLimit(dx, dy);

		// �c�X�N���[�������H
		if ( m_bScrollDragging ) {
			//if( m_bCanSlide ){
			//	SetSelectItem( m_iDragStartItem );
			//}
			// �c�X�N���[����
			// �O�[�̃J�[�\���ɕύX
			::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );

			// �c�h���b�O����
			ScrollByMoveY( point.y );
#ifdef WINCE
			if( !IsScrollWithBk() ){
				// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͒x���ĕ`��
				MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L );
			}
#endif
			// �����X�N���[�����擾
			m_autoScrollInfo.push( GetTickCount(), point );
		} else if( m_bPanDragging ){
			// ���X�N���[����
			// �}�E�X�|�C���^�ύX
			switch( m_drPanScrollDirection ){
				case PAN_SCROLL_DIRECTION_LEFT:
					// ������
					::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_LEFT_CURSOR) );
					break;
				case PAN_SCROLL_DIRECTION_RIGHT:
					// �E����
					::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_RIGHT_CURSOR) );
					break;
			}
		} else {
			::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
#ifdef WINCE
			// �t�H�[�J�X��`��\��
			DrawItemFocusRect( m_iDragStartItem );
			// �^�b�v�������Ń\�t�g�L�[���j���[�\��
			// �i�w�悪�����������̂��߁j
			SHRGINFO RGesture;
			RGesture.cbSize     = sizeof(SHRGINFO);
			RGesture.hwndClient = m_hWnd;
			RGesture.ptDown     = point;
			RGesture.dwFlags    = SHRG_RETURNCMD;
			if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
				// ����������
				// �t�H�[�J�X��`������
				DrawItemFocusRect( m_iDragStartItem );

				// WM_LBUTTONUP�Ɠ��l�Ƀh���b�O�I���������s��

				// �L���v�`���I��
				ReleaseCapture();
				// �x���`��^�C�}�[�̃��Z�b�g
				MyResetRedrawTimer();
				// �I����Ԃ̐ݒ�
				SetSelectItem( m_iDragStartItem );
				RedrawItems( m_iDragStartItem , m_iDragStartItem );
				// �I�t�Z�b�g�̒����i�O�̂��߁j
				MyAdjustDrawOffset();
				m_autoScrollInfo.clear();
				// �h���b�O�t���O�N���A
				m_bDragging = false;
				m_bPanDragging = false;
				m_bScrollDragging = false;
				m_drPanScrollDirection = PAN_SCROLL_DIRECTION_NONE;

				// ���j���[�|�b�v�A�b�v
				ClientToScreen(&point);
				PopupContextMenu(point);
				return;
			}
			// �t�H�[�J�X��`������
			DrawItemFocusRect( m_iDragStartItem );
#endif
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

	// ����ʃo�b�t�@��������΂����Ŋm�ۂ���
	if( m_memBMP == NULL ) {
		CDC* pDC = GetDC();
		MyMakeBackBuffers(pDC);
		ReleaseDC(pDC);
	} else {
		// ����ʃo�b�t�@�̃T�C�Y���������ꍇ�͍Đ���
		BITMAP bmp;
		GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
		if (bmp.bmWidth < m_screenWidth ||
			bmp.bmHeight < m_screenHeight*2) 
		{
			CDC* pDC = GetDC();
			MyMakeBackBuffers(pDC);
			ReleaseDC(pDC);
		}
	}
}

/**
 * ����ʃo�b�t�@�̐���
 */
bool CTouchListCtrl::MyMakeBackBuffers(CDC* pdc)
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
	if (m_memBMP->CreateCompatibleBitmap( pdc , m_screenWidth , m_screenHeight*2 ) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}
	// ���ۂɕ`�悷��̈�̐擪�I�t�Z�b�g
	m_drawStartTopOffset = m_screenHeight/2;

	// DC�𐶐�
	m_memDC->CreateCompatibleDC(pdc);
	m_memDC->SetBkMode(OPAQUE);					// ���߃��[�h�ɐݒ肷��
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	return true;
}

/**
 * WM_DESTROY ���b�Z�[�W�n���h��
 *  �E����ʃo�b�t�@�̉��
 */
void CTouchListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();
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

/**
 * DrawToScreen() �`��
 *
 *  ����ʃo�b�t�@�����ʕ����f�o�C�X�֓]��
 */
void CTouchListCtrl::DrawToScreen(CDC* pDC)
{
	MZ3_TRACE( L"����DrawToScreen(0x%X)\n", pDC!=NULL ? pDC->m_hDC : 0);
	if( m_memDC == NULL ){
		MZ3_TRACE( L" invalid memory DC...\n");
		return;
	}

	if (!m_bBlackScrollMode) {
		int ys = m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY;

		MZ3_TRACE( L" (%d,%d) => (%d,%d) [%d,%d]\n", 
			0				, ys,
			0				, m_viewRect.top,
			m_screenWidth	, m_screenHeight);

		pDC->BitBlt( 
			0				, m_viewRect.top,
			m_screenWidth	, m_screenHeight, 
			m_memDC,
			0				, ys,
			SRCCOPY );
	} else {
		int ys = 0;
		// ���X�N���[���h���b�O���A�܂��͊����X�N���[�����ł���΁A�`��͈͂��v�Z����
		int offsetPixelY = 0;
		if (m_bScrollDragging) {
			// �X�N���[���h���b�O�� �˃J�[�\�����W�ƃh���b�O�J�n���W����`��͈͂��v�Z
			offsetPixelY = m_autoScrollInfo.getLastPoint().y - m_ptDragStart.y;
		} else if (m_bAutoScrolling) {
			// �����X�N���[����     �ˊ����X�N���[�����W�ƃh���b�O�J�n�ʒu����`��͈͂��v�Z
			offsetPixelY = m_autoScrollInfo.getLastPoint().y - m_ptDragStart.y + m_yAutoScrollMax;
		} else {
			// m_offsetPixelY �����炵�ĕ\������
			offsetPixelY = m_offsetPixelY;
		}
		ys = m_drawStartTopOffset + m_viewRect.top - offsetPixelY;

		// ����ʃo�b�t�@�̓]��
		if (m_bScrollDragging || m_bAutoScrolling) {
			int h = m_screenHeight;
			int ydest = m_viewRect.top;

			MZ3_TRACE( L" offsetPixelY : %d\n", offsetPixelY);
			if (offsetPixelY > 0) {
				// �㑤��h��Ԃ�
				h     = m_screenHeight - offsetPixelY;
				ydest = m_viewRect.top + offsetPixelY;
				ys    = m_drawStartTopOffset + m_viewRect.top;
			} else if (offsetPixelY < 0) {
				// ������h��Ԃ�
				h     = m_screenHeight + offsetPixelY;
				ydest = m_viewRect.top;
				ys    = m_drawStartTopOffset + m_viewRect.top - offsetPixelY;
			}

			// �`��͈͕������]������
			MZ3_TRACE( L" (%d,%d) => (%d,%d) [%d,%d]\n", 
				0				, ys,
				0				, ydest,
				m_screenWidth	, h);
			pDC->BitBlt( 
				0				, ydest,
				m_screenWidth	, h, 
				m_memDC,
				0				, ys,
				SRCCOPY );

			// �`��͈͊O��h��Ԃ�
			if (ydest > m_viewRect.top) {
				// �㑤��h��Ԃ�
				int hFill = offsetPixelY;
				int yFill = 0;
				pDC->FillSolidRect(0, m_viewRect.top+yFill, m_screenWidth, hFill, RGB(0,0,0));
			} else if (h < m_screenHeight) {
				// ������h��Ԃ�
				int hFill = m_screenHeight - h;
				int yFill = ydest + h;
				pDC->FillSolidRect(0, m_viewRect.top+yFill, m_screenWidth, hFill, RGB(0,0,0));
			}

		} else {
			MZ3_TRACE( L" (%d,%d) => (%d,%d) [%d,%d]\n", 
				0				, ys,
				0				, m_viewRect.top,
				m_screenWidth	, m_screenHeight);

			pDC->BitBlt( 
				0				, m_viewRect.top,
				m_screenWidth	, m_screenHeight, 
				m_memDC,
				0				, ys,
				SRCCOPY );
		}
	}
}

/**
 * DrawToScreen() �`��
 *
 *  ����ʃo�b�t�@�����ʕ����f�o�C�X�֓]��
 */
void CTouchListCtrl::DrawToScreen(CDC* pDC, const CRect& rectDest)
{
//	MZ3_TRACE( L"DrawToScreen(0x%X, %d, %d, %d, %d)\n", pDC!=NULL ? pDC->m_hDC : 0, 
//		rectDest.left, rectDest.top, rectDest.right, rectDest.bottom);
	if( m_memDC == NULL ){
		return;
	}
	// �擪�A�C�e���̋�`�擾
	// ��1�s�\�����[�h���́u�J�����v����(y=18�Ȃ�)���������߁B2�s�\�����[�h�̏ꍇ�� y=0�B
	CRect rectItem0;
	GetItemRect(GetTopIndex(), &rectItem0, LVIR_BOUNDS);
	int hHeaderColumn = rectItem0.top;

	// �ύX���ʂ�m_offsetPixelY�����炵�ĕ\������
	int x = rectDest.left;
	int y = rectDest.top +m_viewRect.top -hHeaderColumn;
	int w = rectDest.Width();
	int h = rectDest.Height();
	// y ���`��͈͊O�Ȃ�`�悵�Ȃ��B
	if (y>=rectItem0.top) {
		pDC->BitBlt( 
			x, y,
			w, h, 
			m_memDC,
			x, y +m_drawStartTopOffset -m_offsetPixelY,
			SRCCOPY );
	}
}

/**
 * DrawItemWithBackSurface() �`��
 *
 *  ����ʃo�b�t�@�����ʕ����f�o�C�X�֓]��
 */
void CTouchListCtrl::DrawItemWithBackSurface(int nItem)
{
	MZ3_TRACE( L"DrawItemWithBackSurface(%d)\n", nItem);
	if( m_memDC == NULL ){
		return;
	}

	// �A�C�e���̋�`�擾
	CRect rectItem;
	GetItemRect(nItem, &rectItem, LVIR_BOUNDS);

	// �w�i��h��Ԃ�
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	HBITMAP hBgBitmap = GetBgBitmapHandle();
	CRect r = rectItem;
	r.OffsetRect( 0, m_drawStartTopOffset-m_offsetPixelY );
	int x = r.left;
	int y = r.top;
	int w = r.Width();
	int h = r.Height();
//	int h = r.Height() + m_iItemHeight*2;
	
	MZ3_TRACE( L"DrawToScreen(%d), [%d, %d, %d, %d], [%d,%d,%d,%d], %d, %d\n", 
		nItem, 
		rectItem.left, rectItem.top, rectItem.Width(), rectItem.Height(),
		x, y, w, h, m_drawStartTopOffset, m_offsetPixelY);

	if( !theApp.m_optionMng.IsUseBgImage() || hBgBitmap == NULL ) {
		// �w�i�摜�Ȃ��̏ꍇ
		m_memDC->FillSolidRect(x, y, x+w, y+h, RGB(255,255,255));
	}else{
		// �w�i�r�b�g�}�b�v�̕`��
		int offset = 0;
		if( IsScrollWithBk() ){
			offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
		}
		util::DrawBitmap( m_memDC->GetSafeHdc(), hBgBitmap, x, y, w, h, x, rectItem.top + offset );
	}

	// �f�o�b�O�p��`
//	m_memDC->Rectangle(x, y, x+w, y+h);

	// ���߃��[�h�ɐݒ�
	m_memDC->SetBkMode(TRANSPARENT);

	// �t�H���g�����݂̃t�H���g�ɒu������
	CFont* oldFont = (CFont*)m_memDC->SelectObject( GetFont() );

	// �o�b�N�o�b�t�@�ɕ`��
	DrawItemToBackSurface(nItem);

	// �t�H���g�����ɖ߂�
	m_memDC->SelectObject( oldFont );

	// �����I�ɕ`�悷��
	CDC* pDC = GetDC();
	DrawToScreen(pDC, rectItem);
//	DrawToScreen(pDC);
	ReleaseDC(pDC);

	ValidateRect( &rectItem );
}

/**
 * PanDrawToScreen() �`��
 *  �p���X�N���[���p�ɗ���ʃo�b�t�@�����ʕ����f�o�C�X�֓]��
 */
void CTouchListCtrl::PanDrawToScreen(CDC* pDC)
{
	MZ3_TRACE( L"PanDrawToScreen(0x%X)\n", pDC!=NULL ? pDC->m_hDC : 0);
	if( m_memDC == NULL ){
		return;
	}
	int sx = 0;
	int dx = 0;
	int wid = m_screenWidth;

	// �p���X�N���[������
	if( m_offsetPixelX > 0 ){
		// �E�ɂ���Ă���ꍇ
		sx = 0;
		dx = m_offsetPixelX;
		wid = m_screenWidth - m_offsetPixelX;

		// �ύX�O��ʂ������ɕ\������
		pDC->BitBlt( 0 , m_viewRect.top , m_offsetPixelX , m_screenHeight , m_memDC , wid , m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY, SRCCOPY );

		// �ړ����킩��悤�ɏc��������
		pDC->MoveTo( m_offsetPixelX - 1 , m_viewRect.top );
		pDC->LineTo( m_offsetPixelX - 1 , m_viewRect.bottom );
	} else if( m_offsetPixelX < 0 ) {
		// ���ɂ���Ă���ꍇ
		sx = -m_offsetPixelX;
		dx = 0;
		wid = m_screenWidth + m_offsetPixelX;

		// �ύX�O��ʂ��E���ɕ\������
		pDC->BitBlt( wid , m_viewRect.top , sx , m_screenHeight , m_memDC , 0 , m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY, SRCCOPY );

		// �ړ����킩��悤�ɏc��������
		pDC->MoveTo( wid + 1 , m_viewRect.top);
		pDC->LineTo( wid + 1 , m_viewRect.bottom );
	}

	// �I�t�Z�b�g�����ʕ�����Ε\���s�v
	if( abs( m_offsetPixelX ) > abs( m_screenWidth ) ){
		return;
	}

	// �w�i���I�t�Z�b�g�ɍ��킹�ĕ\������
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	HBITMAP hBgBitmap = GetBgBitmapHandle();
	if( !theApp.m_optionMng.IsUseBgImage() || hBgBitmap == NULL ) {
		// �w�i�摜�Ȃ��̏ꍇ
		pDC->FillSolidRect( dx, m_viewRect.top, wid, m_screenHeight, RGB(255,255,255) );
	}else{
		// �w�i�r�b�g�}�b�v�̕`��
		CRect rectViewClient;
		this->GetClientRect( &rectViewClient );
		rectViewClient.OffsetRect( 0 , m_drawStartTopOffset-m_offsetPixelY );
		int x = dx;
		int y = m_viewRect.top;
		int w = wid;
		int h = m_screenHeight;
		int offset = 0;
		if( IsScrollWithBk() ){
			offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
		}
		util::DrawBitmap( pDC->GetSafeHdc(), hBgBitmap, x, y , w, h, sx, m_viewRect.top + offset );
	}
}

/**
 * DrawDetail()
 *  ����ʃo�b�t�@�Ƀ��X�g���ڂ�`�悷��
 */
int	CTouchListCtrl::DrawDetail( bool bForceDraw )
{
	MZ3_TRACE( L"DrawDetail(%s)\n", bForceDraw ? L"true" : L"false");
	if( m_memDC == NULL ){
		return 0;
	}

	// �O�̂��ߒx���`��^�C�}�[���~����
	MyResetRedrawTimer();

	// �w�i��h��Ԃ�
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	if (m_bBlackScrollMode && (m_bScrollDragging || m_bAutoScrolling)) {
		// ���h���b�O���͔w�i���ĕ`�悵�Ȃ�
	} else {

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
			int offset = 0;
			if( IsScrollWithBk() ){
				offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
			}
			util::DrawBitmap( m_memDC->GetSafeHdc(), hBgBitmap, x, y, w, h, 0, 0 + offset );
		}

		MZ3_TRACE( L" Top=%5d,offset=%5d\n" , GetTopIndex() , m_offsetPixelY );
	}

	// ���߃��[�h�ɐݒ�
	m_memDC->SetBkMode(TRANSPARENT);

	if (m_bBlackScrollMode && (m_bScrollDragging || m_bAutoScrolling)) {
		// ���h���b�O���̓A�C�e�����ĕ`�悵�Ȃ�
	} else {
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

			// �o�b�N�o�b�t�@�ɃA�C�e����`�悷��
			DrawItemToBackSurface(nItem);
		}

		// �t�H���g�����ɖ߂�
		m_memDC->SelectObject( oldFont );
	}

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
	// => �Ȃ����w�i�F�h��Ԃ��ɂȂ�̂ŕW�������ɔC����(��������d���Ȃ�)
//	DrawDetail();
//	UpdateWindow();
#else
	// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
	if( !IsScrollWithBk() ){
		// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͒x���ĕ`��
		MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L );
	}
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
	if ( !theApp.m_pMouseGestureManager->IsGestureMode() ) {
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
 *  �ETIMERID_TOUCHLIST_PANSCROLL:
 *    �p���X�N���[������
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

				MZ3_TRACE( L" *************AUTOSCROLL:dyAutoScroll=%d\n", dyAutoScroll );
				MZ3_TRACE( L" *************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );

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
						MyAdjustDrawOffset(MyAdjustDrawOffset_ADJUST_ONLY);
					} else {
						// ��~��ԂōŌ�̕`��
#ifndef WINCE
						// Win32�ł͓Ǝ������ŕ`�悷��
						// WM�ł͏������ǂ����Ȃ��̂ŕW�������ɔC����
//						DrawDetail();
//						UpdateWindow();

//#else
//						if( !IsScrollWithBk() ){
//							// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͍ĕ`��
//							Invalidate();
//						}
#endif
					}

					// �����X�N���[���̒�~
					MZ3_TRACE( L" *************AUTOSCROLL:KillTimer#1\n" );
					MyResetAutoScrollTimer();

					// �������ĕ`��
					MZ3_TRACE( L" �������ĕ`��#1\n" );
					DrawDetail(true);
					UpdateWindow();
				} else {
					// dyAutoScroll �������ړ�����B
					CPoint lastPoint = m_autoScrollInfo.getLastPoint();

					int dy = lastPoint.y + dyAutoScroll;

					MZ3_TRACE( L" *************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
					if( ScrollByMoveY( dy ) ){
						// �擪���Ō���ɒB����
						// �����X�N���[���̒�~
						MZ3_TRACE( L" *************AUTOSCROLL:KillTimer#2\n" );
						MyResetAutoScrollTimer();

						// �������ĕ`��
						MZ3_TRACE( L" �������ĕ`��#2\n" );
						DrawDetail(true);
						UpdateWindow();
					}
				}

				m_yAutoScrollMax = dyAutoScroll;
				MZ3_TRACE( L" *************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );
#ifdef WINCE
				if( !IsScrollWithBk() ){
					// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͒x���ĕ`��
					MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L );
				}
#endif
				break;
			}
/****************************************************
*			 �p���X�N���[������
****************************************************/
		case TIMERID_TOUCHLIST_PANSCROLL:
			{
				int dwDt = GetTickCount() - m_dwPanScrollLastTick;
				m_dwPanScrollLastTick = GetTickCount();

				if( m_dPxelX == 0 ) {
					// �ړ��ʃ[���Ȃ疳�����[�v�h�~�̂��ߒ��~
					MyResetPanScrollTimer();
				} else {
					// �ړ�����
					m_offsetPixelX += dwDt * m_dPxelX / 10;

					MZ3_TRACE( L" m_offsetPixelX = %5d, dwDt = %5d\n"  , m_offsetPixelX , dwDt );

					// �I������
					if( m_dPxelX > 0 ){
						if( m_offsetPixelX > 0 ){
							m_offsetPixelX = 0;
						}
					} else {
						if( m_offsetPixelX < 0 ){
							m_offsetPixelX = 0;
						}
					}
					// �����I�ɕ`�悷��
					CDC* pDC = GetDC();
					PanDrawToScreen(pDC);
					ReleaseDC(pDC);
					if( m_offsetPixelX == 0 ){
						// ���ʕ��ړ�����
						// �p���X�N���[���I��
						MyResetPanScrollTimer();
						m_dPxelX = 0;
					}
				}
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

	// ��
	if (m_bBlackScrollMode) {
		// PIXEL�P�ʂŃX�N���[��������
		m_offsetPixelY = (( dy - m_ptDragStart.y ) % m_iItemHeight );
	} else {
#ifndef WINCE
		// PIXEL�P�ʂŃX�N���[��������
		m_offsetPixelY = (( dy - m_ptDragStart.y ) % m_iItemHeight );
#else
		// WM�ł�Item�P�ʂŃX�N���[��������
		m_offsetPixelY = 0;
#endif
	}

	// �O��̈ړ��s������̍��������X�N���[������
	//  Scroll()���\�b�h�̓s�N�Z�����w�肾���A�s�N�Z���w�肷���
	//  1�s���̔������炢��1�s�ړ������肵�ă}�E�X�̓����Ɠ��������Ȃ��̂�
	//  �s���~�����œ�����
	if( abs(iScrollLine - m_iDragLine) > 0 ){
		CSize szScroll( 0 , -(( iScrollLine - m_iDragLine ) * m_iItemHeight) );

		MZ3_TRACE( L" ScrollByMoveY, Scroll(%5d)\n" , szScroll.cy );

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
	}

	if( ( iNextTop <= 0 ) ||
		( GetItemCount() - GetCountPerPage() <= iNextTop ) ){
		// �擪or�Ō���t���O
		bLimitOver = true;
	}

	bool bDoRedraw = false;
	if( iTop != iNextTop ) {
		// Item�\���ʒu���ς����
		// �g�[�^���ړ��s����~�ς���
		m_iDragLine += iTop - iNextTop;

		MZ3_TRACE( L" ScrollByMoveY, DrawDetail() req1\n" );
		bDoRedraw = true;
	} else {
		// Item�\���ʒu�͕ς��Ȃ�
		if( iOffset != m_offsetPixelY ){
			// �I�t�Z�b�g���ς����

			MZ3_TRACE( L" ScrollByMoveY, DrawDetail() req2\n" );
			bDoRedraw = true;
		}
	}

	if (bDoRedraw) {
		if (m_bBlackScrollMode) {
			// �ĕ`��
			DrawDetail();
			UpdateWindow();
		} else {
#ifndef WINCE
			// �ĕ`��
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
	// �p���X�N���[���^�C�}�[�͂����ł͒�~���Ȃ�
	//MyResetPanScrollTimer();
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
bool CTouchListCtrl::MyAdjustDrawOffset(bool bAdjustOnly)
{
//	MZ3_TRACE(L"MyAdjustDrawOffset()\n");
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
		MZ3_TRACE( L" *************AUTOSCROLL:m_iDragLine=%d,m_ptDragStart.y=%d,m_iItemHeight=%d\n" , m_iDragLine , m_ptDragStart.y , m_iItemHeight );
		MZ3_TRACE( L" *************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
		if (!bAdjustOnly) {
			ScrollByMoveY( dy );
#ifdef WINCE
			if( !IsScrollWithBk() ){
				// WM�ŁA���w�i�����X�N���[���łȂ��ꍇ�͍ĕ`��
				Invalidate();
			}
#endif
		}
	}

	return bMove;
}

/*
 * WM_LBUTTONDBLCLK ���b�Z�[�W�n���h��
 *  �E�ŏ��̃N���b�N�ʒu�Ɠ��ڂ����ꂽ��_�u���N���b�N���������Ȃ�
 */
void CTouchListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// �I��ύX
	int nItem = HitTest(point);
	if (nItem>=0) {
		int idx = util::MyGetListCtrlSelectedItemIndex( *this );
		if( GetItemCount() > 0 && idx >= 0 ) {
			util::MySetListCtrlItemFocusedAndSelected( *this, idx, false );
		}
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
		if( idx != nItem ){
			return;
		}
	}

	// �_�u���^�b�v�̖\���h�~�i�ł��Ă邩�ǂ����s�������O�̂���
	if( m_bAutoScrolling ) {
		return;
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}

/**
 * dx,dy�̃h���b�O�ʂɉ����āA�h���b�O�J�n���ǂ����𔻒肵�A�ϐ���ݒ肷��
 *
 * �h���b�O�J�n���� m_bPanDragging, m_bScrollDragging ��ݒ肷��
 */
void CTouchListCtrl::MySetDragFlagWhenMovedPixelOverLimit(int dx, int dy)
{
	// �c�h���b�O�J�n���f�p�I�t�Z�b�g�l
#ifndef WINCE
	// win32�̏ꍇ���s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
	int dyMinLimit = theApp.m_optionMng.GetFontHeight() / 2 + 1 ;
#else
	// WM�̏ꍇ��s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
	int dyMinLimit = theApp.m_optionMng.GetFontHeight();
#endif

	if (m_bPanDragging) {
		// ���h���b�O��
		if( abs( dx ) < m_screenWidth / 3 ){
			// �}�E�X�����ɖ߂����牡�h���b�O���L�����Z��
			m_bPanDragging = false ;
		}
	} else if (m_bScrollDragging) {
		// �c�h���b�O��

	} else {
		// �h���b�O�������m�肵�Ă��Ȃ�
		if( m_bCanSlide &&
			m_bUseHorizontalDragMove &&
			( abs(dx) > abs(dy) && abs(dx) > m_screenWidth / 3 ) ) {
				// �������̈ړ��ʂ��傫���Ĉړ��ʂ���ʂ�1/3�ȏ�̏ꍇ]
				// �h���b�O�����ݒ�
				if( dx > 0 ){
					m_drPanScrollDirection = PAN_SCROLL_DIRECTION_RIGHT;
				} else {
					m_drPanScrollDirection = PAN_SCROLL_DIRECTION_LEFT;
				}
				// ���h���b�O�J�n
				m_bPanDragging = true;
		} else if( abs(dx) < abs(dy) && abs(dy) > dyMinLimit ) {
			// �c�����̈ړ��ʂ��傫���Ĉړ��ʂ��h���b�O�J�n�I�t�Z�b�g�ȏ�̏ꍇ
			if( GetItemCount()-GetCountPerPage() > 0 ) {
				// �c�X�N���[���\�Ȃ��
				// �c�h���b�O�J�n
				m_bScrollDragging = true;
			}
		}
	}
}

/**
 * �p���X�N���[���J�n
 *
 * �X�N���[�������idirection�j�ɏ]���I�t�Z�b�g�l�ƍ�����ݒ肵�ă^�C�}�[���N������
 */
void CTouchListCtrl::StartPanScroll(PAN_SCROLL_DIRECTION direction)
{
#ifndef WINCE
#define PANSCROLL_DIVIDE 18
#else
#define PANSCROLL_DIVIDE 15
#endif
	if ( !m_bUsePanScrollAnimation || !m_bCanPanScroll ) {
		// �I�v�V�����ŃI�t�ɂȂ��Ă���̂ŃA�j���[�V�������Ȃ�
		Invalidate();
		return;
	}

	if( m_memDC == NULL ){
		return;
	}
	// �p���X�N���[���p�ɒ��O�̕\����Ԃ̃R�s�[�����i���ʕ��j
	CDC* pDC = GetDC();
	m_memDC->BitBlt( m_viewRect.left , m_viewRect.top + m_drawStartTopOffset , m_screenWidth , m_screenHeight , pDC , m_viewRect.left , m_viewRect.top , SRCCOPY );

	MyResetPanScrollTimer();

	switch (direction) {
	case PAN_SCROLL_DIRECTION_RIGHT:
		{
			// �E�����փX�N���[��
			HBITMAP hBmp;
			CBitmap cBmp;
			int rc = cBmp.LoadBitmap( IDB_SLIDE_RIGHT_BITMAP );
			hBmp = (HBITMAP) cBmp;
			BITMAP	bmp;
			GetObject( hBmp , sizeof(bmp), &bmp );
			CDC cTempDC;
			cTempDC.CreateCompatibleDC( m_memDC );
			HBITMAP oldBmp = (HBITMAP)SelectObject( cTempDC.m_hDC , hBmp );
			m_memDC->BitBlt( 0 , m_drawStartTopOffset + ( m_screenHeight - bmp.bmHeight ) / 2 , bmp.bmWidth , bmp.bmHeight , &cTempDC , 0 , 0 , SRCCOPY );
			SelectObject( cTempDC.m_hDC , oldBmp );
			
			// ���ֈ��ʂ��ꂽ�Ƃ��납��J�n
			m_offsetPixelX = - m_screenWidth;
			// �ړ�����
			m_dPxelX = m_screenWidth / PANSCROLL_DIVIDE + 1 ;
			break;
		}
	case PAN_SCROLL_DIRECTION_LEFT:
		{
			// �������փX�N���[��
			HBITMAP hBmp;
			CBitmap cBmp;
			int rc = cBmp.LoadBitmap( IDB_SLIDE_LEFT_BITMAP );
			hBmp = (HBITMAP) cBmp;
			BITMAP	bmp;
			GetObject( hBmp , sizeof(bmp), &bmp );
			CDC cTempDC;
			cTempDC.CreateCompatibleDC( m_memDC );
			HBITMAP oldBmp = (HBITMAP)SelectObject( cTempDC.m_hDC , hBmp );
			m_memDC->BitBlt( m_screenWidth - bmp.bmWidth , m_drawStartTopOffset + ( m_screenHeight - bmp.bmHeight ) / 2 , bmp.bmWidth , bmp.bmHeight , &cTempDC , 0 , 0 , SRCCOPY );
			SelectObject( cTempDC.m_hDC , oldBmp );

			// �E�ֈ��ʂ��ꂽ�Ƃ��납��J�n
			m_offsetPixelX = m_screenWidth;
			// �ړ�����
			m_dPxelX = - m_screenWidth / PANSCROLL_DIVIDE + 1 ;
			break;
		}
	}

	// �p���X�N���[�������ݒ�
	m_drPanScrollDirection = direction;

	// �p���X�N���[���J�n����
	m_dwPanScrollLastTick = GetTickCount();

	// �p���X�N���[���J�n
	ResetEvent( m_hPanScrollEvent );
	MZ3_TRACE( L" ResetEvent(0X%08X)\n" , m_hPanScrollEvent );
	MySetPanScrollTimer( TIMER_INTERVAL_TOUCHLIST_PANSCROLL );
}

/**
 * DrawItemFocusRect()
 *
 * �w�肳�ꂽ�A�C�e���Ƀt�H�[�J�X��`��\������
 * �EXOR�ŕ`�悷��̂�2��Ăׂ΋�`������
 *   �����͕`�悩�������ӎ��ł��Ȃ��̂ŌĂяo�����ŊǗ����邱��
 * �E�A�C�e�����X�N���[�����ꂽ��������
 *
 */
void CTouchListCtrl::DrawItemFocusRect( const int nItem )
{
	CRect rctItem;
	GetItemRect( nItem , rctItem , LVIR_BOUNDS);
	CDC* pdc = GetDC();
	pdc->DrawFocusRect( rctItem );
	ReleaseDC(pdc);
}

/**
 * �p���X�N���[���I���҂�
 *
 * ���X�N���[�����I������̂�҂�
 * dwMilliseconds�F(I)�F�^�C���A�E�g����(ms)
 * �߂�l�F	TRUE�F�p���X�N���[�����I������
 *			FALSE�F�^�C���A�E�g���G���[
 */
BOOL CTouchListCtrl::WaitForPanScroll( DWORD dwMilliseconds )
{
	BOOL bRtn = TRUE;
	if( m_bCanPanScroll && m_bUsePanScrollAnimation ){
		DWORD dwRtn = WaitForSingleObject( m_hPanScrollEvent , dwMilliseconds );
		MZ3_TRACE( L"WaitForSingleObject(0X%08X):%d\n" , m_hPanScrollEvent , dwRtn );
		if( dwRtn != WAIT_OBJECT_0 ) {
			bRtn = FALSE;
		}
	}
	return bRtn;
}

void CTouchListCtrl::DrawItemToBackSurface(int nItem)
{
	// �w�i�͕`��ς݂Ȃ̂ŕ`�悵�Ȃ�
	// �iDrawItem()�Ŕw�i�`�悳���ƁA�Y����������̂Łj
	SetDrawBk( false );

	// DRAWITEMSTRUCT���ł��������Ĕh���N���X��DrawItem()�����܂�
	DRAWITEMSTRUCT dis;
	CRect rctItem;
	GetItemRect( nItem , &rctItem , LVIR_BOUNDS );

//	MZ3_TRACE(L" DrawItemToBackSurface, %d, %d, %d, %d, %d\n",
//		rctItem.left, rctItem.top, rctItem.right, rctItem.bottom, m_drawStartTopOffset);

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

	// �{���̃A�C�e���`����g�p���邱�ƂŎ��ۂ̉�ʂƕ`����e����v������
	DrawItem( &dis );

	SetDrawBk( true );
}

void CTouchListCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
//	MZ3_TRACE(L"CTouchListCtrl::OnDrawItem, %d, %d\n",
//		nIDCtl, lpDrawItemStruct->itemID);

	CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
