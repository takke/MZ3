/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// Ran2View.cpp : 実装ファイル
//
#include "stdafx.h"
#include "Ran2View.h"
#include "resourceppc.h"

#ifndef WINCE
	#include <gdiplus.h>
	using namespace Gdiplus;
#endif

IMPLEMENT_DYNAMIC(Ran2View, CWnd)

// 汎用カラー(システムの純色だけどあとで調整できる様に定義しておく)
const static COLORREF solidBlack = COLORREF(RGB(0x00,0x00,0x00));
const static COLORREF solidBlue = RGB(0x00,0x00,0xFF);
const static COLORREF solidWhite = RGB(0xFF,0xFF,0xFF);
const static COLORREF solidPink = RGB(0xFF,0xC0,0xCB);
const static COLORREF solidDarkBlue = RGB(0x00,0x00,0x8B);
const static COLORREF lightBlue = COLORREF(RGB(0x94,0xD2,0xF1));
const static COLORREF lightGray = COLORREF(RGB(0xD0,0xD0,0xD0));

// タイマーID
#define TIMERID_AUTOSCROLL	1
#define TIMERID_ANIMEGIF	2
#define TIMERID_PANSCROLL	3

// タイマーインターバル [msec]
#define TIMER_INTERVAL_AUTOSCROLL	20L
#define TIMER_INTERVAL_ANIMEGIF		100L
#define	TIMER_INTERVAL_PANSCROLL	10L


const int emojiFixHeight = 16;	// いまのところ固定長の絵文字しかないのでこれでおｋ？

/// プロパティのコンストラクタ
MainInfo::MainInfo() : rowInfo(NULL)
{
	// アンカー位置は-1で初期化
	memset(this->anchorIndex,0xFFFFFFFF,sizeof(int)*PageAnchorMax);
	this->anchorIndex[0] = 0;	// アンカーの無いデータもあるので0行目を必ず設定

	rowInfo = new CPtrArray();
}

/// プロパティのデストラクタ
MainInfo::~MainInfo()
{
	if( rowInfo !=NULL ){
		for(int i=0 ; i<rowInfo->GetSize() ; i++){
			RowProperty* row = (RowProperty*)rowInfo->GetAt(i);
			delete row;
		}
		delete rowInfo;
	}
}


/// プロパティのコンストラクタ
RowProperty::RowProperty()
{
	rowNumber = 0;			// 描画上の行番号
	anchorIndex = 0;		// ページ内アンカーの通し番号
	indentLevel = -1;		// インデントの階層(-1:インデントなし、0:レベル0指定、1:レベル2指定)
	breakLimitPixel =0;		// 折り返し位置のピクセル数
	isPicture = false;		// 画像フラグ

	textProperties = new CPtrArray();
	gaijiProperties = new CPtrArray();
	linkProperties = new CPtrArray();
	imageProperty.imageNumber = -1;

}

/// プロパティのデストラクタ
RowProperty::~RowProperty()
{
	if( textProperties != NULL ){
		for(int i=0 ; i<textProperties->GetSize() ; i++){
			TextProperty* record = (TextProperty*)textProperties->GetAt(i);
			delete record;
		}
		delete textProperties;
	}

	if( gaijiProperties != NULL ){
		for(int i=0 ; i<gaijiProperties->GetSize() ; i++){
			GaijiProperty* record = (GaijiProperty*)gaijiProperties->GetAt(i);
			delete record;
		}
		delete gaijiProperties;
	}

	if( linkProperties != NULL ){
		for(int i=0 ; i<linkProperties->GetSize() ; i++){
			LinkProperty* record = (LinkProperty*)linkProperties->GetAt(i);
			delete record;
		}
		delete linkProperties;
	}
}


/// プロパティのコンストラクタ
ImageProperty::ImageProperty()
{
}

/// プロパティのデストラクタ
ImageProperty::~ImageProperty()
{
}


/// プロパティのコンストラクタ
LinkProperty::LinkProperty()
{
}

/// プロパティのデストラクタ
LinkProperty::~LinkProperty()
{
}


/// プロパティのコンストラクタ
FrameProperty::FrameProperty()
{
}

/// プロパティのデストラクタ
FrameProperty::~FrameProperty()
{
}


/// プロパティのコンストラクタ
UnderLineProperty::UnderLineProperty()
{
}

/// プロパティのデストラクタ
UnderLineProperty::~UnderLineProperty()
{
}


/// プロパティのコンストラクタ
GaijiProperty::GaijiProperty()
{
}

/// プロパティのデストラクタ
GaijiProperty::~GaijiProperty()
{
}


/// プロパティのコンストラクタ
TextProperty::TextProperty()
{
	fontType = FontType_normal;
	isBold = false;
	foregroundColor  = solidBlack;
	backgroundColor = solidWhite;
	isUpHanging = false;
	isDownHanging = false;
	isUnderLine = false;
	linkID = -1;			// リンク連番の初期化
	imglinkID = -1;			// 画像リンク連番の初期化
	movlinkID = -1;			// 動画リンク連番の初期化
}

/// プロパティのデストラクタ
TextProperty::~TextProperty()
{
}


/// プロパティのコンストラクタ
BridgeProperty::BridgeProperty()
{
	fontType = FontType_normal;	// フォントの種別
	isBold = false;					// 太字の有無(true:有り、false:無し)
	foregroundColor = solidBlack;	// 文字色
	backgroundColor = solidWhite;	// 背景色
	isUpHanging = false;			// 上付き有無(true:有り、false:無し)
	isDownHanging = false;			// 下付き有無(true:有り、false:無し)
	isUnderLine = false;			// 下線の有無(true:有り、false:無し)
	isLink = false;
	linkType = LinkType_noLink;
	jumpID = -1;					// リンクの飛び先ID
	pageAnchor = -1;				// リンクの飛び先IDのページ内アンカー
	inPageAnchor = -1;				// 現在ページ内のアンカー
	indentLevel = -1;				// インデントの位置
	picLine = 0;					// 画像の分割した行位置
}

/// プロパティのデストラクタ
BridgeProperty::~BridgeProperty()
{
}


/// プロパティのコンストラクタ
BigBridgeProperty::BigBridgeProperty()
{
	remainStr = TEXT("");
	frameNestLevel = -1;	// ワクがあるときは0～1の値をとる
	linkID = 0;			// リンク連番の初期化
	imglinkID = 0;		// 画像リンク連番の初期化
	movlinkID = 0;		// 動画リンク連番の初期化
	frameTopThrough = false;
	for(int i=0 ; i<FrameNestLevel ; i++){
		frameProperty[i].backgroundColor = solidWhite;
		frameProperty[i].penColor = solidWhite;
		frameProperty[i].frameType = FrameType_nothing;
	}
}

/// プロパティのデストラクタ
BigBridgeProperty::~BigBridgeProperty()
{
}


/// 描画コントロール「らんらん」コンストラクタ
Ran2View::Ran2View()
	: m_bDragging(false)
	, m_dragStartLine(0)
	, m_offsetPixelY(0)
	, m_dwFirstLButtonUp(0)
	, m_pImageList(NULL)
	, m_drawStartTopOffset(0)
	, m_viewLineMax(0)
	, topOffset(0)		// 画面上部からの余白
	, normalFont(NULL)
	, boldFont(NULL)
	, qFont(NULL)
	, qBoldFont(NULL)
	, oldFont(NULL)
	, parsedRecord(NULL)
#ifndef WINCE
	, m_isAnime(false)
#endif
	// パンスクロール用
	, m_offsetPixelX(0)				// 横スクロール表示オフセット値
	, m_dPxelX(0)					// 横スクロール差分
	, m_bPanDragging(false)			// 横ドラッグ中フラグ
	, m_bScrollDragging(false)		// 縦ドラッグ中フラグ
	, m_dwPanScrollLastTick(0)		// パンスクロール開始時刻
	
	// 描画用メンバー
#ifndef WINCE
	, m_graphics(NULL)
#endif
	, m_memDC(NULL)
	, m_memBMP(NULL)
	, m_memPanDC(NULL)
	, m_memPanBMP(NULL)

	// パンスクロール中フラグ
	, m_bAutoScrolling(false)
{
	// メンバの初期化
	// 画面解像度を取得
	currentDPI = this->GetScreenDPI();

	// 汎用ペンの作成
	underLinePen.CreatePen(PS_SOLID,1,solidBlack);
	DarkBlueunderLinePen.CreatePen(PS_SOLID,1,solidDarkBlue);

	// 汎用ブラシの作成
	blueBrush.CreateSolidBrush(solidBlue);
}


/// デストラクタ
Ran2View::~Ran2View()
{
	if( parsedRecord != NULL ){
		this->PurgeMainRecord();
	}
}


BEGIN_MESSAGE_MAP(Ran2View, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL Ran2View::RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASS			wc;
	wc.lpszClassName	= TEXT("RAN2WND");
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= AfxWndProc;
	wc.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon			= NULL;
	wc.lpszMenuName		= NULL;
	wc.hbrBackground	= (HBRUSH)::GetStockObject(WHITE_BRUSH);
//	wc.hbrBackground	= (HBRUSH)::GetStockObject(GRAY_BRUSH);
	wc.style			= CS_GLOBALCLASS;
	wc.cbClsExtra		= NULL;
	wc.cbWndExtra		= NULL;

	BOOL bRC = FALSE;
	bRC = (::RegisterClass(&wc) != 0);
	return(bRC);
}


BOOL Ran2View::UnregisterWndClass(HINSTANCE hInstance)
{
	BOOL bRC = FALSE;
	bRC = ::UnregisterClass(TEXT("RAN2WND"), hInstance);

	return(bRC);
}


// Ran2View メッセージ ハンドラ

BOOL Ran2View::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("\r\n★Create起動時の残メモリ:%d KB\r\n"),memState.dwAvailPhys/1024);
#endif

	CPaintDC	cdc(pParentWnd);	// ダイアログのクライアント領域をベースとする。

	screenWidth  = rect.right - rect.left;
	screenHeight = rect.bottom - rect.top;

	m_memDC = new CDC();
	m_memPanDC = new CDC();

	// バックバッファの生成
	if (!MyMakeBackBuffers(cdc)) {
		return FALSE;
	}

	CRect r = rect;
	r.right = 100;
	r.bottom = 100;

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, r, pParentWnd, nID, pContext);
}


/**
 * サイズ変更イベント
 *
 * 内部データの再構築
 */
void Ran2View::OnSize(UINT nType, int cx, int cy)
{
	screenWidth = cx;
	screenHeight = cy;

	if (charHeight + charHeightOffset > 0) {
		m_viewLineMax = (screenHeight / (charHeight + charHeightOffset));	// 0オリジンなので注意！
	}
#ifdef DEBUG
	wprintf( L"OnSize\n" );
	wprintf( L"m_viewLineMax : %d\n", m_viewLineMax );
#endif

	// バックバッファのサイズが小さい場合は再生成
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
	if (bmp.bmWidth < screenWidth ||
		bmp.bmHeight < screenHeight*2) 
	{
		CPaintDC	cdc(GetParent());	// ダイアログのクライアント領域をベースとする。
		MyMakeBackBuffers(cdc);
	}

	CWnd::OnSize(nType, cx, cy);
}


/**
 * バックバッファの生成
 */
bool Ran2View::MyMakeBackBuffers(CPaintDC& cdc)
{
	//--- 解放
#ifndef WINCE
	if( m_graphics != NULL ){
		delete m_graphics;
	}
#endif

	// バックバッファの解放
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
	}

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
	m_memBMP = new CBitmap();

	// パンスクロール用バッファの解放
	if( m_memPanDC != NULL ){
		m_memPanDC->DeleteDC();
	}

	if( m_memPanBMP != NULL ){
		m_memPanBMP->DeleteObject();
		delete m_memPanBMP;
	}
	m_memPanBMP = new CBitmap();

	
	//--- バッファ生成
	// バックバッファの確保
	// 画面の高さをn倍して余裕をもたせてみた
	if (m_memBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight*2) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}
	m_drawStartTopOffset = screenHeight/2;

	m_memDC->CreateCompatibleDC(&cdc);
	m_memDC->SetBkMode(OPAQUE);	// 透過モードに設定
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	// パンスクロール用バッファの確保
	// 画面の大きさ分だけ確保する
	if (m_memPanBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}

	m_memPanDC->CreateCompatibleDC(&cdc);
	m_memPanDC->SetBkMode(OPAQUE);	// 透過モードに設定
	m_oldPanBMP = m_memPanDC->SelectObject(m_memPanBMP);

