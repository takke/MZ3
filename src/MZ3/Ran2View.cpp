// Ran2View.cpp : 実装ファイル
//
#include "stdafx.h"
#include "Ran2View.h"
#include "resourceppc.h"

IMPLEMENT_DYNAMIC(Ran2View, CWnd)

// 汎用カラー(システムの純色だけどあとで調整できる様に定義しておく)
COLORREF solidBlack = COLORREF(RGB(0x00,0x00,0x00));
COLORREF solidBlue = RGB(0x00,0x00,0xFF);
COLORREF solidWhite = RGB(0xFF,0xFF,0xFF);
COLORREF lightBlue = COLORREF(RGB(0x94,0xD2,0xF1));
COLORREF lightGray = COLORREF(RGB(0xD0,0xD0,0xD0));

#define TIMERID_AUTOSCROLL	1

// 各プロパティのコンストラクタ/デストラクタ
MainInfo::MainInfo() : rowInfo(NULL)
{
	// アンカー位置は-1で初期化
	memset(this->anchorIndex,0xFFFFFFFF,sizeof(int)*PageAnchorMax);
	this->anchorIndex[0] = 0;	// アンカーの無いデータもあるので0行目を必ず設定

	rowInfo = new CPtrArray();
}

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


ImageProperty::ImageProperty()
{
}

ImageProperty::~ImageProperty()
{
}


LinkProperty::LinkProperty()
{
}

LinkProperty::~LinkProperty()
{
}


FrameProperty::FrameProperty()
{
}

FrameProperty::~FrameProperty()
{
}


UnderLineProperty::UnderLineProperty()
{
}

UnderLineProperty::~UnderLineProperty()
{
}


GaijiProperty::GaijiProperty()
{
}

GaijiProperty::~GaijiProperty()
{
}


TextProperty::TextProperty()
{
	fontType = FontType_normal;
	isBold = false;
	foregroundColor  = solidBlack;
	backgroundColor = solidWhite;
	isUpHanging = false;
	isDownHanging = false;
	isUnderLine = false;
}


TextProperty::~TextProperty()
{
}


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


BridgeProperty::~BridgeProperty()
{
}


BigBridgeProperty::BigBridgeProperty()
{
	remainStr = TEXT("");
	frameNestLevel = -1;	// ワクがあるときは0～1の値をとる
	linkID = 0;			// リンク連番の初期化
	frameTopThrough = false;
	for(int i=0 ; i<FrameNestLevel ; i++){
		frameProperty[i].backgroundColor = solidWhite;
		frameProperty[i].penColor = solidWhite;
		frameProperty[i].frameType = FrameType_nothing;
	}
}

BigBridgeProperty::~BigBridgeProperty()
{
}


// 描画コントロール「らんらん」コンストラクタ
Ran2View::Ran2View()
	: m_bDragging(false)
	, m_dragStartLine(0)
	, m_offsetPixelY(0)
	, m_dwFirstLButtonUp(0)
	, m_pImageList(NULL)
	, m_drawStartTopOffset(0)
	, m_dwLastMouseMoveTick(0)
	, m_ptLastMouseMove(0,0)
	, m_dLastMouseMoveSpeed(0.0)
	, m_dLastMouseMoveAccel(0.0)
{
	// メンバの初期化
	// 画面解像度を取得
	CurrentDPI = this->GetScreenDPI();
	topOffset = 0;	// 画面上部からの余白

	normalFont = NULL;
	boldFont = NULL;
	qFont = NULL;
	qBoldFont = NULL;
	oldFont = NULL;
	parsedRecord = NULL;

	// 汎用ペンの作成
	underLinePen.CreatePen(PS_SOLID,1,solidBlack);

	// 汎用ブラシの作成
	blueBrush.CreateSolidBrush(solidBlue);
}


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
	bRC = ::UnregisterClass(TEXT("DETAILWND"),hInstance);

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

	screenWidth = rect.right - rect.left;
	screenHeight = rect.bottom - rect.top;

	backDC = new CDC();
	backDC->CreateCompatibleDC(&cdc);
	backDC->SetBkMode(OPAQUE);	// 透過モードに設定

	memBMP = new CBitmap();
	// 画面の高さをn倍して余裕をもたせてみた
	if( memBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight*2) != TRUE ){
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return(FALSE);
	}
	m_drawStartTopOffset = screenHeight/2;

	memDC = new CDC();
	memDC->CreateCompatibleDC(&cdc);
	memDC->SetBkMode(OPAQUE);	// 透過モードに設定
	oldBMP = memDC->SelectObject(memBMP);

	CRect r = rect;
	r.right = 100;
	r.bottom = 100;
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, r, pParentWnd, nID, pContext);
}

void Ran2View::OnSize(UINT nType, int cx, int cy)
{
	screenWidth = cx;
	screenHeight = cy;

	if (charHeight + charHeightOffset > 0) {
		viewLineMax = (screenHeight / (charHeight + charHeightOffset));	// 0オリジンなので注意！
	}

	// バックバッファのサイズが小さい場合は再生成
	BITMAP bmp;
	GetObject(memBMP->m_hObject, sizeof(BITMAP), &bmp);
	if (bmp.bmWidth < screenWidth ||
		bmp.bmHeight < screenHeight*2) 
	{
		// 解放
		if( backDC != NULL ){
			backDC->DeleteDC();
		}

		if( memDC != NULL ){
			memDC->DeleteDC();
		}

		if( memBMP != NULL ){
			memBMP->DeleteObject();
			delete memBMP;
			memBMP = new CBitmap();
		}

		// バックバッファ生成
		CPaintDC	cdc(GetParent());	// ダイアログのクライアント領域をベースとする。
		if( memBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight*2) != TRUE ){
			MessageBox(TEXT("CreateCompatibelBitmap error!"));
			return;
		}
		m_drawStartTopOffset = screenHeight/2;

		memDC->CreateCompatibleDC(&cdc);
		memDC->SetBkMode(OPAQUE);	// 透過モードに設定
		oldBMP = memDC->SelectObject(memBMP);
	}

	CWnd::OnSize(nType, cx, cy);
}

