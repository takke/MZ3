/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#if !defined(AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_)
#define AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_

#ifndef WINCE
	#include "MZ3.h"
	#include <gdiplus.h>
	using namespace Gdiplus;

	#include "Ran2Image.h"	// アニメGIF描画クラス
#endif

#include <vector>

/// らんらんビュー用各種データ定義
namespace Ran2 {

const int framePixel = 1;				///< ワク線の太さ(ピクセル数)
const int lineVirtualHeightPixel = 3;	///< 仮想行間の太さ
const int FrameNestLevel = 2;			///< ワク線は2段階までネスト可能とする
const int PageAnchorMax = 16;			///< ページ内リンクの最大数

/// HTML タグの種別
const enum TAG_TYPE { 
// 開始タグ
	Tag_nothing=0, 
	Tag_h1, Tag_h2, Tag_h3,	
	Tag_p, Tag_blue, Tag_underline, 
	Tag_sub, Tag_sup, Tag_text, 
	Tag_entity, Tag_gaiji, Tag_img, Tag_mov, 
	Tag_br, Tag_anchor,	Tag_link, 
	Tag_level0, Tag_level1,	Tag_level2,	
	Tag_kakomi_blue, Tag_kakomi_gray, Tag_kakomi_gray2, 
	Tag_kakomi_white, Tag_kakomi_white2,
	Tag_bold, Tag_blockquote,
// 終了タグ
	Tag_end_p, Tag_end_blue, Tag_end_underline,
	Tag_end_sub, Tag_end_sup, Tag_end_bold, Tag_end_blockquote,
	Tag_end_link, Tag_end_img, Tag_end_mov,
	Tag_end_h1, Tag_end_h2, Tag_end_h3, 
	Tag_end_kakomi_blue, Tag_end_kakomi_gray, Tag_end_kakomi_gray2, 
	Tag_end_kakomi_white, Tag_end_kakomi_white2, 
};

/// HTMLタグを分解して情報を保存する
class	HtmlRecord
{
public:
	TAG_TYPE type;			///< タグ種別
	CString  value;			///< 値
//	CString	 parameter;		///< オプション値(セロハン値、リンク先アンカーなど)
};


/// SetRowPropertyが返す処理中の状態
const enum ProcessStateEnum { 
	ProcessState_BeginOfLine=0,		///< 開始タグ。新行の作成が必要。
	ProcessState_EndOfLine,			///< 閉じタグ。行の終了として扱う。
	ProcessState_ForceBreakLine,	///< 強制的な改行。行またぎ情報を保持するのでEndOfLineとは区別して扱う。
	ProcessState_FollowOfLine,		///< 処理の継続。行をまたぐ情報を保持してEndOfLineまで繰り返す。
	ProcessState_through, 
	ProcessState_error, 
};


/// 通常、太字、1/4サイズの定義
const enum FontTypeEnum {
	FontType_normal=0,		///< 通常
	FontType_bold,			///< 太字
	FontType_quarter,		///< 1/4 サイズ
};


/// ワク線の描画種別
const enum FrameTypeEnum {
	FrameType_nothing=-1,	///< 描画しない
	FrameType_open=0,		///< ワク無し
	FrameType_roof,			///< 開始行
	FrameType_follow,		///< 継続行
	FrameType_stool,		///< 終了行
};


/// リンクの種別
enum LinkType { 
	LinkType_noLink = 0,	///< リンクなし
	LinkType_internal,		///< ページ内部リンク
	LinkType_external,		///< ページ外部リンク
	LinkType_picture,		///< 画像拡大リンク
	LinkType_movie,			///< mixiムービーリンク
};


/// 文字情報。背景色やフォント種別などか切り変わるまでを一つのブロックとして扱う。
class TextProperty
{
public:
	CRect			drawRect;				///< 描画領域(実行時に設定される)
	FontTypeEnum	fontType;				///< フォントの種別
	bool			isBold;					///< 太字の有無(true:有り、false:無し)
	COLORREF		foregroundColor;		///< 文字色
	COLORREF		backgroundColor;		///< 背景色
	bool			isUpHanging;			///< 上付き有無(true:有り、false:無し)
	bool			isDownHanging;			///< 下付き有無(true:有り、false:無し)
	bool			isUnderLine;			///< 下線の有無(true:有り、false:無し)
	CString			lineText;				///< 行出力に使われるテキスト(400文字で足りる？)
	int				linkID;					///< リンクID
	int				imglinkID;				///< リンクID
	int				movlinkID;				///< リンクID
	TextProperty();
	~TextProperty();
};


/// 外字情報。一行に収まる小さな画像の出力範囲とリソース番号(ファイル名)を扱う。
class GaijiProperty
{
public:
	CRect			drawRect;
	CString			resourceID;
	GaijiProperty();
	~GaijiProperty();
};


/// 下線情報。文字修飾としてリンクとは明示的に区別する場合に作ったけど現在は未使用。
class UnderLineProperty
{
public:
	CPoint		drawPoint;		///< 下線描画位置
	int			width;			///< 描画幅
	COLORREF	penColor;		///< 描画ペン色
	UnderLineProperty();
	~UnderLineProperty();
};


/// 枠線、背景色の指定。
class FrameProperty
{
public:
	FrameTypeEnum	frameType;			///< ワクの種別
	COLORREF		backgroundColor;	///< 背景色
	COLORREF		penColor;			///< 枠色
	FrameProperty();
	~FrameProperty();
};


/**
 * リンク情報。
 *
 * 下線を引いた範囲でひとつのグラップルを担当するので、行をまたぐ場合は同一のリンク情報が複数個作成される事もある。
 */
class LinkProperty
{
public:
	LinkType	linkType;
	unsigned int linkID;		///< 画面の先頭から右に向かって順番に振られるID。キー操作で次のリンクへの移動時に使用。
	unsigned int imglinkID;		///< 画面の先頭から右に向かって順番に振られるID。キー操作で次のリンクへの移動時に使用。
	unsigned int movlinkID;		///< 画面の先頭から右に向かって順番に振られるID。キー操作で次のリンクへの移動時に使用。
	unsigned int jumpUID;		///< 呼び出し先
	unsigned int anchorIndex;	///< 呼び出し先のページ内アンカー
	CString		paramStr;		///< 汎用パラメータ
	CRect		grappleRect;	///< タップに反応する矩形領域
	LinkProperty();
	~LinkProperty();
};


/**
 * 画像情報
 *
 * インライン描画する際の縮小表示するための情報を保持。
 */
class ImageProperty{
public:
	CRect	drawRect;
	int		imageNumber;	///< 画像番号
	int		width;			///< 画像幅
	int		height;			///< 画像高さ
	int		rowNumber;		///< 画像を行分割したときの位置
	ImageProperty();
	~ImageProperty();
};


/// 行情報。各種Propertyを取りまとめる。
class RowProperty
{
public:
// 一行にひとつだけでおｋな情報
	unsigned int rowNumber;			///< 描画上の行番号
	unsigned int anchorIndex;		///< ページ内アンカーの通し番号
	int			indentLevel;		///< インデントの階層(-1:インデントなし、0:レベル0指定、1:レベル2指定)
	int			breakLimitPixel;	///< 折り返し位置のピクセル数
	bool		isPicture;			///< 画像フラグ

// 要素数の個数だけ準備される情報
	CPtrArray*			textProperties;
	CPtrArray*			gaijiProperties;
	CPtrArray*			linkProperties;