#ifndef WINCE
	m_graphics = new Graphics(m_memDC->m_hDC);
#endif

	return true;
}


/**
 * ウィンドウ破棄イベント
 */
BOOL Ran2View::DestroyWindow()
{
#ifndef WINCE 
	m_isAnime = false;
	KillTimer(TIMERID_ANIMEGIF);

	if( m_graphics != NULL ){
		//m_graphics->ReleaseHDC(m_memDC->m_hDC);
		delete m_graphics;
	}
#endif

	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
		delete m_memDC;
	}

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}

	// パンスクロール用バッファの解放
	if( m_memPanDC != NULL ){
		m_memPanDC->DeleteDC();
		delete m_memPanDC;
	}

	if( m_memPanBMP != NULL ){
		m_memPanBMP->DeleteObject();
		delete m_memPanBMP;
	}

	if( normalFont != NULL ){
		normalFont->DeleteObject();
		delete normalFont;
	}

	if( boldFont != NULL ){
		boldFont->DeleteObject();
		delete boldFont;
	}

	if( qFont != NULL ){
		qFont->DeleteObject();
		delete qFont;
	}

	if( qBoldFont != NULL ){
		qBoldFont->DeleteObject();
		delete qBoldFont;
	}

#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("★Create終了時の残メモリ:%d KB\r\n"),memState.dwAvailPhys/1024);
#endif

	return CWnd::DestroyWindow();
}


/**
 * 表示カラムの変更
 *
 * @param newHeight	新フォントの文字高
 * @return 変更後フォントの文字高
 */