BOOL Ran2View::DestroyWindow()
{
	if( backDC != NULL ){
		backDC->DeleteDC();
		delete backDC;
	}

	if( memDC != NULL ){
		memDC->DeleteDC();
		delete memDC;
	}

	if( memBMP != NULL ){
		memBMP->DeleteObject();
		delete memBMP;
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



// 表示カラムの変更
// 引数：	newHeight	新フォントの文字高
// 戻り値：	変更後フォントの文字高
int	Ran2View::ChangeViewFont(int newHeight, LPCTSTR szFontFace)
{
	// 既にフォントが選択済みなら戻す。
	if( oldFont != NULL ){
		memDC->SelectObject(oldFont);
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
	oldFont = memDC->SelectObject(qFont);
	CSize	charQSize = memDC->GetTextExtent(CString(TEXT("W"))); 
	charQHeight = charQSize.cy;
	charQSize = memDC->GetTextExtent(CString(TEXT("●"))); 
	charQWidth = charQSize.cx;

	// Boldの幅を取得する
	oldFont = memDC->SelectObject(boldFont);

	// 「●」を規定の文字として文字幅と高さを規定
	CSize	charSize = memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = memDC->GetTextExtent(CString(TEXT("●"))); 
	boldCharWidth = charSize.cx;

	// 通常サイズのフォントを設定
	oldFont = memDC->SelectObject(normalFont);
	// 「●」を規定の文字として文字幅と高さを規定
	charSize = memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = memDC->GetTextExtent(CString(TEXT("●"))); 
	charWidth = charSize.cx;
	currentCharWidth = charWidth;

	// スペーシング幅の取得
	ABC	abcInfo;
	memDC->GetCharABCWidths(TEXT('■'),TEXT('■'),&abcInfo);
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
	gaijiWidthOffset = lineVirtualHeightPixel;
//	charRealHeightOffset = realLineHeightPixel;

	// VGA/WVGA対応機は固定値のピクセル数を2倍するのを忘れずに！
	if( IsVGA() == true ){
		charHeightOffset *= 2;
		gaijiWidthOffset *= 2;
	}

	// 特に補正とかがなければそのまま設定した値を返す。
	return(newHeight);
}


void Ran2View::OnPaint()
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト

	DrawToScreen(&dc);
}


void Ran2View::DrawToScreen(CDC* pDC)
{
	int y = -m_offsetPixelY;
	
	pDC->BitBlt( 0, 0, screenWidth, screenHeight, memDC, 0, m_drawStartTopOffset +y, SRCCOPY );
//	pDC->Rectangle(0, 0, screenWidth, screenHeight);
}


// 表示幅一杯まで何文字入るかを再計算し続ける。
// 引数	srcStr	... 対象の文字列
//		width	... 表示幅
// 戻り値 ... 表示幅内に収まる長さの文字列
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


void Ran2View::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}



LRESULT Ran2View::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WindowProc(message, wParam, lParam);
}



// 行情報の設定
// 戻り値：(EOL:[EndOfLine]閉じタグで行が終了 follow:情報を継続して次行 forceBR:情報を継続して改行）
// BOL:(BeginOfLine)開始タグで新行が必要
// EOL:(EndOfLine)閉じタグで行が終了
// follow:情報を継続して次行