	FrameProperty		frameProperty[FrameNestLevel];	///< 要素が最大2つなので固定の配列として持つ
	ImageProperty		imageProperty;					///< 画像は一行に一つなので配列にしない
	RowProperty();
	~RowProperty();
};


/**
 * ページ情報
 *
 * 行情報とページ内リンクを取りまとめる。
 */
class MainInfo
{
public:
	unsigned long uid;
	CString uidName;					///< 元ファイル名
	CString	recordName;					///< 項目名は127文字まで
	int		anchorIndex[PageAnchorMax];	///< ページ内リンクの情報(最大48個まで)
	int		propertyCount;				///< lineInfoに保持される行情報の件数
	CPtrArray*	rowInfo;				///< 行情報
	MainInfo();
	~MainInfo();
};


/**
 * 次行へ持ち越されるかもしれない情報
 */
class BridgeProperty
{
public:	
	bool			isBold;					///< 太字
	FontTypeEnum	fontType;				///< フォントの種別
	COLORREF		foregroundColor;		///< 文字色
	COLORREF		backgroundColor;		///< 背景色
	bool			isUpHanging;			///< 上付き有り？(true:有り、false:無し)
	bool			isDownHanging;			///< 下付き有り？(true:有り、false:無し)
	bool			isUnderLine;			///< 下線有り？(true:有り、false:無し)
	bool			isLink;					///< リンク有り？(true:有り、false:無し)
	LinkType		linkType;				///< リンクの種別
	int				jumpID;					///< リンクの飛び先ID
	int				pageAnchor;				///< リンクの飛び先IDのページ内アンカー
	int				indentLevel;			///< インデントの位置
	int				picLine;				///< 画像の分割した行位置
	int				inPageAnchor;			///< 現在ページ内のアンカー
	BridgeProperty();
	~BridgeProperty();
};



/**
 * p,h1,h2,h3でも跨がなければならない情報
 */
class BigBridgeProperty
{
public:	
	CString			remainStr;						///< 出力できなくて持ち越した文字列
	int				frameNestLevel;					///< 枠情報のネスト位置
	int				screenWidth;					///< 画面の幅
	int				remainWidth;					///< 描画に使える残り幅
	int				startWidth;						///< 出力開始位置のX座標
	int				linkID;							///< リンクの連番
	int				imglinkID;						///< 画像リンクの連番
	int				movlinkID;						///< 動画リンクの連番
	FrameProperty	frameProperty[FrameNestLevel];	///< 要素が最大2つなので固定の配列として持つ
	bool			frameTopThrough;				///< ワクの上端を描画完了が終わったらtrueになる
	BigBridgeProperty();
	~BigBridgeProperty();
};

/// 慣性スクロール情報
struct MouseMoveInfo {
	DWORD	tick;	///< 時刻
	CPoint	pt;		///< 位置