int	Ran2View::ChangeViewFont(int newHeight, LPCTSTR szFontFace)
{
	// 既にフォントが選択済みなら戻す。
	if( oldFont != NULL ){
		m_memDC->SelectObject(oldFont);
		oldFont = NULL;
		delete normalFont;
		delete boldFont;
		delete qFont;
		delete qBoldFont;
	}
	normalFont = new CFont;
	boldFont = new CFont;
	qFont = new CFont;
	qBoldFont = new CFont();

#ifndef WM2003
	BYTE fontQuality = NONANTIALIASED_QUALITY;
	if( IsVGA() == true ){
		fontQuality = CLEARTYPE_QUALITY;
	}

	VERIFY(normalFont->CreateFont(
	   newHeight,					// nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_MEDIUM,					// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

	VERIFY(boldFont->CreateFont(
	   newHeight,					// nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_BOLD,						// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

	VERIFY(qFont->CreateFont(
	   (int)(newHeight*0.7),		// nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_MEDIUM,					// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

	VERIFY(qBoldFont->CreateFont(
	   (int)(newHeight*0.7),		// nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_BOLD,						// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

#else
	BYTE fontQuality = NONANTIALIASED_QUALITY;
	if( IsVGA() == true ){
		fontQuality = CLEARTYPE_QUALITY;
	}

	VERIFY(normalFont->CreateFont(
	   newHeight,					// nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_MEDIUM,					// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

	VERIFY(boldFont->CreateFont(
	   newHeight,					// nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_BOLD,						// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

	VERIFY(qFont->CreateFont(
	   newHeight*0.7,               // nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_MEDIUM,					// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

	VERIFY(qBoldFont->CreateFont(
	   newHeight*0.7,               // nHeight
	   0,							// nWidth
	   0,							// nEscapement
	   0,							// nOrientation
	   FW_BOLD,						// nWeight
	   FALSE,						// bItalic
	   FALSE,						// bUnderline
	   0,							// cStrikeOut
	   DEFAULT_CHARSET,				// nCharSet
	   OUT_DEFAULT_PRECIS,			// nOutPrecision
	   CLIP_DEFAULT_PRECIS,			// nClipPrecision
	   fontQuality,					// nQuality
	   DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
	   szFontFace));				// lpszFacename

#endif
	// 1/4フォントの文字幅と高さを規定
	oldFont = m_memDC->SelectObject(qFont);
	CSize	charQSize = m_memDC->GetTextExtent(CString(TEXT("W"))); 
	charQHeight = charQSize.cy;
	charQSize = m_memDC->GetTextExtent(CString(TEXT("●"))); 
	charQWidth = charQSize.cx;

	// Boldの幅を取得する
	oldFont = m_memDC->SelectObject(boldFont);

	// 「●」を規定の文字として文字幅と高さを規定
	CSize	charSize = m_memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = m_memDC->GetTextExtent(CString(TEXT("●"))); 
	boldCharWidth = charSize.cx;

	// 通常サイズのフォントを設定
	oldFont = m_memDC->SelectObject(normalFont);
	// 「●」を規定の文字として文字幅と高さを規定
	charSize = m_memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = m_memDC->GetTextExtent(CString(TEXT("●"))); 
	charWidth = charSize.cx;
	currentCharWidth = charWidth;

	// スペーシング幅の取得
	ABC	abcInfo;
	m_memDC->GetCharABCWidths(TEXT('■'),TEXT('■'),&abcInfo);
	charSpacing = abcInfo.abcA + abcInfo.abcC;

	// 画面右側の余白量の設定
	NormalWidthOffset = (int)(charWidth * 1.3);

	// 上付き、下付きの突き出し量を設定
	hangingOffset = charHeight / 2;

	// 上端、左端、ワクの余白を設定
	leftOffset = charWidth / 4;
	topOffset = charHeight / 4;
	frameOffset = charHeight / 2;

	// 行間のオフセット量
	charHeightOffset = lineVirtualHeightPixel;

	// 外字のオフセット量
	gaijiWidthOffset = 1;
//	charRealHeightOffset = realLineHeightPixel;

	// VGA/WVGA対応機は固定値のピクセル数を2倍するのを忘れずに！
	if( IsVGA() == true ){
		charHeightOffset *= 2;
		gaijiWidthOffset *= 2;
	}

	// 特に補正とかがなければそのまま設定した値を返す。
	return(newHeight);
}


/**
 * 描画イベント
 */
void Ran2View::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	DrawToScreen(&dc);
}


/**
 * 描画
 *
 * オフスクリーンから物理デバイスへの転送
 */
void Ran2View::DrawToScreen(CDC* pDC)
{
	int sx = 0;
	int dx = 0;
	int wid = screenWidth;

	// パンスクロール処理
	if( m_offsetPixelX > 0 ){
		// 右にずれている場合
		sx = 0;
		dx = m_offsetPixelX;
		wid = screenWidth - m_offsetPixelX;

		// 変更前画面を左側に表示する
		pDC->BitBlt( 0 , 0 , m_offsetPixelX , screenHeight , m_memPanDC , wid , 0 , SRCCOPY );

		// 移動がわかるように縦線を引く
		pDC->MoveTo( m_offsetPixelX - 1 , 0);
		pDC->LineTo( m_offsetPixelX - 1 , screenHeight );
	} else if( m_offsetPixelX < 0 ) {
		// 左にずれている場合
		sx = -m_offsetPixelX;
		dx = 0;
		wid = screenWidth + m_offsetPixelX;

		// 変更前画面を右側に表示する
		pDC->BitBlt( wid , 0 , sx , screenHeight , m_memPanDC , 0 , 0 , SRCCOPY );

		// 移動がわかるように縦線を引く
		pDC->MoveTo( wid + 1 , 0);
		pDC->LineTo( wid + 1 , screenHeight );
	}

	// オフセットが一画面分あれば表示不要
	if( abs( m_offsetPixelX ) > abs( screenWidth ) ){
		return;
	}

	// 変更後画面をオフセットに合わせて表示する
//	TRACE( L"m_drawStartTopOffset,m_offsetPixelY : %5d %5d\n", m_drawStartTopOffset,m_offsetPixelY );
	pDC->BitBlt( dx, 0, wid, screenHeight, m_memDC, sx, m_drawStartTopOffset -m_offsetPixelY, SRCCOPY );
//	pDC->Rectangle(0, 0, screenWidth, screenHeight);
}


/**
 * 表示幅一杯まで何文字入るかを再計算し続ける。
 *
 * @param srcStr 対象の文字列
 * @param width	 表示幅
 * @return 表示幅内に収まる長さの文字列
 */
CString	Ran2View::CalcTextByWidth(CDC* dstDC,CString srcStr,int width)
{
	int	length = 2;	// 先頭からの文字数
	CString dstStr = srcStr;	// 戻り値用の文字列
	bool isExit = false;

	// 0文字の場合はヌルー
	if( length <= 0 )
		return(dstStr);

	// 短い文字列で範囲内にバシっと収まるならそのまま返す
	if( dstDC->GetTextExtent(dstStr).cx < width ){
		return(dstStr);
	}

	// 先頭から表示幅内に収まるまで長さを計測
	dstStr = srcStr.Left(length);

	while( !isExit ){
		dstStr = srcStr.Left(length - 1); 
		if( dstDC->GetTextExtent(dstStr).cx > width ){
			// はみ出した最終位置から1文字引いた形で返す
			dstStr = srcStr.Left(length - 2); 
			isExit = true;
		}
		// 文字数の上限を超えた場合も抜ける
		if( srcStr.GetLength() < length ){
			isExit = true;
		}
		length++;
	}

	return(dstStr);
}


/**
 * 行情報の設定
 *
 * ParseDatData2 から呼び出される
 *
 * @param hashRecord    振り分けるタグ情報
 * @param rowRecord     振り分け対象先
 * @param bridgeInfo    行末記号(p,h1,h2,h2,h3の終了)でリセットされる文字修飾情報
 * @param bigBridgeInfo end記号(end_kakomi_hoge)でリセットされる行修飾情報(いまのところ枠線のみ)
 *
 * @return ProcessState_EndOfLine:      閉じタグで行が終了 <br>
 *         ProcessState_BeginOfLine:    開始タグで新行が必要 <br>
 *         follow:                      情報を継続して次行 <br>
 *         ProcessState_ForceBreakLine: 情報を継続して改行 <br>
 */
ProcessStateEnum Ran2View::SetRowProperty(HtmlRecord* hashRecord, 
										  RowProperty* rowRecord,
										  BridgeProperty* bridgeInfo,
										  BigBridgeProperty* bigBridgeInfo)
{
	static int imageDummyLineCount = 0; // 画像の空行出力計測用
	ProcessStateEnum processState = ProcessState_through; 
	bool isBreak = false;		// 行単位の処理が終わる度にレコードを書き出すフラグ

	// タグ情報の振り分け
	TAG_TYPE currentTag = hashRecord->type;

	switch (currentTag) {
	case Tag_p:
	case Tag_h1:
	case Tag_h2:
	case Tag_h3:
	case Tag_level0:
	case Tag_level1:
	case Tag_level2:
	case Tag_blockquote:
		// 開始タグ
		processState = ProcessState_BeginOfLine;
		break;

	case Tag_end_p:
	case Tag_end_h1:
	case Tag_end_h2:
	case Tag_end_h3:
		// 終了タグ
		processState = ProcessState_EndOfLine;
		break;

	case Tag_br:
		// 改行とかがあった時
		processState = ProcessState_ForceBreakLine;
		break;

	case Tag_nothing:
		// エラータグの場合はここでスキップ
		return(ProcessState_error);
	}

	// 行を跨いでも変わらない情報から処理
	// 囲みの設定(ワク線無し、背景が薄青)
	if( currentTag == Tag_kakomi_blue || currentTag == Tag_h1 || currentTag == Tag_h2 ){
		bigBridgeInfo->frameNestLevel++;
		int nestLevel = bigBridgeInfo->frameNestLevel;

		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_open;	// ワクの種別
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = lightBlue;		// 背景色
		bigBridgeInfo->frameProperty[nestLevel].penColor = lightBlue;				// 枠色
	// 囲みの設定(ワク線無し、背景が薄灰色)
	}else if( currentTag == Tag_kakomi_gray ){
		bigBridgeInfo->frameNestLevel++;
		int nestLevel = bigBridgeInfo->frameNestLevel;

		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_open;	// ワクの種別
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = lightGray;		// 背景色
		bigBridgeInfo->frameProperty[nestLevel].penColor = lightGray;				// 枠色

	// 囲みの設定(ワク線が青、背景が薄灰色)
	}else if( currentTag == Tag_kakomi_gray2 ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_roof;		// ワクの種別
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = lightGray;		// 背景色
		bigBridgeInfo->frameProperty[nestLevel].penColor = lightBlue;				// 枠色
		bigBridgeInfo->frameTopThrough = false;

	// 囲みの設定(ワク線が黒、背景が白色)
	}else if( currentTag == Tag_kakomi_white ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_roof;		// ワクの種別
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = solidWhite;		// 背景色
		bigBridgeInfo->frameProperty[nestLevel].penColor = solidBlack;				// 枠色
		bigBridgeInfo->frameTopThrough = false;

	// 囲みの設定(ワク線が青、背景が白色)
	}else if( currentTag == Tag_kakomi_white2 ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_roof;		// ワクの種別
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = solidWhite;		// 背景色
		bigBridgeInfo->frameProperty[nestLevel].penColor = solidBlue;				// 枠色
		bigBridgeInfo->frameTopThrough = false;

	// 囲みの設定(<blockquote>:ワク線なし、背景がピンク)
	}else if( currentTag == Tag_blockquote ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_open;		// ワクの種別
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = solidPink;		// 背景色
		bigBridgeInfo->frameProperty[nestLevel].penColor = solidPink;				// 枠色
		bigBridgeInfo->frameTopThrough = false;
	}

	// ワク情報の転記
	for(int i=0 ; i<FrameNestLevel ; i++){
		rowRecord->frameProperty[i].frameType = bigBridgeInfo->frameProperty[i].frameType;
		rowRecord->frameProperty[i].backgroundColor = bigBridgeInfo->frameProperty[i].backgroundColor;
		rowRecord->frameProperty[i].penColor = bigBridgeInfo->frameProperty[i].penColor;
	}

	// 囲みの終了(ワクあり)
	if( currentTag == Tag_end_kakomi_gray2 || currentTag == Tag_end_kakomi_white ||
			currentTag == Tag_end_kakomi_white2 || currentTag == Tag_end_blockquote ){

		int currentlevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[currentlevel].frameType = FrameType_stool;

		bigBridgeInfo->frameNestLevel--;

	// 囲みの終了(ワク無し)
	}else if( currentTag == Tag_end_kakomi_blue || currentTag == Tag_end_kakomi_gray ||
			currentTag == Tag_end_kakomi_gray2 || currentTag == Tag_end_kakomi_white ||
			currentTag == Tag_end_kakomi_white2 || currentTag == Tag_end_h1 || 
			currentTag == Tag_end_h2 ){

		// ワク情報のクリア 
		int currentlevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[currentlevel].frameType = FrameType_nothing;
		bigBridgeInfo->frameProperty[currentlevel].backgroundColor = solidWhite;
		bigBridgeInfo->frameProperty[currentlevel].penColor = solidWhite;
		bigBridgeInfo->frameNestLevel--;
	}

	// ページ内アンカーの情報
	if( currentTag == Tag_anchor ){
		bridgeInfo->inPageAnchor = _wtol(hashRecord->value.GetBuffer(0));

	// インデントレベルの設定(行またぎあり)
	}else if( currentTag == Tag_level0 ){
		bridgeInfo->indentLevel = 0;

	}else if( currentTag == Tag_level1 ){
		bridgeInfo->indentLevel = 1;

	}else if( currentTag == Tag_level2 ){
		bridgeInfo->indentLevel = 2;

	// bool値を持つ属性の設定
	}else if( currentTag == Tag_sub ){
		bridgeInfo->isDownHanging = true;

	}else if( currentTag == Tag_sup ){
		bridgeInfo->isUpHanging = true;

	// 太字の設定
	}else if( currentTag == Tag_bold ){
		bridgeInfo->isBold = true;

	// 文字色の設定
	}else if( currentTag == Tag_blue ){
		bridgeInfo->foregroundColor = solidBlue;

	// 下線の開始
	}else if( currentTag == Tag_underline ){
		bridgeInfo->isUnderLine = true;

	// 下付きの終了
	}else if( currentTag == Tag_end_sub ){
		bridgeInfo->isDownHanging = false;
	// 上付きの終了
	}else if( currentTag == Tag_end_sup ){
		bridgeInfo->isUpHanging = false;

	// 太字の終了
	}else if( currentTag == Tag_end_bold ){
		bridgeInfo->isBold = false;

	// 文字色の設定終了
	}else if( currentTag == Tag_end_blue ){
		bridgeInfo->foregroundColor = solidBlack;

	// 下線の終了
	}else if( currentTag == Tag_end_underline ){
		bridgeInfo->isUnderLine = false;

	// 文字列の設定
	}else if( currentTag == Tag_text || currentTag == Tag_entity ||  bigBridgeInfo->remainStr.GetLength() > 0){
//		processState = ProcessState_ForceBreakLine;	// 基本は一行単位で改行する
		processState = ProcessState_through;	// 基本は改行なしとする。

		TextProperty* newText = new TextProperty();
		CString srcStr;
		// 持ち越しがある場合は再設定
		if( bigBridgeInfo->remainStr.GetLength() > 0 ){
			srcStr = bigBridgeInfo->remainStr;
		}else{
			srcStr = hashRecord->value;
		}

		// テキストと実体参照は最終的な出力ができなかった場合、次の行へ出力情報を送る
		int remainWidth = bigBridgeInfo->remainWidth;

		// 文字幅の計測前にフォントの種別をbridgeInfoから取得して適切なフォントに切り替える
		// 太字の時
		if( bridgeInfo->isBold == true ){
			newText->isBold = true;
			if( bridgeInfo->isUpHanging == true || bridgeInfo->isDownHanging == true ){
				// フォント修飾情報の登録
				if( bridgeInfo->isUpHanging == true )
					newText->isUpHanging = true;
				else
					newText->isDownHanging = true;

				oldFont = m_memDC->SelectObject(qBoldFont);
				currentCharWidth = charQWidth;
				newText->fontType = FontType_quarter;
			}else{
				oldFont = m_memDC->SelectObject(boldFont);
				currentCharWidth = boldCharWidth;
			}
		}else{
			if( bridgeInfo->isUpHanging == true || bridgeInfo->isDownHanging == true ){
				// フォント修飾情報の登録
				if( bridgeInfo->isUpHanging == true )
					newText->isUpHanging = true;
				else
					newText->isDownHanging = true;

				oldFont = m_memDC->SelectObject(qFont);
				currentCharWidth = charQWidth;
				newText->fontType = FontType_quarter;
			}else{
				oldFont = m_memDC->SelectObject(normalFont);
				currentCharWidth = charWidth;
			}
		}
		CString cutStr = this->CalcTextByWidth(m_memDC,srcStr,remainWidth);

		// 一行に収まらない場合は持ち越し
		if( cutStr.GetLength() < srcStr.GetLength() ){
			bigBridgeInfo->remainStr = srcStr.Mid(cutStr.GetLength());

			if( bigBridgeInfo->remainStr.GetLength() > 0 ){
				// 持ち越した場合は持ち越し分が完了するまで繰り返す
				processState = ProcessState_FollowOfLine;
			}
	
		}else{
			bigBridgeInfo->remainStr = TEXT("");	// 持ち越し文字列をクリア
		}

		newText->foregroundColor = bridgeInfo->foregroundColor;
		newText->backgroundColor = bridgeInfo->backgroundColor;

		// ワク線ありの場合はワク内の背景色を優先に設定
		if( bigBridgeInfo->frameNestLevel >= 0 ){
			newText->backgroundColor = bigBridgeInfo->frameProperty[bigBridgeInfo->frameNestLevel].backgroundColor;
		}

		// 一行に収まる場合は出力に消費した文字列の長さを限界から差し引く
		int blockWidth = m_memDC->GetTextExtent(cutStr).cx;
		bigBridgeInfo->remainWidth -= blockWidth;

		// 文字幅を計算したのでフォントを戻す
		m_memDC->SelectObject(oldFont);

		// 対象文字列の転記
		newText->lineText = cutStr;

		// 出力領域の設定
		int sx = bigBridgeInfo->startWidth + gaijiWidthOffset;

		// ワクがある場合は開始位置をずらす
		if( bigBridgeInfo->frameNestLevel >= 0 ){
			sx += (bigBridgeInfo->frameNestLevel+1) * (leftOffset+framePixel);
		}

		CRect drawRect = CRect(sx,0,sx+blockWidth,0);	// Y座標は実行時に解釈されるので不要
		newText->drawRect = drawRect; 

		// 下線がある場合はフラグを立てる
		if( bridgeInfo->isUnderLine == true ){
			newText->isUnderLine = true;
		}

		// 次のテキストの開始座標を更新
		bigBridgeInfo->startWidth += blockWidth;

		// 文字列情報の追記
//		TRACE(TEXT("%d行の%d項目[%s]を追加しました\r\n"),
//			rowRecord->rowNumber,rowRecord->textProperties->GetSize(),newText->lineText);
		
		if( newText->lineText.GetLength() > 0 ){
			// テキストごとにリンクIDを設定する
			newText->linkID = -1;
			newText->imglinkID = -1;
			newText->movlinkID = -1;
			// リンクがある場合はフラグを立ててタップ反応領域を登録
			if( bridgeInfo->isLink == true ){
				newText->isUnderLine = true;
				newText->foregroundColor = solidDarkBlue;

				LinkProperty* newLink = new LinkProperty();

				// リンク情報の取得
				newLink->linkType = bridgeInfo->linkType;
				newLink->jumpUID = bridgeInfo->jumpID;
				newLink->anchorIndex = bridgeInfo->pageAnchor;

				// リンク連番を登録時にインクリメント
				newLink->linkID = bigBridgeInfo->linkID;
				newLink->imglinkID = bigBridgeInfo->imglinkID;
				newLink->movlinkID = bigBridgeInfo->movlinkID;

				// 描画する領域をリンク情報として登録
				CRect tapRect = CRect(sx,0,sx+blockWidth,0);	// Y座標は実行時に解釈されるので不要
				newLink->grappleRect = tapRect;
				rowRecord->linkProperties->Add(newLink);
				// リンク種別によりリンク連番を設定する
				switch( bridgeInfo->linkType ){
					case LinkType_external:
						// リンク
						newText->linkID = bigBridgeInfo->linkID;
						break;
					case LinkType_picture:
						// 画像リンク
						newText->imglinkID = bigBridgeInfo->imglinkID;
						break;
					case LinkType_movie:
						// 動画リンク
						newText->movlinkID = bigBridgeInfo->movlinkID;
						break;
				}
			}
			rowRecord->textProperties->Add(newText);
		}else{
			delete newText;
		}

	// 外字の設定
	}else if( currentTag == Tag_gaiji ){
		GaijiProperty* newGaiji = NULL;

//		int blockWidth = charWidth;	// 文字幅と同一とする
		int blockWidth = emojiFixHeight + gaijiWidthOffset;	// 絵文字は16ピクセル固定（CImageListを利用するため）

#ifndef WINCE
		// 文字の大きさに合わせて拡大(絵文字以下のフォントの場合はそのまんまなので重なるかもね)
		if( emojiFixHeight < charHeight ){
			double gaijiScale = (double)charHeight / (double)emojiFixHeight;
			blockWidth = (int)(blockWidth*gaijiScale);
		}
#endif
		// 外字が幅に収まらない場合は改行して再チャレンジ支援
		if( blockWidth > bigBridgeInfo->remainWidth ){
			// 持ち越した場合は持ち越し分が完了するまで繰り返す
			processState = ProcessState_FollowOfLine;
		}else{
			newGaiji = new GaijiProperty();
			// [m:xx] から xx を抽出し、リソースIDとする
			newGaiji->resourceID = hashRecord->value.Mid(3, hashRecord->value.GetLength()-4);	// リソース名の置換を行う場合はここでやっちゃって！

			// 出力領域の設定
			int sx = bigBridgeInfo->startWidth + (gaijiWidthOffset);
			CRect drawRect = CRect(sx,0,sx+blockWidth,0);	// Y座標は実行時に解釈されるので不要
			newGaiji->drawRect = drawRect;

			// 次のテキストの開始座標を更新(外字はカツカツなので文字のスペーシングを前後に入れる)
			bigBridgeInfo->startWidth += (blockWidth + gaijiWidthOffset);
			bigBridgeInfo->remainWidth -= (blockWidth + gaijiWidthOffset);
			// 配列に追記
			rowRecord->gaijiProperties->Add(newGaiji);
		}
	// リンクの設定
	}else if( currentTag == Tag_link ){
		// リンクの領域登録はtextの領域作成時にまとめて行う
		bridgeInfo->isLink = true;
//		bridgeInfo->jumpID = 0;

		// 同一IDなら内部リンクとして扱う // 初期化していないので判定しない
		//if( bridgeInfo->jumpID == currentUIDNumber ){
		//	bridgeInfo->linkType = LinkType_internal;
		//}else{
			bridgeInfo->linkType = LinkType_external;	// リンクタイプ＝外部リンク
		//}

//		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
		bridgeInfo->pageAnchor = 0;
	// リンクタグの終了
	}else if( currentTag == Tag_end_link ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
		bigBridgeInfo->linkID++;	// リンク連番をインクリメント
	// 画像リンクの設定
	}else if( currentTag == Tag_img ){
		// リンクの領域登録はtextの領域作成時にまとめて行う
		bridgeInfo->isLink = true;
//		bridgeInfo->jumpID = 0;

		bridgeInfo->linkType = LinkType_picture;		// リンクタイプ＝画像リンク

//		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
		bridgeInfo->pageAnchor = 0;
	// 画像リンクタグの終了
	}else if( currentTag == Tag_end_img ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
		bigBridgeInfo->imglinkID++;		// 画像リンク連番をインクリメント
	// 動画リンクの設定
	}else if( currentTag == Tag_mov ){
		// リンクの領域登録はtextの領域作成時にまとめて行う
		bridgeInfo->isLink = true;
//		bridgeInfo->jumpID = 0;

		bridgeInfo->linkType = LinkType_movie;			// リンクタイプ＝動画リンク

//		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
		bridgeInfo->pageAnchor = 0;
	// 動画リンクタグの終了
	}else if( currentTag == Tag_end_mov ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
		bigBridgeInfo->movlinkID++;		// 動画リンク連番をインクリメント
	}else{
		// それ以外のエラーの場合
		//CString logStr;
		//logStr.Format(TEXT("Through tag!!(%s:%s)\r\n"),hashRecord->key,hashRecord->value);
		//OutputDebugString(logStr);
	}

	return(processState);
}



/// 新行情報の追加
void Ran2View::AddNewRowProperty(CPtrArray* rowPropertyArray,bool forceNewRow)
{
	RowProperty* newRowRecord = new RowProperty();
	newRowRecord->rowNumber = rowPropertyArray->GetSize(); // 行番号を設定
	newRowRecord->breakLimitPixel = screenWidth - NormalWidthOffset - (leftOffset*2);

	// framePropertyを初期化
	newRowRecord->frameProperty[0].frameType = FrameType_nothing;
	newRowRecord->frameProperty[1].frameType = FrameType_nothing;

	rowPropertyArray->Add(newRowRecord);
}


/// 大跨ぎ情報のリセット
void Ran2View::ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,BridgeProperty* bridgeInfo,ProcessStateEnum mode,int width)
{
	// 出力可能幅と開始位置だけリセット
	if( width != 0 ){
		bigBridgeInfo->screenWidth = width;		// 画面のクライアント領域を設定
	}
	bigBridgeInfo->remainWidth = (screenWidth - NormalWidthOffset - (leftOffset*2)) - ((leftOffset+framePixel)*3*(bigBridgeInfo->frameNestLevel+1));
	bigBridgeInfo->startWidth = leftOffset;

	// BOLだけ先頭の突き出し分オフセットするので注意！
	if( mode == ProcessState_BeginOfLine ){
		// インデントが設定されている時は突き出し量を設定
		if( bridgeInfo->indentLevel != -1 ){
			// 開始行だけ突き出して出力する(開始位置が一文字左)
			int indentOffset = (bridgeInfo->indentLevel) * (charWidth);
			// ワクがある場合は始点を更にずらす
			indentOffset += (bigBridgeInfo->frameNestLevel+1) * (charWidth); 

			bigBridgeInfo->startWidth += indentOffset;
			bigBridgeInfo->remainWidth -= indentOffset;
		}
	}else{
		// インデントが設定されている時は突き出し量を設定
		if( bridgeInfo->indentLevel != -1 ){
			// インデント量の計算
			int indentOffset = ((bridgeInfo->indentLevel+1) * (charWidth));
			// ワクがある場合は始点を更にずらす
			indentOffset += (bigBridgeInfo->frameNestLevel+1) * (charWidth); 

			bigBridgeInfo->startWidth += indentOffset;
			bigBridgeInfo->remainWidth -= indentOffset;
		}
	}
}


/// ワクのtop/bottom終了すりかえ
void Ran2View::ChangeFrameProperty(BigBridgeProperty* bigBridgeInfo)
{
	// ワクのtopを差し替える
	for(int i=0 ; i<FrameNestLevel ; i++){
		if( bigBridgeInfo->frameProperty[i].frameType  == FrameType_roof ){
			bigBridgeInfo->frameProperty[i].frameType = FrameType_follow;
		}
		if( bigBridgeInfo->frameProperty[i].frameType  == FrameType_stool ){
			bigBridgeInfo->frameProperty[i].frameType = FrameType_nothing;
		}
	}
}


/**
 * DATファイルからのデータ構築
 */
int Ran2View::LoadDetail(CStringArray* bodyArray, CImageList* pImageList)
{
#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("★LoadDetail起動時の残メモリ:%d bytes\r\n"),memState.dwAvailPhys);
#endif

	m_pImageList = pImageList;

	int rc = 0;
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	m_offsetPixelY = 0;
	m_offsetPixelX = 0;			// 横スクロールオフセットを初期化

	// パンスクロール用に直前の表示状態のコピーを取る（一画面分）
	//m_memPanDC->FillSolidRect( 0 , 0 , screenWidth , screenHeight , RGB(128,128,128) );
	m_memPanDC->BitBlt( 0 , 0 , screenWidth , screenHeight , m_memDC , 0 , m_drawStartTopOffset , SRCCOPY );

	if( bodyArray != NULL ){
		PurgeMainRecord();
		parsedRecord = ParseDatData2(bodyArray,this->screenWidth); // datの変換処理
		bodyArray->FreeExtra();
		bodyArray->RemoveAll();
//		delete bodyArray;
	}

	// 描画に必要な行数を返す
	if( parsedRecord != NULL ){
		rc = parsedRecord->rowInfo->GetSize() - m_viewLineMax; 
	}
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));


#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("★LoadDetail終了時の残メモリ:%d Bytes\r\n"),memState.dwAvailPhys);
#endif

#ifndef WINCE
	// GIFアニメは慣性スクロールの1/3ぐらいの頻度の更新でおｋ？
	SetTimer( TIMERID_ANIMEGIF, TIMER_INTERVAL_ANIMEGIF, NULL );
#endif
	return(rc);
}


/**
 * 任意の行からオフスクリーンに描画する
 */
int	Ran2View::DrawDetail(int startLine, bool bForceDraw)
{
#ifdef DEBUG
//	wprintf( L"DrawDetail: %5d line, force[%s]\n", startLine, bForceDraw ? L"true" : L"false");
#endif

	if (startLine<0) {
		// 先頭より飛び出していれば0に引き戻す
		startLine = 0;
	}

	// どの行から描画したかを保存しておく
	m_drawOffsetLine = startLine;

	// レコードの展開ミスや範囲外の指定は弾く
	if( parsedRecord == NULL || startLine > parsedRecord->rowInfo->GetSize() ){
		return(0);
	}

	//// パンスクロール用に直前の表示状態のコピーを取る（一画面分）
	////m_memPanDC->FillSolidRect( 0 , 0 , screenWidth , screenHeight , RGB(128,128,128) );
	//m_memPanDC->BitBlt( 0 , 0 , screenWidth , screenHeight , m_memDC , 0 , m_drawStartTopOffset , SRCCOPY );

	// 塗りつぶす
//	m_memDC->PatBlt( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, WHITENESS );
//	m_memDC->FillSolidRect( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, RGB(255,255,255) );
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
	m_memDC->FillSolidRect( 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255,255,255) );

//	TRACE( L"[DrawRect]\n" );
//	TRACE( L" %dx%d\n", bmp.bmWidth, bmp.bmHeight );
//	TRACE( L" %dx%d\n", screenWidth, screenHeight );

	// オフセットスクロール用にN行余分に描画する。
	const int N_OVER_OFFSET_LINES = 2;
	for(int i=-N_OVER_OFFSET_LINES; i<=m_viewLineMax+N_OVER_OFFSET_LINES ; i++){
		int targetLine = startLine + i;

		if (targetLine < 0) {
			continue;
		}

		// 範囲を越えたらスルー
		if( parsedRecord->rowInfo->GetSize() <= targetLine ){
			break;
		}

		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(targetLine);
		if( row != NULL ){
			//TRACE(TEXT("%dの描画を開始します\r\n"), targetLine);

			// フレームの描画。画像が設定されていたら描画しない
			if( row->imageProperty.imageNumber == -1 ){
				this->DrawFrameProperty(i,row);
			}

			// テキスト要素の出力(下線、リンク下線、セロハン含む)
			this->DrawTextProperty(i, row->textProperties);

			// 外字要素の出力
			this->DrawGaijiProperty(i, row->gaijiProperties);
		}

		//TRACE( L" line : %d\n", targetLine );
	}
	m_memDC->Rectangle( 0, 0, 10, 10 );

	// 描画
//	this->Invalidate(FALSE);
	if (bForceDraw) {
		// 強制的に描画する
		CDC* pDC = GetDC();

		DrawToScreen(pDC);

		ReleaseDC(pDC);
	}

	return(1);
}


/**
 * 行内のテキストプロパティの配列を描画する
 *
 * DrawDetail から呼び出される
 *
 * line: 描画したい行
 * rowProperty: 行の情報
 */
void Ran2View::DrawFrameProperty(int line,RowProperty* rowProperty)
{
	for(int i=0 ;i<FrameNestLevel ; i++){
		// ワクの描画が設定されている時のみ描画
		if(	rowProperty->frameProperty[i].frameType != FrameType_nothing ){
			CRect drawRect;
			// 背景色の描画
			CBrush backBrush(rowProperty->frameProperty[i].backgroundColor);
			int sx = leftOffset + (i * frameOffset);
			int ex = (screenWidth - NormalWidthOffset) - leftOffset - (i * frameOffset);
			int sy = m_drawStartTopOffset + topOffset + (line*(charHeight+charHeightOffset));
			int ey = sy + (charHeight+charHeightOffset);
			drawRect = CRect(sx,sy,ex,ey);
			m_memDC->FillRect(drawRect,&backBrush);

			CPen framePen(PS_SOLID,1,rowProperty->frameProperty[i].penColor);
			// 上端と左右を描画
			if( rowProperty->frameProperty[i].frameType == FrameType_roof ){

				CPen* oldPen = m_memDC->SelectObject(&framePen);
				m_memDC->MoveTo(drawRect.left,drawRect.top);
				m_memDC->LineTo(drawRect.right,drawRect.top);

				m_memDC->MoveTo(drawRect.left,drawRect.top);
				m_memDC->LineTo(drawRect.left,drawRect.bottom);

				m_memDC->MoveTo(drawRect.right,drawRect.top);
				m_memDC->LineTo(drawRect.right,drawRect.bottom);

				m_memDC->SelectObject(oldPen);
			// 末端を遡って描画
			}else if( rowProperty->frameProperty[i].frameType == FrameType_stool ){
				int sx = leftOffset + (i * frameOffset);
				int ex = (screenWidth - NormalWidthOffset) - leftOffset - (i * frameOffset);
				int sy;
				if( rowProperty->textProperties->GetCount() == 0 ){
					// 終行は次の行と連結されてしまうので-1行する
					sy = m_drawStartTopOffset + topOffset + ((line-1)*(charHeight+charHeightOffset));
				} else {
					// 終行にテキストがある場合はそのまま
					sy = m_drawStartTopOffset + topOffset + ((line)*(charHeight+charHeightOffset));
				}
				int ey = sy + (charHeight+charHeightOffset);
				CRect drawRect = CRect(sx,sy,ex,ey);

				CPen* oldPen = m_memDC->SelectObject(&framePen);
				if( rowProperty->textProperties->GetCount() != 0 ){
					// 終行にテキストがある場合は左右の枠線を描画する
					m_memDC->MoveTo(drawRect.left,drawRect.top);
					m_memDC->LineTo(drawRect.left,drawRect.bottom);

					m_memDC->MoveTo(drawRect.right,drawRect.top);
					m_memDC->LineTo(drawRect.right,drawRect.bottom);
				}
				m_memDC->MoveTo(drawRect.left,drawRect.bottom-framePixel);
				m_memDC->LineTo(drawRect.right,drawRect.bottom-framePixel);
				m_memDC->SelectObject(oldPen);

			// 左右の枠線だけ描画
			}else if( rowProperty->frameProperty[i].frameType == FrameType_follow ){

				CPen* oldPen = m_memDC->SelectObject(&framePen);
				m_memDC->MoveTo(drawRect.left,drawRect.top);
				m_memDC->LineTo(drawRect.left,drawRect.bottom);

				m_memDC->MoveTo(drawRect.right,drawRect.top);
				m_memDC->LineTo(drawRect.right,drawRect.bottom);

				m_memDC->SelectObject(oldPen);
			}

		}
	}
	
}


/**
 * 行内のテキストプロパティの配列を描画する
 *
 * DrawDetail から呼び出される
 *
 * @param line 描画したい行
 * @param textProperties textPropertyをまとめた配列
 */
void Ran2View::DrawTextProperty(int line,CPtrArray* textProperties)
{
	bool bReverse = false;
	for(int j=0 ; j<textProperties->GetSize() ; j++){
		// テキストブロックの出力(後で関数化する)
		TextProperty* text = (TextProperty*)textProperties->GetAt(j);
		int sy = m_drawStartTopOffset + topOffset + framePixel + (line*(charHeight+charHeightOffset));
		// 上付き指定の場合は表示位置をずらす
		if( text->isDownHanging == true ){
			sy += (charHeight - charQHeight - framePixel);
		}
		int ey = sy + (charHeight+charHeightOffset);

		bReverse = false;
		if( ((m_activeLinkID.anchor >=0) && (m_activeLinkID.anchor == text->linkID) ) ||
			((m_activeLinkID.image >=0) && (m_activeLinkID.image == text->imglinkID) ) ||
			((m_activeLinkID.movie >=0) && (m_activeLinkID.movie == text->movlinkID) ) )
		{
			// ポイントしているリンク連番がテキスト情報のものと一致していれば反転フラグを設定
			bReverse = true;
		}

		CRect drawRect = CRect(text->drawRect.left,sy,text->drawRect.right,ey);
		if( bReverse ){
			// 反転表示用に黒く塗りつぶす
			m_memDC->FillSolidRect( &drawRect , solidBlack );
		}

		// アンダーラインの描画
		if( text->isUnderLine == true ){ 
			CPen* oldPen = NULL;
			if( text->foregroundColor == solidDarkBlue ){
				oldPen = m_memDC->SelectObject(&DarkBlueunderLinePen);
			} else {
				oldPen = m_memDC->SelectObject(&underLinePen);
			}
			m_memDC->MoveTo(drawRect.left, drawRect.bottom-charHeightOffset);
			m_memDC->LineTo(drawRect.right, drawRect.bottom-charHeightOffset);
			oldPen = m_memDC->SelectObject(oldPen);
		}

		// 文字色とフォントの切り替え
		if( bReverse ) {
			// 反転表示用 文字色＝白、背景色＝黒
			m_memDC->SetTextColor(text->backgroundColor);
			m_memDC->SetBkColor(text->foregroundColor);
		} else {
			// ノーマル表示用 テキスト情報から色を取得
			m_memDC->SetTextColor(text->foregroundColor);
			m_memDC->SetBkColor(text->backgroundColor);
		}

		if( text->isBold == true ){
			if( text->isUpHanging == true || text->isDownHanging == true ){
				oldFont = m_memDC->SelectObject(qBoldFont);
			}else{
				oldFont = m_memDC->SelectObject(boldFont);
			}
		}else{
			if( text->isUpHanging == true || text->isDownHanging == true ){
				oldFont = m_memDC->SelectObject(qFont);
			}else{
				oldFont = m_memDC->SelectObject(normalFont);
			}
		}

		m_memDC->DrawText(text->lineText,-1,drawRect,DT_LEFT | DT_NOPREFIX);
		m_memDC->SelectObject(oldFont);
	}
}


/**
 * 行内のテキストプロパティの配列を描画する
 *
 * DrawDetail から呼び出される
 *
 * @param line 描画したい行
 * @param textProperties textPropertyをまとめた配列
 */
void Ran2View::DrawGaijiProperty(int line,CPtrArray* gaijiProperties)
{
	// 外字の描画(後で関数にする)
	for(int j=0 ; j<gaijiProperties->GetSize() ; j++){
		// テキストブロックの出力(後で関数化する)
		GaijiProperty* gaiji = (GaijiProperty*)gaijiProperties->GetAt(j);
		int sy = m_drawStartTopOffset + topOffset + framePixel + (line*(charHeight+charHeightOffset));

		if (m_pImageList!=NULL) {
			int imageIdx = _wtoi(gaiji->resourceID);
			// 長さチェック
			if (0 <= imageIdx && imageIdx < m_pImageList->GetImageCount()) {
#ifdef WINCE
				m_pImageList->Draw( m_memDC, imageIdx, CPoint(gaiji->drawRect.left, sy), ILD_TRANSPARENT );
#else
				CString imagePath = theApp.m_imageCache.GetImagePath(imageIdx);

				// ran2ImageArray から探索
				// 絵文字は高々３００弱なので全検索しちゃう
				Ran2Image* targetImage = NULL;
				for (int ran2ImageArrayIdx=0; ran2ImageArrayIdx<ran2ImageArray.GetCount(); ran2ImageArrayIdx++) {
					Ran2Image* image = (Ran2Image*)ran2ImageArray.GetAt(ran2ImageArrayIdx);
					if (image->m_strFilename == imagePath) {
						targetImage = image;
						break;
					}
				}
				if (targetImage==NULL) {
					// 新規生成
					targetImage = new Ran2Image(imagePath);
					ran2ImageArray.Add(targetImage);
				}

				Ran2Image* image = targetImage;
				if( imagePath.GetLength() > 0 && image->GetWidth() > 0 && image->GetHeight()){

					// 文字の大きさに合わせて拡大(絵文字以下のフォントの場合はそのまんまなので重なるかもね)
					int hmWidth = image->GetWidth();
					int hmHeight = image->GetHeight();
					if( hmWidth < charHeight ){
						double gaijiScale = (double)charHeight / (double)hmHeight;
						hmWidth  = (int)(hmWidth *gaijiScale);
						hmHeight = (int)(hmHeight*gaijiScale);
					}

					// 絵文字の高さが文字より大きい場合、ベースラインを変更する
					int baseLineOffset = 0;
					if( image->GetHeight() > charHeight ){
						baseLineOffset = ((image->GetHeight()+charHeightOffset) - charHeight) / 2;
					}

					//image->InitAnimation(m_graphics,CPoint(gaiji->drawRect.left, sy-baseLineOffset),charHeight);
					m_graphics->DrawImage(image,gaiji->drawRect.left,sy-baseLineOffset,hmWidth,hmHeight);
					if( image->IsAnimatedGIF() == true ){
						m_isAnime = true;
					}
				}
#endif
			}
		}
	}
}


/**
 * datファイルから実行時クラスへの変換その2(Unicodeに変換されている事が前提)
 *
 * LoadDetail から呼び出される
 *
 * ファイルをCArchiveで一行づつ読むのではなく、一括で読み込んでCStringArrayへ分割してから処理を行う
 */
MainInfo* Ran2View::ParseDatData2(CStringArray* datArray,int width)
{
	// 返却値
	MainInfo* newMainRecord = new MainInfo();

	// 実ライン情報の一時保存先
	CPtrArray*	rowPropertyArray = newMainRecord->rowInfo;

	// Main.uniファイルの終端まで到着するまで繰り返す
	CString	lineStr;
	int		indentLevel = -1;	 // 入れ子レベル

	int lineNo = 0;
	int frameNestLevel = -1;		// ワクのネスト描画レベル
	int	anchorCount = 0;			// ページ内アンカーの個数
	BridgeProperty		bridgeInfo;		// 行をまたぐ修飾情報(新行でリセットされる)
	BigBridgeProperty	bigBridgeInfo;	// 行をまたぐ修飾情報(新行でもリセットされない)

	// 跨ぎ情報のリセット
	this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BeginOfLine,width);

	// CFileArchiveでファイルの終末までループさせる
	// 一番最初の行をからっぽで追加
	// (このレコードには必ず0のアンカーが入るはずなので最後にチェックをかけて確認すること！)
	RowProperty* topRowRecord = new RowProperty;
	topRowRecord->breakLimitPixel = screenWidth - NormalWidthOffset - (leftOffset*2);

	rowPropertyArray->Add(topRowRecord);

	int readCount = 0;
	for( int i=0 ; i<datArray->GetSize() ; i++ ){
		CString lineStr = datArray->GetAt(i);
		// 要素がない場合はスキップ
		if( lineStr.GetLength() <= 0 ){
			continue;
		}

		// ここをHtml用に修正！！
		HtmlRecord*	hashRecord = new HtmlRecord();

		// 文字、絵文字の振り分け
		if( wcsncmp(lineStr, TEXT("[m:"), 3) == 0){
			// 数値, インデックスチェックが通れば外字、それ以外はテキストとみなす
			CString code = lineStr.Mid(3,lineStr.GetLength()-4);
			for (int i=0; i<code.GetLength(); i++) {
				if (!isdigit(code[i])) {
					code = L"";
				}
			}
			if (code.IsEmpty()) {
				// 非外字
				hashRecord->type = Tag_text;
			} else {
				// 外字
				hashRecord->type = Tag_gaiji;
			}
		}else if( lineStr.Compare(TEXT("<br>")) == 0 ) {
			hashRecord->type = Tag_br;
		}else if( lineStr.Compare(TEXT("<b>")) == 0 ) {
			hashRecord->type = Tag_bold;
		}else if( lineStr.Compare(TEXT("</b>")) == 0 ) {
			hashRecord->type = Tag_end_bold;
		}else if( lineStr.Compare(TEXT("<blue>")) == 0 ) {
			hashRecord->type = Tag_blue;
		}else if( lineStr.Compare(TEXT("</blue>")) == 0 ) {
			hashRecord->type = Tag_end_blue;
		}else if( lineStr.Compare(TEXT("<blockquote>")) == 0 ) {
			// 引用開始
			hashRecord->type = Tag_blockquote;
		}else if( lineStr.Compare(TEXT("</blockquote>")) == 0 ) {
			// 引用終了
			hashRecord->type = Tag_end_blockquote;
		}else if( lineStr.Compare(TEXT("<a>")) == 0 ) {
			// リンク開始
			hashRecord->type = Tag_link;
		}else if( lineStr.Compare(TEXT("</a>")) == 0 ) {
			// リンク終了
			hashRecord->type = Tag_end_link;
		}else if( lineStr.Compare(TEXT("<img>")) == 0 ) {
			// 画像リンク開始
			hashRecord->type = Tag_img;
		}else if( lineStr.Compare(TEXT("</img>")) == 0 ) {
			// 画像リンク終了
			hashRecord->type = Tag_end_img;
		}else if( lineStr.Compare(TEXT("<mov>")) == 0 ) {
			// 動画リンク開始
			hashRecord->type = Tag_mov;
		}else if( lineStr.Compare(TEXT("</mov>")) == 0 ) {
			// 動画リンク終了
			hashRecord->type = Tag_end_mov;
		}else{
			hashRecord->type = Tag_text;
		}
		hashRecord->value = lineStr;

		// 既存の配列の最後尾を取得
		int lastRowPropertyIndex = rowPropertyArray->GetUpperBound();
		RowProperty* currentRowRecord = NULL;
		if( lastRowPropertyIndex >= 0 ){
			currentRowRecord = (RowProperty*)rowPropertyArray->GetAt(lastRowPropertyIndex);

			if( currentRowRecord == NULL ){
				break; // ループを終了
			}
		}

		// 行情報の振り分け処理
		ProcessStateEnum rc = this->SetRowProperty(hashRecord, currentRowRecord, &bridgeInfo, &bigBridgeInfo);

		// ページ内アンカーの行位置をキャッシュする
		if( bridgeInfo.inPageAnchor != -1 ){
			newMainRecord->anchorIndex[bridgeInfo.inPageAnchor] = rowPropertyArray->GetSize() - 1;
		}

		// 開始タグはインデント位置の再設定だけ
		if( rc == ProcessState_BeginOfLine ){

			// 大またぎ情報をリセット
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BeginOfLine);
		// 持ち越し情報の繰り返し処理の場合 
		}else if( rc == ProcessState_FollowOfLine ){
			// 持ち越しじゃなくなるまで繰り返し
			while( rc == ProcessState_FollowOfLine ){
				//TRACE(TEXT(" 繰越中[%d]\r\n"),rowPropertyArray->GetSize());
				// 新行情報の追加
				this->AddNewRowProperty(rowPropertyArray);
				int lastRowPropertyIndex = rowPropertyArray->GetUpperBound();
				RowProperty* currentRowRecord = NULL;
				if( lastRowPropertyIndex >= 0 ){
					currentRowRecord = (RowProperty*)rowPropertyArray->GetAt(lastRowPropertyIndex);

					if( currentRowRecord == NULL ){
						break; // ループを終了
					}
				}

				// 大またぎ情報をリセット
				this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
				// ワクのtop/bottomの状態を差し替える
				this->ChangeFrameProperty(&bigBridgeInfo);
				// 折り返し行を繰り返し描画
				rc = this->SetRowProperty(hashRecord, currentRowRecord, &bridgeInfo, &bigBridgeInfo);
			}

			// 新行情報の追加
//			this->AddNewRowProperty(rowPropertyArray);
			// 大またぎ情報をリセット
//			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ワクのtop/bottomの状態を差し替える
			this->ChangeFrameProperty(&bigBridgeInfo);

			//TRACE(TEXT(" 繰越終了[%d]\r\n"),rowPropertyArray->GetSize());
		// 強制改行は新規レコードをNewして追記 
		}else if( rc == ProcessState_ForceBreakLine ){
			// 新行情報の追加
			this->AddNewRowProperty(rowPropertyArray);
			// 大またぎ情報をリセット
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ワクのtop/bottomの状態を差し替える
			this->ChangeFrameProperty(&bigBridgeInfo);

		// 終了タグは新しいレコードをNewして追記後に行またぎ情報をリセット
		}else if( rc == ProcessState_EndOfLine ){
			// 新行情報の追加
			this->AddNewRowProperty(rowPropertyArray);
			// 行またぎ情報をリセット
			memset(&bridgeInfo,NULL,sizeof(BridgeProperty));
			bridgeInfo.foregroundColor = solidBlack;
			bridgeInfo.backgroundColor = solidWhite;
			bridgeInfo.fontType = FontType_normal;
			bridgeInfo.indentLevel = -1;
			bridgeInfo.pageAnchor = -1;
			bridgeInfo.inPageAnchor = -1;

			// 大またぎ情報をリセット
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ワクのtop/bottomの状態を差し替える
			this->ChangeFrameProperty(&bigBridgeInfo);
		}
		readCount++;
		
		delete hashRecord;
	}

	// MainInfo、RowPropertyの順番でメモリの内容をファイルへダンプ
	newMainRecord->propertyCount = rowPropertyArray->GetSize();

	return(newMainRecord);

}


/**
 * mainRecordの破棄
 */
void Ran2View::PurgeMainRecord()
{
	// 行情報の破棄
	if( parsedRecord != NULL ){
		TRACE(TEXT("PurgeMainRecord:int %d records.\r\n"),parsedRecord->rowInfo->GetSize());

		for(int i=0 ; i<parsedRecord->rowInfo->GetSize() ; i++){
//			TRACE(TEXT(" Purge.. :%d\r\n"),i);
			RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(i);
			delete row;
		}
		parsedRecord->rowInfo->FreeExtra();
		parsedRecord->rowInfo->RemoveAll();
		delete parsedRecord->rowInfo;
		parsedRecord->rowInfo = NULL;

		delete parsedRecord;
		parsedRecord = NULL;

#ifndef WINCE
		// Ran2Imageの破棄もここでやっとく
//		for(int cacheIndex=0 ; cacheIndex<ran2ImageArray.GetSize() ; cacheIndex++){
//			Ran2Image* image = (Ran2Image*)ran2ImageArray.GetAt(cacheIndex);
//			delete image;
//		}
//		ran2ImageArray.RemoveAll();
#endif
	}
}


// 現在位置を再描画
void Ran2View::Refresh()
{
	this->DrawDetail(m_drawOffsetLine);
}

/*
bool Ran2View::IsPoratrait()
{
    bool fPoratrait;

    if( ::GetSystemMetrics(SM_CYSCREEN) >= ::GetSystemMetrics(SM_CXSCREEN) ){
      fPoratrait = false;
    } else {
      fPoratrait = true;
    }

	return(fPoratrait);
}
*/


bool Ran2View::IsVGA()
{
	if( currentDPI == 192 ){
		return(true);
	}

	return(false);
}


/// DPI値をレジストリから取得
int Ran2View::GetScreenDPI()
{
	// レジストリからの設定情報など
	HKEY	hSettingKey;
	CString msgStr;

// PPC2003とWM5.0でDPIの設定されているところが違うので要注意！
#ifndef WM2003
	if( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("\\System\\GWE\\Display"),0,KEY_ALL_ACCESS,&hSettingKey) == ERROR_SUCCESS ){
#else
	if( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("\\Drivers\\Display\\GPE"),0,KEY_ALL_ACCESS,&hSettingKey) == ERROR_SUCCESS ){
#endif
		DWORD	regType = REG_DWORD;
		DWORD	valSize = sizeof(DWORD);
		DWORD	dispos = REG_OPENED_EXISTING_KEY;
		DWORD	DPIVal;
		// DPI値を取得
		if( hSettingKey != NULL ){
			::RegQueryValueEx(hSettingKey,TEXT("LogicalPixelsY"),NULL,&regType,(LPBYTE)&DPIVal,&valSize);
			::RegCloseKey(hSettingKey);
			return(DPIVal);
		}
	}

	// 判断できない場合は96DPIという事にしておく
	return(96);
}


//-------------------------------------------------------------------------------------------------
// 以下、User I/F (Behavior) 関連
//-------------------------------------------------------------------------------------------------


/**
 * ウィンドウプロシージャ
 *
 * 特に処理なし
 */
LRESULT Ran2View::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WindowProc(message, wParam, lParam);
}


/**
 * パンスクロール開始
 *
 * スクロール方向（direction）に従いオフセット値と差分を設定してタイマーを起動する
 */
void Ran2View::StartPanDraw(PAN_SCROLL_DIRECTION direction)
{
	if (!m_bUsePanScrollAnimation) {
		// オプションでオフになっているので無視する
		return;
	}

	KillTimer( TIMERID_PANSCROLL );

	switch (direction) {
	case PAN_SCROLL_DIRECTION_RIGHT:
		// 右方向へスクロール
		
		// 左へ一画面ずれたところから開始
		m_offsetPixelX = - screenWidth;
		// 移動差分
#ifndef WINCE
		m_dPxelX = screenWidth / 10 ;
#else
		m_dPxelX = screenWidth / 18 ;
#endif
		break;

	case PAN_SCROLL_DIRECTION_LEFT:
		// 左方向へスクロール

		// 右へ一画面ずれたところから開始
		m_offsetPixelX = screenWidth;
		// 移動差分
#ifndef WINCE
		m_dPxelX = - screenWidth / 10 ;
#else
		m_dPxelX = - screenWidth / 18 ;
#endif
		break;
	}

	// パンスクロール中フラグ設定
	m_bAutoScrolling = true;

	// パンスクロール開始時刻
	m_dwPanScrollLastTick = GetTickCount();

	// パンスクロール開始
	SetTimer( TIMERID_PANSCROLL, TIMER_INTERVAL_PANSCROLL, NULL );
}


/**
 * 縦スクロールイベント
 */
void Ran2View::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


/**
 * 左ダブルクリックイベント
 */
void Ran2View::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
}


/**
 * 自動ドラッグ処理（慣性スクロール、パンスクロール）の停止
 */
void Ran2View::ResetDragOffset(void)
{
	// 慣性スクロール停止
	KillTimer(TIMERID_AUTOSCROLL);
	
	// パンスクロール停止
	KillTimer(TIMERID_PANSCROLL);

	// パンスクロール中フラグクリア
	m_bAutoScrolling = false;

	// オフセット初期化
	m_offsetPixelY = 0;
	m_offsetPixelX = 0;
}


/**
 * 右クリック終了イベント
 */
void Ran2View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// 親の呼び出し
	::SendMessage( GetParent()->GetSafeHwnd(), WM_RBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );

	CWnd::OnRButtonUp(nFlags, point);
}


/**
 * 左クリック終了イベント
 *
 * タップによるリンク位置の探索
 */
void Ran2View::OnLButtonUp(UINT nFlags, CPoint point)
{
	int dx = m_ptDragStart.x - point.x;
	int dy = m_ptDragStart.y - point.y;

	if (m_bDragging) {
		//m_bDragging = false;		// ドラッグ中フラグは後の処理で判定に使う
		//::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
		ReleaseCapture();

		// dx,dyのドラッグ量に応じて、ドラッグ開始かどうかを判定する
		// m_bPanDragging, m_bScrollDragging が設定される
		MySetDragFlagWhenMovedPixelOverLimit(dx,dy);
	}

	// ダブルクリック判定
	if (m_dwFirstLButtonUp>0 &&
		(GetTickCount() - m_dwFirstLButtonUp) < GetDoubleClickTime())
	{
		// ダブルクリック済みなのでクリアする
		m_dwFirstLButtonUp = 0;
		// ドラッグ中フラグもクリア
		m_bDragging = false;		
		m_bPanDragging = false;
		m_bScrollDragging = false;
		m_activeLinkID.clear();

		// ダブルクリックとみなす
		// 親の呼び出し
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	} else {
		// ドラッグ完了

		if( m_bUseDoubleClickMove ) {
			// オプションがonならばダブルクリック判定
			m_dwFirstLButtonUp = GetTickCount();
		} else {
			// オプションがoffの場合ダブルクリック判定しない
			m_dwFirstLButtonUp = 0;
		}

		m_ptFirstLButtonUp = point;

		if( m_bPanDragging ){ 
			// 横方向にドラッグ
#ifdef DEBUG
			wprintf( L"OnLButtonUp: %5d\n" , dx);
#endif
			if( dx > 0 ) {
				// 左方向
				CBrush brs;
				brs.CreateSolidBrush( lightGray );
				CBrush * oldbrs = m_memDC->SelectObject( &brs );
				m_memDC->RoundRect( screenWidth  / 8 , m_drawStartTopOffset + screenHeight / 4 , screenWidth * 7 / 8 , m_drawStartTopOffset + screenHeight * 3 / 4 , 10 , 10  );
				m_memDC->SelectObject( &oldbrs );
				brs.DeleteObject();
				brs.CreateSolidBrush( RGB( 32 , 32 , 32 ) );
				oldbrs = m_memDC->SelectObject( &brs );
				CPoint pts[3];
				pts[0].x = screenWidth * 5 / 8;
				pts[0].y = m_drawStartTopOffset + screenHeight * 3 / 8;
				pts[1].x = screenWidth * 5 / 8;
				pts[1].y = m_drawStartTopOffset + screenHeight * 5 / 8;
				pts[2].x = screenWidth * 3 / 8;
				pts[2].y = m_drawStartTopOffset + screenHeight / 2;
				m_memDC->Polygon( pts , 3 );
				m_memDC->SelectObject( &oldbrs );
				brs.DeleteObject();

				// 強制的に描画する
				// 今のままでは次のコメントが無くても描画されてしまうので保留
				// （実はWM_PAINTが来たら見えてしまうのは秘密だ）
				//CDC* pDC = GetDC();
				//DrawToScreen(pDC);
				//ReleaseDC(pDC);
				// 次のコメントを表示
				::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM( 10 , 1000 ));
			} else {
				// 右方向
				CBrush brs;
				brs.CreateSolidBrush( lightGray );
				CBrush * oldbrs = m_memDC->SelectObject( &brs );
				m_memDC->RoundRect( screenWidth  / 8 , m_drawStartTopOffset + screenHeight / 4 , screenWidth * 7 / 8 , m_drawStartTopOffset + screenHeight * 3 / 4 , 10 , 10  );
				m_memDC->SelectObject( &oldbrs );
				brs.DeleteObject();
				brs.CreateSolidBrush( RGB( 32 , 32 , 32 ) );
				oldbrs = m_memDC->SelectObject( &brs );
				CPoint pts[3];
				pts[0].x = screenWidth * 3 / 8;
				pts[0].y = m_drawStartTopOffset + screenHeight * 3 / 8;
				pts[1].x = screenWidth * 3 / 8;
				pts[1].y = m_drawStartTopOffset + screenHeight * 5 / 8;
				pts[2].x = screenWidth * 5 / 7;
				pts[2].y = m_drawStartTopOffset + screenHeight / 2;
				m_memDC->Polygon( pts , 3 );
				m_memDC->SelectObject( &oldbrs );
				brs.DeleteObject();

				// 強制的に描画する
				// 今のままでは次のコメントが無くても描画されてしまうので保留
				// （実はWM_PAINTが来たら見えてしまうのは秘密だ）
				//CDC* pDC = GetDC();
				//DrawToScreen(pDC);
				//ReleaseDC(pDC);
				// 前のコメントを表示
				::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM( 10 , 0 ));
			}
		} else if( m_bScrollDragging ) {
			// 縦方向にドラッグ
			m_autoScrollInfo.push( GetTickCount(), point );
			double speed = m_autoScrollInfo.calcMouseMoveSpeedY();
			TRACE( L"! speed   : %5.3f [px/msec]\n", speed );

			KillTimer( TIMERID_AUTOSCROLL );
			m_bAutoScrolling = false;
			if( GetAllLineCount()-GetViewLineMax() > 0 ) {
				// スクロール可能ならば（と判定しないとスクロール不可能状態でタイマー処理が走る）
				// 慣性スクロール開始
				m_dwAutoScrollStartTick = GetTickCount();
				m_yAutoScrollMax = 0;
				m_bAutoScrolling = true;
				SetTimer( TIMERID_AUTOSCROLL, TIMER_INTERVAL_AUTOSCROLL, NULL );
			} else {
				// リンク情報をクリアして再表示する
				m_activeLinkID.clear();
				DrawDetail( m_drawOffsetLine , true );
			}
		} else {
			// 左クリック扱いの処理を行う

			// タップ位置の行番号を取得
			int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

			// Row配列からの取得位置を算出
			int rowNumber = m_drawOffsetLine + tapLine;

			// タップ位置が範囲内を越える場合は何もしない
			if( parsedRecord->rowInfo->GetSize() > rowNumber  && rowNumber >= 0 ){
				RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
				for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
					LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);

					CString logStr;
					logStr.Format(TEXT("リンク情報は[Type:%d][ID:%d][PA:%d][left:%d]～[right:%d]\r\n"),
						linkInfo->linkType, linkInfo->linkID , linkInfo->anchorIndex,
						linkInfo->grappleRect.left, linkInfo->grappleRect.right);
					OutputDebugString(logStr);

					// リンク表示範囲内であればリンク種別により親ウインドウを呼び出して開く
					if( linkInfo->grappleRect.left <= point.x && linkInfo->grappleRect.right >= point.x ){
						if( linkInfo->linkType == LinkType_internal ){
							// 内部（同一ユーザID？）リンク（現状未使用？）
							break;
						}else if( linkInfo->linkType == LinkType_external ){
							// 外部リンク（mixiURL、http:、ttp:、YouTube）
							::SendMessage( GetParent()->GetSafeHwnd(), WM_COMMAND, (WPARAM)MAKEWPARAM(  ID_REPORT_URL_BASE + 1 + linkInfo->linkID , 0 ), (LPARAM)NULL );
							//logStr = L"LinkType_external" + logStr;
							//MessageBox( logStr );
							break;
						}else if( linkInfo->linkType == LinkType_picture ){
							// 画像リンク（日記、コミュコメント、フォトアルバム）
							::SendMessage( GetParent()->GetSafeHwnd(), WM_COMMAND, (WPARAM)MAKEWPARAM(  ID_REPORT_IMAGE + 1 + linkInfo->imglinkID , 0 ), (LPARAM)NULL );
							//logStr = L"LinkType_picture" + logStr;
							//MessageBox( logStr );
							break;
						}else if( linkInfo->linkType == LinkType_movie ){
							// 動画リンク（mixi動画）
							::SendMessage( GetParent()->GetSafeHwnd(), WM_COMMAND, (WPARAM)MAKEWPARAM(  ID_REPORT_MOVIE + 1 + linkInfo->movlinkID , 0 ), (LPARAM)NULL );
							//logStr = L"LinkType_movie" + logStr;
							//MessageBox( logStr );
							break;
						}
					}
				}
			}
			// リンク情報をクリアして再表示する
			if( !m_bAutoScrolling ){
				m_activeLinkID.clear();
				DrawDetail( m_drawOffsetLine , true );
			}
		}

		//ドラッグ処理停止
		m_bDragging = false;
		m_bPanDragging = false;
		m_bScrollDragging = false;
		m_activeLinkID.clear();
		
		// 親の呼び出し
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	}

	//CWnd::OnLButtonUp(nFlags, point);
}