// パラメータ：
// hashRecord: 振り分けるタグ情報
// rowRecord: 振り分け対象先
// bridgeInfo: 行末記号(p,h1,h2,h2,h3の終了)でリセットされる文字修飾情報
// bigBridgeInfo: end記号(end_kakomi_hoge)でリセットされる行修飾情報(いまのところ枠線のみ)
ProcessStateEnum Ran2View::SetRowProperty(HtmlRecord* hashRecord,RowProperty* rowRecord,BridgeProperty* bridgeInfo,BigBridgeProperty* bigBridgeInfo)
{
	static int imageDummyLineCount = 0; // 画像の空行出力計測用
	ProcessStateEnum processState = ProcessState_through; 
	CString logStr;
	bool isBreak = false;		// 行単位の処理が終わる度にレコードを書き出すフラグ

	// タグ情報の振り分け
	const enum tagStatus { 
	// 開始タグ
		Tag_nothing=0, 
		Tag_h1, Tag_h2, Tag_h3,	
		Tag_p, Tag_blue, Tag_underline, 
		Tag_sub, Tag_sup, Tag_text, 
		Tag_entity, Tag_gaiji, Tag_img, 
		Tag_br, Tag_anchor,	Tag_link, 
		Tag_level0, Tag_level1,	Tag_level2,	
		Tag_kakomi_blue, Tag_kakomi_gray, Tag_kakomi_gray2, 
		Tag_kakomi_white, Tag_kakomi_white2,
		Tag_bold,
	// 終了タグ
		Tag_end_p, Tag_end_blue, Tag_end_underline,
		Tag_end_sub, Tag_end_sup, Tag_end_b,
		Tag_end_link, 
		Tag_end_h1, Tag_end_h2, Tag_end_h3, 
		Tag_end_kakomi_blue, Tag_end_kakomi_gray, Tag_end_kakomi_gray2, 
		Tag_end_kakomi_white, Tag_end_kakomi_white2, 
	};
	const wchar_t* tagText[] = { 
	// 開始タグ(25個)
		TEXT("dummy"), 
		TEXT("h1"), TEXT("h2"), TEXT("h3"), 
		TEXT("p"), TEXT("blue"), TEXT("underline"), 
		TEXT("sub"), TEXT("sup"), TEXT("text"), 
		TEXT("entity"), TEXT("gaiji"), TEXT("img"), 
		TEXT("br"), TEXT("anchor"), TEXT("link"), 
		TEXT("level0"), TEXT("level1"),	TEXT("level2"), 
		TEXT("kakomi_blue"), TEXT("kakomi_gray"), TEXT("kakomi_gray2"), 
		TEXT("kakomi_white"), TEXT("kakomi_white2"),
		TEXT("b"),
	// 終了タグ(15個)
		TEXT("end_p"), TEXT("end_blue"), TEXT("end_underline"),
		TEXT("end_sub"), TEXT("end_sup"), TEXT("end_b"), 
		TEXT("end_link"), 
		TEXT("end_h1"), TEXT("end_h2"), TEXT("end_h3"),	
		TEXT("end_kakomi_blue"), TEXT("end_kakomi_gray"), TEXT("end_kakomi_gray2"), 
		TEXT("end_kakomi_white"), TEXT("end_kakomi_white2"), 
		NULL,
	};

	tagStatus currentTag = (tagStatus)0;	// 振り分けできないタグはエラー扱いとする

	// タグの一時振り分け
	for(int i=0 ;tagText[i]!=NULL; i++){
		if( hashRecord->key.Compare(tagText[i]) == 0 ){
			currentTag = (tagStatus)i;

			// 開始タグ
			if( currentTag == Tag_p ||
				currentTag == Tag_h1 ||
				currentTag == Tag_h2 ||
				currentTag == Tag_h3 ||
				currentTag == Tag_level0 ||
				currentTag == Tag_level1 ||
				currentTag == Tag_level2 )
			{
				processState = ProcessState_BOL;
			}			

			// 終了タグ
			if( currentTag == Tag_end_p ||
				currentTag == Tag_end_h1 || 
				currentTag == Tag_end_h2 ||
				currentTag == Tag_end_h3 )
			{
				processState = ProcessState_EOL;
			}

			// 改行とかがあった時
			if( currentTag == Tag_br ){
				processState = ProcessState_FBL;
			}

			// エラータグの場合はここでスキップ
			if( currentTag == Tag_nothing ){
				return(ProcessState_error);
			}
		}
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
	}

	// ワク情報の転記
	for(int i=0 ; i<FrameNestLevel ; i++){
		rowRecord->frameProperty[i].frameType = bigBridgeInfo->frameProperty[i].frameType;
		rowRecord->frameProperty[i].backgroundColor = bigBridgeInfo->frameProperty[i].backgroundColor;
		rowRecord->frameProperty[i].penColor = bigBridgeInfo->frameProperty[i].penColor;
	}

	// 囲みの終了(ワクあり)
	if( currentTag == Tag_end_kakomi_gray2 || currentTag == Tag_end_kakomi_white ||
			currentTag == Tag_end_kakomi_white2 ){

		int currentlevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[currentlevel].frameType = FrameType_stool;

		bigBridgeInfo->frameNestLevel--;

	// 囲みの終了(ワク無し)
	}else if( currentTag == Tag_end_kakomi_blue || currentTag == Tag_end_kakomi_gray ||
			currentTag == Tag_end_kakomi_gray2 || currentTag == Tag_end_kakomi_white ||
			currentTag == Tag_end_kakomi_white2 || currentTag == Tag_end_h1 || currentTag == Tag_end_h2 ){

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
	}else if( currentTag == Tag_end_b ){
		bridgeInfo->isBold = false;

	// 文字色の設定終了
	}else if( currentTag == Tag_end_blue ){
		bridgeInfo->foregroundColor = solidBlack;

	// 下線の終了
	}else if( currentTag == Tag_end_underline ){
		bridgeInfo->isUnderLine = false;

	// 文字列の設定
	}else if( currentTag == Tag_text || currentTag == Tag_entity ||  bigBridgeInfo->remainStr.GetLength() > 0){
//		processState = ProcessState_FBL;	// 基本は一行単位で改行する
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

				oldFont = memDC->SelectObject(qBoldFont);
				currentCharWidth = charQWidth;
				newText->fontType = FontType_quarter;
			}else{
				oldFont = memDC->SelectObject(boldFont);
				currentCharWidth = boldCharWidth;
			}
		}else{
			if( bridgeInfo->isUpHanging == true || bridgeInfo->isDownHanging == true ){
				// フォント修飾情報の登録
				if( bridgeInfo->isUpHanging == true )
					newText->isUpHanging = true;
				else
					newText->isDownHanging = true;

				oldFont = memDC->SelectObject(qFont);
				currentCharWidth = charQWidth;
				newText->fontType = FontType_quarter;
			}else{
				oldFont = memDC->SelectObject(normalFont);
				currentCharWidth = charWidth;
			}
		}
		CString cutStr = this->CalcTextByWidth(memDC,srcStr,remainWidth);

		// 一行に収まらない場合は持ち越し
		if( cutStr.GetLength() < srcStr.GetLength() ){
			bigBridgeInfo->remainStr = srcStr.Mid(cutStr.GetLength());

			if( bigBridgeInfo->remainStr.GetLength() > 0 ){
				// 持ち越した場合は持ち越し分が完了するまで繰り返す
				processState = ProcessState_FOL;
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
		int blockWidth = memDC->GetTextExtent(cutStr).cx;
		bigBridgeInfo->remainWidth -= blockWidth;

		// 文字幅を計算したのでフォントを戻す
		memDC->SelectObject(oldFont);

		// 対象文字列の転記
		newText->lineText = cutStr;

		// 出力領域の設定
		int sx = bigBridgeInfo->startWidth;

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
			rowRecord->textProperties->Add(newText);
			// リンクがある場合はフラグを立ててタップ反応領域を登録
			if( bridgeInfo->isLink == true ){
				newText->isUnderLine = true;

				LinkProperty* newLink = new LinkProperty();

				// リンク情報の取得
				newLink->linkType = bridgeInfo->linkType;
				newLink->jumpUID = bridgeInfo->jumpID;
				newLink->anchorIndex = bridgeInfo->pageAnchor;

				// リンク連番を登録時にインクリメント
				newLink->linkID = bigBridgeInfo->linkID++;

				// 描画する領域をリンク情報として登録
				CRect tapRect = CRect(sx,0,sx+blockWidth,0);	// Y座標は実行時に解釈されるので不要
				newLink->grappleRect = tapRect;
				rowRecord->linkProperties->Add(newLink);
			}
		}else{
			delete newText;
		}

	// 外字の設定
	}else if( currentTag == Tag_gaiji ){
		GaijiProperty* newGaiji = new GaijiProperty();

		// [m:xx] から xx を抽出し、リソースIDとする
		newGaiji->resourceID = hashRecord->value.Mid(3,hashRecord->value.GetLength()-4);	// リソース名の置換を行う場合はここでやっちゃって！
//		int blockWidth = charWidth;	// 文字幅と同一とする
		int blockWidth = 16 +gaijiWidthOffset;	// 絵文字は16ピクセル固定（CImageListを利用するため）

		// 外字が幅に収まらない場合は改行して再チャレンジ支援
		if( blockWidth > bigBridgeInfo->remainWidth ){
			// 持ち越した場合は持ち越し分が完了するまで繰り返す
			processState = ProcessState_FOL;
		}else{
			// 出力領域の設定
			int sx = bigBridgeInfo->startWidth + (charSpacing);
			CRect drawRect = CRect(sx,0,sx+blockWidth,0);	// Y座標は実行時に解釈されるので不要
			newGaiji->drawRect = drawRect;

			// 次のテキストの開始座標を更新(外字はカツカツなので文字のスペーシングを前後に入れる)
			bigBridgeInfo->startWidth += (blockWidth + charSpacing);
			bigBridgeInfo->remainWidth -= (blockWidth + charSpacing);

			// 配列に追記
			rowRecord->gaijiProperties->Add(newGaiji);
		}
	// リンクの設定
	}else if( currentTag == Tag_link ){
		// リンクの領域登録はtextの領域作成時にまとめて行う
		bridgeInfo->isLink = true;
//		bridgeInfo->jumpID = 0;

		// 同一IDなら内部リンクとして扱う
		if( bridgeInfo->jumpID == currentUIDNumber ){
			bridgeInfo->linkType = LinkType_internal;
		}else{
			bridgeInfo->linkType = LinkType_external;
		}

		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
		// リンクタグの終了
	}else if( currentTag == Tag_end_link ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
	}else{
		// それ以外のエラーの場合
		logStr.Format(TEXT("Through tag!!(%s:%s)\r\n"),hashRecord->key,hashRecord->value);
		OutputDebugString(logStr);
	}

	return(processState);
}