	MouseMoveInfo( DWORD tick_, const CPoint& pt_ )
		: tick(tick_)
		, pt(pt_)
	{
	}
};

/// 慣性スクロール管理クラス
class AutoScrollManager {
public:
	std::vector<MouseMoveInfo> m_moveInfoList;	///< 移動情報のログ

	/// 移動情報クリア
	void clear() {
		m_moveInfoList.clear();
	}

	/// 位置追加
	void push( DWORD tick, const CPoint& pt ) {
		m_moveInfoList.push_back( MouseMoveInfo(tick,pt) );
		while (m_moveInfoList.size()>4) {
			m_moveInfoList.erase( m_moveInfoList.begin() );
		}
	}

	/// 縦方向速度算出
	double calcMouseMoveSpeedY() {
		double speed = 0.0;

		if (m_moveInfoList.size()>=2) {
			MouseMoveInfo& p1 = m_moveInfoList[0];
			MouseMoveInfo& p2 = m_moveInfoList[m_moveInfoList.size()-1];

			int dt = p2.tick - p1.tick;
			if (dt>0) {
				speed = (double)(p2.pt.y - p1.pt.y) / dt;
			}
		}

		return speed;
	}

	/// 縦方向加速度算出
	double calcMouseMoveAccelY() {
		double accel = 0.0;

		if (m_moveInfoList.size()>=2) {
			MouseMoveInfo& p1 = m_moveInfoList[0];
			MouseMoveInfo& p2 = m_moveInfoList[m_moveInfoList.size()-1];

			int dt = p2.tick - p1.tick;
			if (dt>0) {
				accel = (double)(p2.pt.y - p1.pt.y) / dt / dt;
			}
		}

		return accel;
	}

	/// 最終位置
	CPoint getLastPoint()
	{
		if (m_moveInfoList.empty()) {
			return CPoint(0,0);
		} else {
			return m_moveInfoList[ m_moveInfoList.size()-1 ].pt;
		}
	}
};

}// namespace Ran2


using namespace Ran2;

/**
 * 絵文字対応描画コントロール「らんらん」
 */
class Ran2View : public CWnd
{
public:
	//--- 外部から変更可能なオプション
	//--- TODO: アクセッサを用意すべき
	bool		m_bUsePanScrollAnimation;		///< パンスクロール時のアニメーション
	bool		m_bUseHorizontalDragMove;	///< 横ドラッグでの項目移動
	bool		m_bUseDoubleClickMove;		///< ダブルクリックでの項目移動

private:
	DECLARE_DYNAMIC(Ran2View)
	int			currentDPI;				///< DPI値
	CString		uidStr;					///< 現在描画中のUID名(テストでのみ使用)
	int			currentUIDNumber;		///< 現在描画中のUID番号

