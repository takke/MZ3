/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// ForcedDebugDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "ForcedDebugDlg.h"


// CForcedDebugDlg ダイアログ

IMPLEMENT_DYNAMIC(CForcedDebugDlg, CDialog)

CForcedDebugDlg::CForcedDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CForcedDebugDlg::IDD, pParent)
	, m_bStop(false)
	, drawing(false)
{

}

CForcedDebugDlg::~CForcedDebugDlg()
{
}

void CForcedDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CForcedDebugDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CForcedDebugDlg メッセージ ハンドラ

/// return generated random value between [from,to].
int rand_range( int from, int to ) {
	int range = to - from +1;
	return rand() % range + from;
}

#define N_BUG 10

BOOL CForcedDebugDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifndef WINCE
	// サイズ初期化
	WINDOWPLACEMENT    wp;
    if (GetWindowPlacement(&wp)) {
		SetWindowPos(NULL, wp.rcNormalPosition.left, wp.rcNormalPosition.top, 
			480, 640, SWP_SHOWWINDOW);
	}

#endif

	CRect rect;
	GetClientRect( &rect );
	int w = rect.Width();
	int h = rect.Height();

	// init force
	force.w  = 100;
	force.h  = 20;
	force.dx = 10;	// speed
	force.init( w, h );

	// init ball
	Ball ball;
	ball.alive = true;
	ball.w = 10;
	ball.h = 10;

	CPoint ball_zone( w/2 - ball.w/2, 120 );

	for( int i=0; i<N_BUG; i++ ) {
		ball.x = ball_zone.x + rand_range(-100,100);
		ball.y = ball_zone.y + rand_range(-100,100);

		ball.dx = rand_range( -2, 2 );
		ball.dy = rand_range(  2, 3 );
		balls.push_back(ball);
	}

	// start working thread
	AfxBeginThread( Ticker, this );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

unsigned int CForcedDebugDlg::Ticker( LPVOID This ) 
{
	CForcedDebugDlg* pDlg = (CForcedDebugDlg*) This;

	::Sleep( 1000L );
	pDlg->Invalidate( FALSE );

	while( !pDlg->m_bStop ) {

		static const DWORD dwWait = 30L;
		::Sleep( dwWait );

		if( !pDlg->drawing ) {
			pDlg->drawing = true;

			pDlg->NextFrame();
			pDlg->Invalidate( FALSE );
		}
	}

	return 0;
}

BOOL CForcedDebugDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message ) {
	case WM_KEYDOWN:
		switch( pMsg->wParam ) {
		case VK_RETURN:
			break;

		case VK_UP:
			force.dx ++;
			break;

		case VK_DOWN:
			force.dx --;
			break;
		}
		return TRUE;

	case WM_KEYUP:
		switch( pMsg->wParam ) {
		case VK_F1:
			OnOK();
			break;

		case VK_F2:
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CForcedDebugDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

void CForcedDebugDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
	m_bStop = true;
}

bool CForcedDebugDlg::NextFrame(void)
{
	CRect rect;
	GetClientRect( &rect );
	int w = rect.Width();
	int h = rect.Height();

	bool bKeyRight = (GetAsyncKeyState( VK_RIGHT ) & 0x8000) != 0;
	bool bKeyLeft  = (GetAsyncKeyState( VK_LEFT  ) & 0x8000) != 0;

	// force move
	// right
	if( bKeyRight ) {
		force.moveRight();
	}
	// left
	if( bKeyLeft ) {
		force.moveLeft();
	}

	// correct pos.
	if( force.x + force.w/2 > w ) {
		force.x = w - force.w/2;
	}
	if( force.x - force.w/2 < 0 ) {
		force.x = force.w/2;
	}

	// move balls
	for( unsigned int i=0; i<balls.size(); i++ ) {
		Ball& ball = balls[i];
		if( ball.alive ) {
			ball.move();
		}
	}

	// collect/refleft balls
	for( unsigned int i=0; i<balls.size(); i++ ) {
		Ball& ball = balls[i];
		if( ball.alive ) {
			// collect balls drop out
			if( ball.y > h ) {
				ball.alive = false;
				continue;
			}

			// reflection by side walls
			if( ball.x+ball.w/2 > w || ball.x-ball.w/2 < 0 ) 
			{
				ball.dx = -ball.dx;
				continue;
			}

			// reflection by top wall
			if( ball.y -ball.h/2 < 0 ) {
				ball.dy = -ball.dy;
			}

			// reflection by force
			// modify dx by left/right key
			if( force.left() <= ball.x && ball.x <= force.right() &&
				force.top() <= ball.y && ball.y <= force.bottom() && 
				ball.dy > 0 )
			{
				// modify dx by left key
				if( bKeyLeft ) {
					ball.dx -= 1;
				}

				// modify dx by right key
				if( bKeyRight ) {
					ball.dx += 1;
				}

				// reflect
				ball.dy = -ball.dy;

				// speed up/down
				if( bKeyLeft || bKeyRight ) {
					// speed up
					ball.dy --;
				}else{
					// speed down when not lowest.
					ball.dy ++;
					if( ball.dy > -2 ) {
						ball.dy = -1;
					}
				}
			}

		}
	}


	// remove died balls from ball list
	int removedBallCount = 0;
	for( unsigned int i=0; i<balls.size(); ) {
		Ball& ball = balls[i];
		if( !ball.alive ) {
			balls.erase( balls.begin()+i );
			removedBallCount ++;
		}else{
			i++;
		}
	}

	// append balls
	/*
	if( removedBallCount > 0 ) {
		// init ball
		Ball ball;
		ball.alive = true;
		ball.w = 10;
		ball.h = 10;
		ball.dx = 1;
		ball.dy = 1;

		CPoint bug_zone( w/2 - ball.w/2, 120 );

		for( int i=0; i<removedBallCount; i++ ) {
			ball.x = bug_zone.x + rand_range(-100,100);
			ball.y = bug_zone.y + rand_range(-100,100);
			ball.dx = rand_range( -2, 2 );
			ball.dy = rand_range(  1, 2 );
			balls.push_back(ball);
		}
	}
	*/

	return true;
}

void CForcedDebugDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ここにメッセージ ハンドラ コードを追加します。
	// 描画メッセージで CDialog::OnPaint() を呼び出さないでください。

	// clear screen
	CRect rect;
	GetClientRect( &rect );
	dc.FillSolidRect( rect, RGB(128,255,128) );

	// status
	CString s;
	CRect r( 10, 20, 200, 20+100 );
	dc.SetBkMode( TRANSPARENT );

	// ball count
	s.Format( L"balls : %d", balls.size() );
	r.top    = 20;
	r.bottom = r.top + 100;
	dc.DrawText( s, r, DT_LEFT );

	// pos,speed
	s.Format( L"%d,%d", force.x, force.dx );
	r.top    = 50;
	r.bottom = r.top + 100;
	dc.DrawText( s, r, DT_LEFT );

	// draw ball
	for( unsigned int i=0; i<balls.size(); i++ ) {
		Ball& ball = balls[i];
		if( ball.alive ) {
			dc.Ellipse( ball.x-ball.w/2, ball.y-ball.h/2, ball.x+ball.w/2, ball.y+ball.h/2 );
		}
	}

	// draw force
	dc.Rectangle( force.x-force.w/2, force.y-force.h/2, force.x+force.w/2, force.y+force.h/2 );

	drawing = false;
}

void CForcedDebugDlg::OnOK()
{
	m_bStop = true;

	CDialog::OnOK();
}