// 新行情報の追加
void Ran2View::AddNewRowProperty(CPtrArray* rowPropertyArray,bool forceNewRow)
{
	RowProperty* newRowRecord = new RowProperty();
	newRowRecord->rowNumber = rowPropertyArray->GetSize(); // 行番号を設定
	newRowRecord->breakLimitPixel = screenWidth - NormalWidthOffset - (leftOffset*2);

	rowPropertyArray->Add(newRowRecord);
}


// 大跨ぎ情報のリセット
void Ran2View::ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,BridgeProperty* bridgeInfo,ProcessStateEnum mode,int width)
{
	// 出力可能幅と開始位置だけリセット
	if( width != 0 ){
		bigBridgeInfo->screenWidth = width;		// 画面のクライアント領域を設定
	}
	bigBridgeInfo->remainWidth = (screenWidth - NormalWidthOffset - (leftOffset*2)) - ((leftOffset+framePixel)*3*(bigBridgeInfo->frameNestLevel+1));
	bigBridgeInfo->startWidth = leftOffset;

	// BOLだけ先頭の突き出し分オフセットするので注意！
	if( mode == ProcessState_BOL ){
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


// ワクのtop/bottom終了すりかえ
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


// DATファイルからのデータ構築
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

	if( bodyArray != NULL ){
		PurgeMainRecord();
		parsedRecord = ParseDatData2(bodyArray,this->screenWidth); // datの変換処理
		bodyArray->FreeExtra();
		bodyArray->RemoveAll();
//		delete bodyArray;
	}

	// 描画に必要な行数を返す
	if( parsedRecord != NULL ){
		rc = parsedRecord->rowInfo->GetSize() - viewLineMax; 
	}
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));