	CPen		underLinePen;			///< 汎用の黒ペン
	CPen		DarkBlueunderLinePen;	///< アンカ用の青ペン
	CBrush		blueBrush;				///< 汎用の青ブラシ
	CBrush*		oldBrush;				///< 復帰用のポインタ

	CFont*		normalFont;				///< 通常フォント
	CFont*		boldFont;				///< 太字表示で使うフォント
	CFont*		qFont;					///< 上付き/下付きで使う1/4サイズのフォント
	CFont*		qBoldFont;				///< 上付き/下付きで使う1/4サイズの太字フォント

	CFont*		oldFont;				///< 切り替え前のフォント
	int			m_drawStartTopOffset;	///< 描画開始オフセット(ピクセル)
	int			topOffset;				///< 上端の余白(ピクセル)
	int			leftOffset;				///< 左端の余白(ピクセル)
	int			frameOffset;			///< ネストする枠線の余白(ピクセル)
	int			NormalWidthOffset;		///< タグ指定がないときの画面幅差分

	CRect		viewRect;				///< 描画領域：矩形
	int			screenWidth;			///< 描画領域：横幅
	int			screenHeight;			///< 描画領域：縦幅
	int			charHeightOffset;		///< 行間のマージン
	int			gaijiWidthOffset;		///< 外字の文字間のマージン

	int			charSpacing;			///< 文字の前後スペース幅の総量(ABC幅のAとC)
	int			charHeight;				///< 通常文字の高さ
	int			charWidth;				///< 通常文字の幅(boldの太さに変更)
	int			boldCharWidth;			///< 太文字の幅
	int			charQHeight;			///< 1/4文字の高さ
	int			charQWidth;				///< 1/4文字の幅
	int			currentCharWidth;		///< 現在出力に使っている文字の幅(折り返し処理で使用)

	int			hangingOffset;				///< 上付き、下付きの突き出し量

	CDC*		m_memDC;					///< 裏画面DC
	CBitmap*	m_memBMP;					///< 裏画面バッファ
	CBitmap*	m_oldBMP;					///< 旧画面の情報

	CDC*		m_memPanDC;					///< パンスクロール用裏画面DC
	CBitmap*	m_memPanBMP;				///< パンスクロール用裏画面バッファ
	CBitmap*	m_oldPanBMP;				///< パンスクロール用旧画面の情報

	int			m_viewLineMax;				///< 現在のフォントで行表示可能な数
	int			m_drawOffsetLine;			///< 現在描画を行っている行位置

	COLORREF	normalBkColor;				///< 通常時背景色
	COLORREF	reverseBkColor;				///< 反転時背景色
	COLORREF	normalTextColor;			///< 通常時文字色
	COLORREF	reverseTextColor;			///< 反転時文字色
	COLORREF	markTextColor;				///< 特殊マーク色

	/// リンクID管理クラス
	class LinkID {
	public:
		int anchor;		///< アンカーリンクのID
		int image;		///< 画像リンクのID
		int movie;		///< 動画リンクのID

		/// コンストラクタ
		LinkID() {
			clear();
		}

		/// 初期化
		void clear() {
			anchor = -1;
			image = -1;
			movie = -1;
		}
	};
	LinkID		m_activeLinkID;				///< アクティブなリンクのID

	CImageList*	m_pImageList;				///< 画像キャッシュへのポインタ

	// ドラッグ関連情報
	bool		m_bDragging;				///< マウスドラッグ中
	CPoint		m_ptDragStart;				///< ドラッグ開始位置
	int			m_dragStartLine;			///< ドラッグ開始時の行番号
	int			m_offsetPixelY;				///< オフセットピクセル数
	// パン関連情報
	bool		m_bPanDragging;				///< 横方向マウスドラッグ中
	bool		m_bScrollDragging;			///< スクロール中
	int			m_offsetPixelX;				///< 横方向オフセットピクセル数
	int			m_dPxelX;					///< 横方向単位時間移動量 [pixels/10msec]
	DWORD		m_dwPanScrollLastTick;		///< パンスクロール開始時刻