/**
 * 左クリック開始イベント
 *
 * タップによるリンクの処理
 */
void Ran2View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		return;
		m_bDragging = false;
		::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
	}

	LinkID lastLinkID = m_activeLinkID;

	// タップ位置の行番号を取得
	int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

	// Row配列からの取得位置を算出
	int rowNumber = m_drawOffsetLine + tapLine;

	bool bLinkArea = false;

	//リンク処理はOnLButtonUpで行う
//	// タップ位置が範囲内を越える場合は何もしない
	// リンク連番クリア
	m_activeLinkID.clear();
	if( parsedRecord->rowInfo->GetSize() > rowNumber && rowNumber >= 0 ){
		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
		for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
			LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);
			// リンク範囲の該当内部か？
			if( linkInfo->grappleRect.left <= point.x && linkInfo->grappleRect.right >= point.x ){
				// リンクポイント中フラグを立てる
				bLinkArea = true;
				if( m_bScrollDragging != true && m_bPanDragging != true ){
					// ドラッグ中でなければリンクタイプに合わせてリンク連番を設定する
					switch( linkInfo->linkType ){
						case LinkType_external:
							m_activeLinkID.anchor = linkInfo->linkID;
							break;
						case LinkType_picture:
							m_activeLinkID.image = linkInfo->imglinkID;
							break;
						case LinkType_movie:
							m_activeLinkID.movie = linkInfo->movlinkID;
							break;
					}
				}
			}
		}
	}

	if( lastLinkID.anchor != m_activeLinkID.anchor ||
		lastLinkID.image  != m_activeLinkID.image ||
		lastLinkID.movie  != m_activeLinkID.movie)
	{
		// リンクポイント状態に変化があれば
		// リンク反転表示用に一画面再描画→効率悪い！
		DrawDetail( m_drawOffsetLine , true );
	}

	// ドラッグ開始
	// マウスカーソル設定
	if (GetAllLineCount()-GetViewLineMax() > 0) {
		// スクロール可能
		if( bLinkArea ) {
			// リンクポイント中は指差し
			::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
		} else {
			// グラブ
			::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );
		}
	} else {
		if( bLinkArea ) {
			// リンクポイント中は指差し
			::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
		}
	}

	// キャプチャ開始
	SetCapture();

	m_bDragging = true;
	m_ptDragStart = point;
	m_dragStartLine = m_drawOffsetLine;

	// まだドラッグ方向は確定していない
	m_bPanDragging = false;
	m_bScrollDragging = false;

	// 慣性スクロール停止
	KillTimer( TIMERID_AUTOSCROLL );
	m_bAutoScrolling = false;

	// 慣性スクロール情報
	m_autoScrollInfo.clear();
	m_autoScrollInfo.push( GetTickCount(), point );