#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("★LoadDetail終了時の残メモリ:%d Bytes\r\n"),memState.dwAvailPhys);
#endif

	return(rc);
}


// 描画の入り口
int	Ran2View::DrawDetail(int startLine, bool bForceDraw)
{
	if (startLine<0) {
		return 0;
	}
	// どの行から描画したかを保存しておく
	drawOffsetLine = startLine;

	// レコードの展開ミスや範囲外の指定は弾く
	if( parsedRecord == NULL || startLine > parsedRecord->rowInfo->GetSize() ){
		return(0);
	}

	// 塗りつぶす
//	memDC->PatBlt( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, WHITENESS );
//	memDC->FillSolidRect( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, RGB(255,255,255) );
	BITMAP bmp;
	GetObject(memBMP->m_hObject, sizeof(BITMAP), &bmp);
	memDC->FillSolidRect( 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255,255,255) );

//	TRACE( L"[DrawRect]\n" );
//	TRACE( L" %dx%d\n", bmp.bmWidth, bmp.bmHeight );
//	TRACE( L" %dx%d\n", screenWidth, screenHeight );

	// オフセットスクロール用にN行余分に描画する。
	const int N_OVER_OFFSET_LINES = 2;
	for(int i=-N_OVER_OFFSET_LINES; i<=viewLineMax+N_OVER_OFFSET_LINES ; i++){
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
//			TRACE(TEXT("%dの描画を開始します\r\n"), targetLine);
			// フレームの描画。画像が設定されていたら描画しない
/*
			if( row->imageProperty.imageNumber == -1 ){
				this->DrawFrameProperty(i,row);
			}
*/
			// テキスト要素の出力(下線、リンク下線、セロハン含む)
			this->DrawTextProperty(i,row->textProperties);

			// 外字要素の出力
			this->DrawGaijiProperty(i,row->gaijiProperties);
		}

//		TRACE( L" line : %d\n", targetLine );
	}
//	memDC->Rectangle( 0, 0, 10, 10 );

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


// 行内のテキストプロパティの配列を描画する
// line: 描画したい行
// rowProperty: 行の情報
void Ran2View::DrawFrameProperty(int line,RowProperty* rowProperty)
{
	for(int i=0 ;i<FrameNestLevel ; i++){
		// ワクの描画が設定されている時のみ描画
		if(	rowProperty->frameProperty[i].frameType != FrameType_nothing ){
			CRect drawRect;
			// bottomの時以外に限って背景を描画
			if( rowProperty->frameProperty[i].frameType != FrameType_stool ){ 
				// 背景色の描画
				CBrush backBrush(rowProperty->frameProperty[i].backgroundColor);
				int sx = leftOffset + (i * frameOffset);
				int ex = (screenWidth - NormalWidthOffset) - leftOffset - (i * frameOffset);
				int sy = m_drawStartTopOffset + topOffset + (line*(charHeight+charHeightOffset));
				int ey = sy + (charHeight+charHeightOffset);
				drawRect = CRect(sx,sy,ex,ey);
				memDC->FillRect(drawRect,&backBrush);
			}

			CPen framePen(PS_SOLID,1,rowProperty->frameProperty[i].penColor);
			// 上端と左右を描画
			if( rowProperty->frameProperty[i].frameType == FrameType_roof ){

				CPen* oldPen = memDC->SelectObject(&framePen);
				memDC->MoveTo(drawRect.left,drawRect.top);
				memDC->LineTo(drawRect.right,drawRect.top);

				memDC->MoveTo(drawRect.left,drawRect.top);
				memDC->LineTo(drawRect.left,drawRect.bottom);

				memDC->MoveTo(drawRect.right,drawRect.top);
				memDC->LineTo(drawRect.right,drawRect.bottom);

				memDC->SelectObject(oldPen);
			// 末端を遡って描画
			}else if( rowProperty->frameProperty[i].frameType == FrameType_stool ){
				int sx = leftOffset + (i * frameOffset);
				int ex = (screenWidth - NormalWidthOffset) - leftOffset - (i * frameOffset);
				// 終行は次の行と連結されてしまうので-1行する
				int sy = m_drawStartTopOffset + topOffset + ((line-1)*(charHeight+charHeightOffset));
				int ey = sy + (charHeight+charHeightOffset);
				CRect drawRect = CRect(sx,sy,ex,ey);

				CPen* oldPen = memDC->SelectObject(&framePen);
				memDC->MoveTo(drawRect.left,drawRect.bottom-framePixel);
				memDC->LineTo(drawRect.right,drawRect.bottom-framePixel);
				memDC->SelectObject(oldPen);

			// 左右の枠線だけ描画
			}else if( rowProperty->frameProperty[i].frameType == FrameType_follow ){

				CPen* oldPen = memDC->SelectObject(&framePen);
				memDC->MoveTo(drawRect.left,drawRect.top);
				memDC->LineTo(drawRect.left,drawRect.bottom);

				memDC->MoveTo(drawRect.right,drawRect.top);
				memDC->LineTo(drawRect.right,drawRect.bottom);

				memDC->SelectObject(oldPen);
			}

		}
	}
	
}