	// ダブルクリック判定情報
	DWORD		m_dwFirstLButtonUp;			///< 最初に左クリックされた時刻
	CPoint		m_ptFirstLButtonUp;			///< 最初に左クリックされた位置

	AutoScrollManager	m_autoScrollInfo;	///< 慣性スクロール情報

	DWORD		m_dwAutoScrollStartTick;	///< 慣性スクロール開始時刻
	int			m_yAutoScrollMax;			///< 慣性スクロール中の最大移動量

	bool		m_bAutoScrolling;			///< 慣性スクロール中

	MainInfo*	parsedRecord;

#ifndef WINCE
	CPtrArray	ran2ImageArray;				///< アニメGIFのインスタンス保持用
	Graphics*	m_graphics;					///< GDI+の描画ユーティリティ
	bool		m_isAnime;					///< アニメGIFが一枚でも含まれていればタイマー更新を行う
#endif
	bool		m_isMomi2;					///< 慣性スクロール中のフラグ

#ifdef DEBUG
	MEMORYSTATUS	memState;			
#endif

public:
	Ran2View();
	virtual ~Ran2View();

	// クラス登録
	static BOOL RegisterWndClass(HINSTANCE hInstance);
	static BOOL UnregisterWndClass(HINSTANCE hInstance);

	/// 1画面で表示可能な行数
	int		GetViewLineMax() { return m_viewLineMax; }
	
	/// 全行数
	int		GetAllLineCount() {
		if (parsedRecord != NULL &&
			parsedRecord->rowInfo != NULL)
		{
			return parsedRecord->rowInfo->GetSize();
		}
		return 0;
	}
	
	/// 1行あたりのオフセットピクセル数
	int		GetCharHeightOffset() {
		return charHeightOffset;
	}

	int		ChangeViewFont(int newHeight, LPCTSTR szFontFace);
	int		MyGetScrollPos();

	// 描画
	int		DrawDetail(int startLine, bool bForceDraw=true);

	/// 現在位置を再描画
	void	Refresh();

	// データの読み込み
	int		LoadDetail(CStringArray* bodyArray, CImageList* pImageList);

	/// パンスクロールの方向定義
	enum PAN_SCROLL_DIRECTION
	{
		PAN_SCROLL_DIRECTION_RIGHT,	///< 右方向
		PAN_SCROLL_DIRECTION_LEFT,	///< 左方向
	};
	void	StartPanDraw(PAN_SCROLL_DIRECTION direction);

	void	ResetDragOffset(void);

private:
	void	MySetDragFlagWhenMovedPixelOverLimit(int dx, int dy);
	void	PurgeMainRecord();
	bool	MyMakeBackBuffers(CPaintDC& cdc);

	CString	CalcTextByWidth(CDC* dstDC,CString srcStr,int width);

	// 小分けした描画関数
	void	DrawTextProperty(int line,CPtrArray* textProperties);
	void	DrawGaijiProperty(int line,CPtrArray* gaijiProperties);
	void	DrawFrameProperty(int line,RowProperty* rowProperty);
	int		DrawImageProperty(int line,RowProperty* rowProperty);

	// ワクのtop/bottom終了すりかえ
	void	ChangeFrameProperty(BigBridgeProperty* bigBridgeInfo);

	// 大跨ぎ情報のリセット
	void	ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,
								   BridgeProperty* bridgeInfo,
								   ProcessStateEnum mode=ProcessState_through,
								   int width=0);

	// 新行情報の追加
	void	AddNewRowProperty(CPtrArray* rowPropertyArray, bool forceNewRow=false);

	// ハッシュから行情報の振り分け
	ProcessStateEnum SetRowProperty(HtmlRecord* hashRecord,
									RowProperty* rowRecord,
									BridgeProperty* bridgeInfo,
									BigBridgeProperty* bigBridgeInfo);

	// MainInfoのCStringArrayからの構築
	MainInfo* ParseDatData2(CStringArray* datArray,int width);

	void	DrawToScreen(CDC* pDC);
	bool	ScrollByMoveY(int dy);

	// 解像度の判別など
	int		GetScreenDPI();
	bool	IsVGA();
//	bool	IsPoratrait();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL DestroyWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


#endif	//#define AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_