//	CWnd::OnLButtonDown(nFlags, point);
}


/**
 * 現在のスクロール位置の取得
 */
int Ran2View::MyGetScrollPos()
{
	if (m_offsetPixelY<0) {
		// pixelオフセットありの場合は「表示行+1行」を返す
		return m_drawOffsetLine+1;
	} else {
		return m_drawOffsetLine;
	}
}


/**
 * マウス移動イベント
 */
void Ran2View::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef DEBUG
//	wprintf( L"OnMouseMove\n" );
#endif

	LinkID lastLinkID    = m_activeLinkID;

	// Nピクセル移動したらダブルクリックキャンセル
	if (m_dwFirstLButtonUp!=0) {
		int dx = point.x - m_ptFirstLButtonUp.x;
		int dy = point.y - m_ptFirstLButtonUp.y;
//		TRACE( L"dx,dy = (%3d,%3d), r^2=%3d\n", dx, dy, dx*dx+dy*dy );
#ifdef WINCE
		// WindowsMobile : タップのためかなり大きなズレが生じる
		// TODO : 本来は DPI 値で自動調整かつユーザ毎に変更可能にすべき。
		const int N = 30;
#else
		// Windows : クリックのためズレはかなり小さい
		const int N = 2;
#endif
		if (dx*dx + dy*dy > N*N) {
//			CString msg;
//			msg.Format( L"ダブルクリックキャンセル, dx,dy = (%3d,%3d), r^2=%3d\n", dx, dy, dx*dx+dy*dy );
//			MZ3LOGGER_DEBUG( msg );
			m_dwFirstLButtonUp = 0;
		}
	}

	bool bLinkArea = false;

	// タップ位置の行番号を取得
	int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

	// Row配列からの取得位置を算出
	int rowNumber = m_drawOffsetLine + tapLine;
	// マウス位置が範囲内を越える場合は何もしない
	// リンク連番クリア
	m_activeLinkID.clear();
	if( parsedRecord->rowInfo->GetSize() > rowNumber  && rowNumber >= 0 ){
		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
		for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
			LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);
			// リンク表示範囲内か？
			if( linkInfo->grappleRect.left <= point.x && linkInfo->grappleRect.right >= point.x ){
				// リンクポイント中フラグを立てる
				bLinkArea = true;
				if( m_bScrollDragging != true && m_bPanDragging != true ){
					// ドラッグ中でなければリンクタイプに合わせてリンク連番を設定する
					switch( linkInfo->linkType ){
						case LinkType_external:
							m_activeLinkID.anchor = linkInfo->linkID;
							break;
						case LinkType_picture:
							m_activeLinkID.image = linkInfo->imglinkID;
							break;
						case LinkType_movie:
							m_activeLinkID.movie = linkInfo->movlinkID;
							break;
					}
				}
				//CString logStr;
				//logStr.Format(TEXT("座標[%d,%d][m_offsetPixelY:%d][charHeightOffset + charHeight:%d][tapLine:%d][drawOffsetLine:%d]\r\n"),
				//	point.x , point.y , m_offsetPixelY,
				//	charHeightOffset + charHeight, tapLine,drawOffsetLine);
				//OutputDebugString(logStr);
			}
		}
	}

	if (m_bDragging) {
		// 左ボタンを押してドラッグ中
		int dx = m_ptDragStart.x - point.x;
		int dy = m_ptDragStart.y - point.y;
#ifdef DEBUG
		if (false) {
			wprintf( L"dx : %5d\n", dx );
			wprintf( L"dy : %5d\n", dx );
		}
#endif

		// dx,dyのドラッグ量に応じて、ドラッグ開始かどうかを判定する
		// m_bPanDragging, m_bScrollDragging が設定される
		MySetDragFlagWhenMovedPixelOverLimit(dx,dy);

		if( GetAllLineCount()-GetViewLineMax() > 0 ) {
			// 縦スクロール可能ならば
			if( m_bScrollDragging ){
				// 慣性スクロール情報収集
				m_autoScrollInfo.push( GetTickCount(), point );

				// ドラッグ処理
				ScrollByMoveY( dy );
			}
		}
		
		if( m_bPanDragging ){
			// 横スクロール中ならばマウスポインタ変更
			if( dx>0 ) {
				// 左方向
				::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_LEFT_CURSOR) );
			} else {
				// 右方向
				::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_RIGHT_CURSOR) );
			}
		} else {
			// リンク表示範囲内なら指差しカーソルに変更
			if( bLinkArea ) {
				::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
			} else {
				// スクロール可能であれば「グー」のカーソルに変更
				if (GetAllLineCount()-GetViewLineMax() > 0) {
					::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );
				} else {
					::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
				}
			}
			if( !m_bScrollDragging ) {
				if (lastLinkID.anchor != m_activeLinkID.anchor ||
					lastLinkID.image != m_activeLinkID.image ||
					lastLinkID.movie != m_activeLinkID.movie)
				{
					// リンクポイント状態に変化があれば
					// リンク反転表示用に一画面再描画→効率悪い！
					DrawDetail( m_drawOffsetLine , true );
				}
			}
		}

	} else {
		// 左ボタンを離して移動中

		// リンク表示範囲内なら指差しカーソルに変更
		if( bLinkArea ) {
			::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
		} else {
			// スクロール可能であれば「パー」のカーソルに変更
			if (GetAllLineCount()-GetViewLineMax() > 0) {
				::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
			} else {
				::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
			}
		}
		// オートスクロール中でなければリンク反転表示用に再描画→効率悪い！
		if( !m_bAutoScrolling ) {
			if (lastLinkID.anchor != m_activeLinkID.anchor ||
				lastLinkID.image != m_activeLinkID.image ||
				lastLinkID.movie != m_activeLinkID.movie)
			{
				// リンクポイント状態に変化があれば
				// リンク反転表示用に一画面再描画→効率悪い！
				DrawDetail( m_drawOffsetLine, true );
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


/**
 * dx,dyのドラッグ量に応じて、ドラッグ開始かどうかを判定し、変数を設定する
 *
 * ドラッグ開始時は m_bPanDragging, m_bScrollDragging を設定する
 */
void Ran2View::MySetDragFlagWhenMovedPixelOverLimit(int dx, int dy)
{
	// 縦ドラッグ開始判断用オフセット値
#ifndef WINCE
	// win32の場合半行以内の移動はドラッグとみなさない
	int dyMinLimit = ( charHeight+charHeightOffset ) / 2 + 1 ;
#else
	// WMの場合一行以内の移動はドラッグとみなさない
	int dyMinLimit = charHeight+charHeightOffset;
#endif

	if (m_bPanDragging) {
		// 横ドラッグ中
		//if( abs( dx ) < screenWidth / 3 ){
		//	// マウスを元に戻したら横ドラッグをキャンセル　→動きがあやしいので保留
		//	m_bPanDragging = false ;
		//}
	} else if (m_bScrollDragging) {
		// 縦ドラッグ中

	} else {
		// ドラッグ方向が確定していない
		if( m_bUseHorizontalDragMove &&
			( abs(dx) > abs(dy) && abs(dx) > screenWidth / 3 ) ) {
			// 横方向の移動量が大きくて移動量が画面の1/3以上の場合
			// 横ドラッグ開始
			m_bPanDragging = true;
		} else if(  abs(dx) < abs(dy) && abs(dy) > dyMinLimit ) {
			// 縦方向の移動量が大きくて移動量がドラッグ開始オフセット以上の場合
			if( GetAllLineCount()-GetViewLineMax() > 0 ) {
				// 縦スクロール可能ならば
				// 縦ドラッグ開始
				m_bScrollDragging = true;
			}
		}
	}
}


/**
 * ドラッグ中の描画処理
 *
 * 描画範囲を超えた場合 true を返す
 */
bool Ran2View::ScrollByMoveY(int dy)
{
#ifdef DEBUG
//# define TRACE_ScrollByMoveY	wprintf
# define TRACE_ScrollByMoveY	__noop
#else
# define TRACE_ScrollByMoveY	__noop
#endif

	if (parsedRecord == NULL && parsedRecord->rowInfo == NULL ) {
		// 描画対象データがないためエラーとする
		return true;
	}

	bool bLimitOver = false;
	const int lineHeightPixel = (charHeight + charHeightOffset);	// 1行当たりのピクセル数
	int offsetLine = dy / lineHeightPixel;	// ドラッグ開始行からのオフセット行

	TRACE_ScrollByMoveY( L"---\n" );
	TRACE_ScrollByMoveY( L"m_dragStartLine + offsetLine : %5d\n", m_dragStartLine + offsetLine );
	TRACE_ScrollByMoveY( L"dy               : %5d\n", dy );
	TRACE_ScrollByMoveY( L"m_offsetPixelY   : %5d\n", m_offsetPixelY );
	TRACE_ScrollByMoveY( L"m_viewLineMax    : %5d\n", m_viewLineMax );
	TRACE_ScrollByMoveY( L"m_dragStartLine  : %5d\n", m_dragStartLine );

	if (-dy % lineHeightPixel <=0 || m_dragStartLine + offsetLine > 0) {
		// ピクセルオフセット
		// （0行目であれば上方向のスクロールは行わない）
		m_offsetPixelY = -dy % lineHeightPixel;

		TRACE_ScrollByMoveY( L"Yピクセルオフセット再計算：%d\n", m_offsetPixelY );
	} else if( m_dragStartLine + offsetLine == 0 ) {
		// 0行目ならば先頭位置を0調整
		m_offsetPixelY = 0;

		TRACE_ScrollByMoveY( L"0行目のためYピクセルオフセット初期化\n" );
		bLimitOver = true;
	}

	int nAllLine = GetAllLineCount();

	TRACE_ScrollByMoveY( L"nAllLine : %5d\n", nAllLine );
	TRACE_ScrollByMoveY( L"m_dragStartLine +offsetLine +m_viewLineMax : %5d\n", m_dragStartLine +offsetLine +m_viewLineMax );
	TRACE_ScrollByMoveY( L"m_offsetPixelY : %5d\n", m_offsetPixelY );
	TRACE_ScrollByMoveY( L"screenHeight %% lineHeightPixel : %5d\n", screenHeight % lineHeightPixel );

	// nAllLine        : データの全行数 [line]
	// m_dragStartLine : ドラッグ開始行 [line]
	// offsetLine      : ドラッグ開始行からのオフセット行数 [line]
	// m_viewLineMax   : 1画面で表示可能な行数 [line]
	// screenHeight    : 1画面の高さ [px]
	// lineHeightPixel : 1行当たりのピクセル数 [px]
	// m_offsetPixelY  : オフセットピクセル [px]

	bool bBottomLimitOver = false;
	if (m_offsetPixelY>=0) {
		if (m_dragStartLine +offsetLine +m_viewLineMax > nAllLine) {
			// 行数超過
			TRACE_ScrollByMoveY( L"行数超過(1)\n" );
			bBottomLimitOver = true;
		} else if (m_dragStartLine +offsetLine +m_viewLineMax == nAllLine && 
		           m_offsetPixelY <= screenHeight % lineHeightPixel)
		{
			// 最終行かつ下方向ピクセルオフセットあり
			TRACE_ScrollByMoveY( L"最終行かつ下方向ピクセルオフセットあり(1)\n" );
			bBottomLimitOver = true;
		}
	} else {
		// m_offsetPixelY <0 の場合：
		if (m_dragStartLine +offsetLine +m_viewLineMax+1 > nAllLine) {
			// 行数超過
			TRACE_ScrollByMoveY( L"行数超過(2)\n" );
			bBottomLimitOver = true;
		} else if (m_dragStartLine +offsetLine +m_viewLineMax+1 == nAllLine && 
				   m_offsetPixelY <= -(lineHeightPixel -screenHeight % lineHeightPixel))
		{
			// 最終行かつ下方向ピクセルオフセットあり
			TRACE_ScrollByMoveY( L"最終行かつ下方向ピクセルオフセットあり(2)\n" );
			bBottomLimitOver = true;
		}
	}
	if (bBottomLimitOver) {
		TRACE_ScrollByMoveY( L"... 下にはみでたため、再下端まで戻す\n" );
		// 下にはみ出た場合最下端まで引き戻す
		offsetLine     = nAllLine - m_viewLineMax - m_dragStartLine;
		m_offsetPixelY = screenHeight % lineHeightPixel;
		bLimitOver = true;
	}

	// 描画
	TRACE_ScrollByMoveY( L"DrawDetail, from %d [line], offset %d [pixel]\n", m_dragStartLine + offsetLine, m_offsetPixelY );
	DrawDetail(m_dragStartLine + offsetLine, true);

	// スクロール位置が変化したかもしれないのでオーナーに通知
	CPoint lastPoint = m_autoScrollInfo.getLastPoint();
	::SendMessage( GetParent()->GetSafeHwnd(), WM_MOUSEMOVE, (WPARAM)0, (LPARAM)MAKELPARAM(lastPoint.x, lastPoint.y+dy) );

	return bLimitOver;
}


/**
 * タイマーイベント
 */
void Ran2View::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == TIMERID_AUTOSCROLL ){
		// 慣性スクロール
#ifdef DEBUG
		wprintf( L"OnTimer, TIMERID_AUTOSCROLL\n" );
#endif

		// 仮想的な移動量算出
		int dt = GetTickCount() - m_dwAutoScrollStartTick;
		// 擬似的なマイナスの加速度とする。
#ifdef WINCE
		double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.20;	// マイナスの加速度, スケーリングは適当ｗ
#else
		double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.06;	// マイナスの加速度, スケーリングは適当ｗ
#endif
		double speed = m_autoScrollInfo.calcMouseMoveSpeedY();

		int dyByAccel    = (int)((accel*dt*dt)/2.0);		// マイナスの加速度
		int dyByVelocity = (int)(dt * speed);				// 初速による移動	
		int dyAutoScroll = dyByAccel + dyByVelocity;		// LButtonUp からの移動量

#if 0
		wprintf( L" dt : %5d, speed : %5.2f, accel : %5.6f, dy : %5d (%5d,%5d)\n", 
			dt, speed, accel, dyAutoScroll, dyByAccel, dyByVelocity );
#endif
		// 最大位置より戻った（極点を超えた）、
		// 加速度がしきい値より小さい、
		// またはN秒経過したなら終了
		if (speed == 0.0 ||
			(speed < 0 && dyAutoScroll > m_yAutoScrollMax) ||
			(speed > 0 && dyAutoScroll < m_yAutoScrollMax) ||
			(fabs(accel)<0.00005) ||
			dt > 5 * 1000)
		{
			KillTimer(nIDEvent);
			m_bAutoScrolling = false;
		} else {
			// dyAutoScroll 分だけ移動する。
			CPoint lastPoint = m_autoScrollInfo.getLastPoint();
			int dy = m_ptDragStart.y - lastPoint.y - dyAutoScroll;
			if (ScrollByMoveY( dy )) {
				// 範囲超過のため終了
				KillTimer(nIDEvent);
				m_bAutoScrolling = false;
			}
		}

		m_yAutoScrollMax = dyAutoScroll;
#ifdef DEBUG
		//wprintf( L"m_yAutoScrollMax:%5d\n" , m_yAutoScrollMax);
#endif
	} else if( nIDEvent == TIMERID_PANSCROLL ) {
		// パンスクロール
#ifdef DEBUG
		wprintf( L"OnTimer, TIMERID_PANSCROLL\n" );
#endif
		int dwDt = GetTickCount() - m_dwPanScrollLastTick;
		m_dwPanScrollLastTick = GetTickCount();

		if( m_dPxelX == 0 ) {
			// 移動量ゼロなら無限ループ防止のため中止
			m_offsetPixelX = 0;
			m_bAutoScrolling = false;
			KillTimer(nIDEvent);
		} else {
			// 移動処理
			m_offsetPixelX += dwDt * m_dPxelX / 10;
#ifdef DEBUG
			wprintf( L"m_offsetPixelX = %5d, dwDt = %5d\n"  , m_offsetPixelX , dwDt );
#endif
			// 終了判定
			if( m_dPxelX > 0 ){
				if( m_offsetPixelX > 0 ){
					m_offsetPixelX = 0;
				}
			} else {
				if( m_offsetPixelX < 0 ){
					m_offsetPixelX = 0;
				}
			}
			if( m_offsetPixelX == 0 ){
				// 一画面分移動した
				// パンスクロール終了
				m_bAutoScrolling = false;
				KillTimer(nIDEvent);
				m_dPxelX = 0;
			} 
			// 強制的に描画する
			CDC* pDC = GetDC();
			DrawToScreen(pDC);
			ReleaseDC(pDC);
		}

	}

	CWnd::OnTimer(nIDEvent);
}