// 行内のテキストプロパティの配列を描画する
// line: 描画したい行
// textProperties: textPropertyをまとめた配列
void Ran2View::DrawTextProperty(int line,CPtrArray* textProperties)
{
	CString	logStr;

	for(int j=0 ; j<textProperties->GetSize() ; j++){
		// テキストブロックの出力(後で関数化する)
		TextProperty* text = (TextProperty*)textProperties->GetAt(j);
		int sy = m_drawStartTopOffset + topOffset + framePixel + (line*(charHeight+charHeightOffset));
		// 上付き指定の場合は表示位置をずらす
		if( text->isDownHanging == true ){
			sy += (charHeight - charQHeight - framePixel);
		}
		int ey = sy + (charHeight+charHeightOffset);

		CRect drawRect = CRect(text->drawRect.left,sy,text->drawRect.right,ey);
/*
		logStr.Format(TEXT("[%s](L:%d,T:%d,R:%d,B:%d)\r\n"),
			text->lineText,drawRect.left,drawRect.top,drawRect.right,drawRect.bottom);
			OutputDebugString(logStr);
*/
		// アンダーラインの描画
		if( text->isUnderLine == true ){ 
			CPen* oldPen = NULL;
			oldPen = memDC->SelectObject(&underLinePen);
			memDC->MoveTo(drawRect.left, drawRect.bottom-charHeightOffset);
			memDC->LineTo(drawRect.right, drawRect.bottom-charHeightOffset);
			oldPen = memDC->SelectObject(oldPen);
		}

		// 文字色とフォントの切り替え
		memDC->SetTextColor(text->foregroundColor);
		memDC->SetBkColor(text->backgroundColor);
		if( text->isBold == true ){
			if( text->isUpHanging == true || text->isDownHanging == true ){
				oldFont = memDC->SelectObject(qBoldFont);
			}else{
				oldFont = memDC->SelectObject(boldFont);
			}
		}else{
			if( text->isUpHanging == true || text->isDownHanging == true ){
				oldFont = memDC->SelectObject(qFont);
			}else{
				oldFont = memDC->SelectObject(normalFont);
			}
		}

		memDC->DrawText(text->lineText,-1,drawRect,DT_LEFT | DT_NOPREFIX);
		memDC->SelectObject(oldFont);

	}
}


// 行内のテキストプロパティの配列を描画する
// line: 描画したい行
// textProperties: textPropertyをまとめた配列
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
				// 描画
				m_pImageList->Draw( memDC, imageIdx, CPoint(gaiji->drawRect.left, sy), ILD_TRANSPARENT );
			}
		}
	}
}


// datファイルから実行時クラスへの変換その2(Unicodeに変換されている事が前提)
// ファイルをCArchiveで一行づつ読むのではなく、一括で読み込んでCStringArrayへ分割してから処理を行う
MainInfo* Ran2View::ParseDatData2(CStringArray* datArray,int width)
{

	CString logStr;	// エラー出力用

	MainInfo* newMainRecord = NULL;
	newMainRecord = new MainInfo();

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
	this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BOL,width);

	// CFileArchiveでファイルの終末までループさせる
	// 一番最初の行をからっぽで追加
	// (このレコードには必ず0のアンカーが入るはずなので最後にチェックをかけて確認すること！)
	RowProperty* topRowRecord = new RowProperty;
	topRowRecord->breakLimitPixel = screenWidth - NormalWidthOffset - (leftOffset*2);

	rowPropertyArray->Add(topRowRecord);

	int readCount = 0;
	for( int i=0 ; i<datArray->GetSize() ; i++ ){
		CString lineStr = datArray->GetAt(i);
		// 前後の空白を除去
//		lineStr = lineStr.Trim();
		//logStr.Format(TEXT("[Read:%d][%s]\r\n"),readCount,lineStr.GetBuffer(0));
		//OutputDebugString(logStr);

		// 要素がない場合はスキップ
		if( lineStr.GetLength() <= 0 ){
			continue;
		}

		// ここをHtml用に修正！！
		HtmlRecord*	hashRecord = new HtmlRecord();

		// 文字、絵文字の振り分け
		if( wcsncmp(lineStr, TEXT("[m:"), 2) == 0){
			// 数値, インデックスチェックが通れば外字、それ以外はテキストとみなす
			CString code = lineStr.Mid(3,lineStr.GetLength()-4);
			for (int i=0; i<code.GetLength(); i++) {
				if (!isdigit(code[i])) {
					code = L"";
				}
			}
			if (code.IsEmpty()) {
				// 非外字
				hashRecord->key = TEXT("text");
			} else {
				// 外字
				hashRecord->key = TEXT("gaiji");
			}
		}else if( lineStr.Compare(TEXT("[br]")) == 0 ) {
			hashRecord->key = TEXT("br");
		}else if( lineStr.Compare(TEXT("[b]")) == 0 ) {
			hashRecord->key = TEXT("b");
		}else if( lineStr.Compare(TEXT("[/b]")) == 0 ) {
			hashRecord->key = TEXT("end_b");
		}else if( lineStr.Compare(TEXT("[blue]")) == 0 ) {
			hashRecord->key = TEXT("blue");
		}else if( lineStr.Compare(TEXT("[/blue]")) == 0 ) {
			hashRecord->key = TEXT("end_blue");
		}else{
			hashRecord->key = TEXT("text");
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
		ProcessStateEnum rc = this->SetRowProperty(hashRecord,currentRowRecord,&bridgeInfo,&bigBridgeInfo);

		// ページ内アンカーの行位置をキャッシュする
		if( bridgeInfo.inPageAnchor != -1 ){
			newMainRecord->anchorIndex[bridgeInfo.inPageAnchor] = rowPropertyArray->GetSize() - 1;
		}

		// 開始タグはインデント位置の再設定だけ
		if( rc == ProcessState_BOL ){

			// 大またぎ情報をリセット
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BOL);
		// 持ち越し情報の繰り返し処理の場合 
		}else if( rc == ProcessState_FOL ){
			// 持ち越しじゃなくなるまで繰り返し
			while( rc == ProcessState_FOL ){
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
				rc = this->SetRowProperty(hashRecord,currentRowRecord,&bridgeInfo,&bigBridgeInfo);
			}

			// 新行情報の追加
//			this->AddNewRowProperty(rowPropertyArray);
			// 大またぎ情報をリセット
//			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ワクのtop/bottomの状態を差し替える
			this->ChangeFrameProperty(&bigBridgeInfo);

			//TRACE(TEXT(" 繰越終了[%d]\r\n"),rowPropertyArray->GetSize());
		// 強制改行は新規レコードをNewして追記 
		}else if( rc == ProcessState_FBL ){
			// 新行情報の追加
			this->AddNewRowProperty(rowPropertyArray);
			// 大またぎ情報をリセット
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ワクのtop/bottomの状態を差し替える
			this->ChangeFrameProperty(&bigBridgeInfo);

		// 終了タグは新しいレコードをNewして追記後に行またぎ情報をリセット
		}else if( rc == ProcessState_EOL ){
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



// mainRecordの破棄
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
	}
}



// 現在位置を再描画
void Ran2View::Refresh()
{
	this->DrawDetail(drawOffsetLine);
}


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

bool Ran2View::IsVGA()
{
	if( CurrentDPI == 192 ){
		return(true);
	}

	return(false);
}


// DPI値をレジストリから取得
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


void Ran2View::OnLButtonDblClk(UINT nFlags, CPoint point)
{

	CWnd::OnLButtonDblClk(nFlags, point);
}


void Ran2View::ResetDragOffset(void)
{
	KillTimer(TIMERID_AUTOSCROLL);
	m_offsetPixelY = 0;
}


void Ran2View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// 親の呼び出し
	::SendMessage( GetParent()->GetSafeHwnd(), WM_RBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );

	CWnd::OnRButtonUp(nFlags, point);
}


// タップによるリンク位置の探索
void Ran2View::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		m_bDragging = false;
		::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
		ReleaseCapture();
	}

	// ダブルクリック判定
	if (m_dwFirstLButtonUp>0 &&
		(GetTickCount() - m_dwFirstLButtonUp) < GetDoubleClickTime())
	{
		// ダブルクリック済みなのでクリアする
		m_dwFirstLButtonUp = 0;

		// ダブルクリックとみなす
		// 親の呼び出し
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	} else {
		// ドラッグ完了
		m_dwFirstLButtonUp = GetTickCount();
		m_ptFirstLButtonUp = point;

		// 自動スクロール
		TRACE( L"! speed   : %5.2f [px/msec]\n", m_dLastMouseMoveSpeed );

		KillTimer( TIMERID_AUTOSCROLL );
		if (m_dLastMouseMoveSpeed != 0.0) {
			// 自動スクロール開始
			m_dwAutoScrollStartTick = GetTickCount();
			m_yAutoScrollMax = 0;
			SetTimer( TIMERID_AUTOSCROLL, 20L, NULL );
		}

		// 親の呼び出し
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	}

	//CWnd::OnLButtonUp(nFlags, point);
}


// タップによるリンクの処理
void Ran2View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		m_bDragging = false;
		::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
	}


	CString logStr;
	// タップ位置の行番号を取得
	int tapLine = (point.y - topOffset) / (charHeightOffset + charHeight);

	// Row配列からの取得位置を算出
	int rowNumber = drawOffsetLine + tapLine;

	bool bProcessed = false;

	// タップ位置が範囲内を越える場合は何もしない
	if( parsedRecord->rowInfo->GetSize() > rowNumber ){
		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
		for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
			LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);
/*
			logStr.Format(TEXT("リンク情報は[Type:%d][ID:%d][PA:%d][left:%d]～[right:%d]\r\n"),
				linkInfo->linkType, linkInfo->jumpUID, linkInfo->anchorIndex,
				linkInfo->grappleRect.left, linkInfo->grappleRect.right);
			OutputDebugString(logStr);
*/
			// リンク範囲の該当内部であれうばジャンプ処理を行う
			if( linkInfo->grappleRect.left <= point.x && linkInfo->grappleRect.right >= point.x ){
				if( linkInfo->linkType == LinkType_internal ){
/*
					HistoryInfo* jumpInfo = new HistoryInfo();
					jumpInfo->uid = linkInfo->jumpUID;
					jumpInfo->pageAnchor = linkInfo->anchorIndex;
					this->GetParent()->SendMessage(WM_HTML_NEXTITEM,(WPARAM)jumpInfo,0);
					//logStr.Format(TEXT("アンカー[%d]へのリンクです"),linkInfo->anchorIndex);
					//MessageBox(logStr,TEXT("ページ内アンカーの呼び出し"),MB_OK);
*/
					bProcessed = true;
					break;
				}else if( linkInfo->linkType == LinkType_external ){
/*
					HistoryInfo* jumpInfo = new HistoryInfo();
					jumpInfo->uid = linkInfo->jumpUID;
					jumpInfo->pageAnchor = linkInfo->anchorIndex;
					this->GetParent()->SendMessage(WM_HTML_NEXTITEM,(WPARAM)jumpInfo,0);
*/
					bProcessed = true;
					break;
				}else if( linkInfo->linkType == LinkType_picture ){
/*					this->GetParent()->SendMessage(WM_HTML_NEXTITEM,linkInfo->jumpUID,1);
					CString rcStr = app->GetImageNameByNumber(linkInfo->jumpUID);
					logStr.Format(TEXT("画像名[%s]へのリンクです"),rcStr);
					MessageBox(logStr,TEXT("画像の呼び出し"),MB_OK);
*/					bProcessed = true;
					break;
				}
			}
		}
	}
	if (bProcessed) {
		Default();
		return;
	}

	// ドラッグ開始
	if (GetAllLineCount()-GetViewLineMax() > 0) {
		::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );
		SetCapture();
		m_bDragging = true;
		m_ptDragStart = point;
		m_dragStartLine = drawOffsetLine;
		m_ptDragStart.y -= m_offsetPixelY;
		m_offsetPixelY = 0;

		// 自動スクロール停止
		KillTimer( TIMERID_AUTOSCROLL );
	}

	// 自動スクロール情報
	m_dwLastMouseMoveTick = GetTickCount();
	m_ptLastMouseMove = point;
	m_dLastMouseMoveSpeed = 0.0;
	m_dLastMouseMoveAccel = 0.0;

//	CWnd::OnLButtonDown(nFlags, point);
}


void Ran2View::OnMouseMove(UINT nFlags, CPoint point)
{
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

	if (m_bDragging) {
		// 自動スクロール情報収集
		{
			int dt = GetTickCount() - m_dwLastMouseMoveTick;
			int dy = point.y - m_ptLastMouseMove.y;
//			TRACE( L"---\n" );
			TRACE( L" elapsed : %5d [msec] ", dt );
			TRACE( L" dy      : %5d [px] ", dy );
			if (dt>0) {
				m_dLastMouseMoveSpeed = (double)dy / dt;
				m_dLastMouseMoveAccel = (double)dy / dt / dt;
				TRACE( L" speed   : %5.2f [px/msec]", m_dLastMouseMoveSpeed );
				TRACE( L" accel   : %5.2f [px/msec]", m_dLastMouseMoveAccel );
			} else {
				m_dLastMouseMoveSpeed = 0.0;
				m_dLastMouseMoveAccel = 0.0;
			}
			TRACE( L"\n" );
		
			m_dwLastMouseMoveTick = GetTickCount();
			m_ptLastMouseMove = point;
		}

		// ドラッグ処理
		int dy = m_ptDragStart.y - point.y;
		ScrollByMoveY( dy );
	} else {
		// スクロール可能であれば「パー」のカーソルに変更
		if (GetAllLineCount()-GetViewLineMax() > 0) {
			::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


void Ran2View::ScrollByMoveY(int dy)
{
	int d_line = dy / (charHeight + charHeightOffset);

//	TRACE( L"---\n" );
//	TRACE( L"d_line       : %5d\n", d_line );
//	TRACE( L"dy           : %5d\n", dy );
//	TRACE( L"offset       : %5d\n", m_offsetPixelY );

	if (-dy % (charHeight + charHeightOffset) <=0 || drawOffsetLine > 0) {
		// ピクセルオフセット
		// （0行目であれば上方向のスクロールは行わない）
		m_offsetPixelY = -dy % (charHeight + charHeightOffset);
	}

	// 下の方にスクロール可能か確認する
	if (parsedRecord != NULL &&
		parsedRecord->rowInfo != NULL &&
		(m_dragStartLine + d_line) <= parsedRecord->rowInfo->GetSize() - viewLineMax) 
	{
		DrawDetail(m_dragStartLine + d_line);

		// スクロール位置が変化したかもしれないのでオーナーに通知
		::SendMessage( GetParent()->GetSafeHwnd(), WM_MOUSEMOVE, (WPARAM)0, (LPARAM)MAKELPARAM(m_ptLastMouseMove.x, m_ptLastMouseMove.y+dy) );
	}
}


void Ran2View::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==TIMERID_AUTOSCROLL) {
		// 自動スクロール

		// 仮想的な移動量算出
		int dt = GetTickCount() - m_dwAutoScrollStartTick;
		// 擬似的なマイナスの加速度とする。
#ifdef WINCE
		double accel = -m_dLastMouseMoveAccel * 0.10;	// マイナスの加速度, スケーリングは適当ｗ
#else
		double accel = -m_dLastMouseMoveAccel * 0.03;	// マイナスの加速度, スケーリングは適当ｗ
#endif

		int dyByAccel    = (int)((accel*dt*dt)/2.0);			// マイナスの加速度
		int dyByVelocity = (int)(dt * m_dLastMouseMoveSpeed);	// 初速による移動	
		int dyAutoScroll = dyByAccel + dyByVelocity;			// LButtonUp からの移動量

		TRACE( L" dt : %5d, speed : %5.2f, accel : %5.2f, dy : %5d (%5d,%5d)\n", 
			dt, m_dLastMouseMoveSpeed, m_dLastMouseMoveAccel, dyAutoScroll, dyByAccel, dyByVelocity );

		// 最大位置より戻った（極点を超えた）、またはN秒経過したなら終了
		if (m_dLastMouseMoveSpeed == 0.0 ||
			(m_dLastMouseMoveSpeed < 0 && dyAutoScroll > m_yAutoScrollMax) ||
			(m_dLastMouseMoveSpeed > 0 && dyAutoScroll < m_yAutoScrollMax) ||
			dt > 5 * 1000)
		{
			KillTimer(nIDEvent);
		} else {
			// dyAutoScroll 分だけ移動する。
			int dy = m_ptDragStart.y - m_ptLastMouseMove.y - dyAutoScroll;
			ScrollByMoveY( dy );
		}

		m_yAutoScrollMax = dyAutoScroll;
	}

	CWnd::OnTimer(nIDEvent);
}
