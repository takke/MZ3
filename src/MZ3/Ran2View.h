#if !defined(AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_)
#define AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_

//#pragma once
//#include "DIBSectionLite.h"

namespace Ran2 {

const int framePixel = 1;					// ワク線の太さ(ピクセル数)
const int lineVirtualHeightPixel = 3;		// 仮想行間の太さ
const int charInfoBlockSize = 64;	// 行情報に踏めることのできる文字修飾情報の最大要素数
const int lineTextLength = 512;		// 行単位の文字の最大の最大長
const int FrameNestLevel = 2;		// ワク線は2段階までネスト可能とする
const int PageAnchorMax = 16;		// ページ内リンクの最大数
const int pictureNameLength = 16;	// 画像ファイルのID名の最大長

// Htmlタグを分解して情報を保存する
class	HtmlRecord
{
public:
	CString	key;			// タグ名
	CString value;			// 値
	CString	parameter;		// オプション値(セロハン値、リンク先アンカーなど)
};


// SetRowPropertyが返す処理中の状態
//	BOL:(BeginOfLine)	開始タグ。新行の作成が必要。
//	EOL:(EndOfLine)		閉じタグ。行の終了として扱う。
//	FOL:(FollowOfLine)	処理の継続。行をまたぐ情報を保持してEOLまで繰り返す。
//	FBL:(ForceBreakLine)強制的な改行。行またぎ情報を保持するのでEOLとは区別して扱う。
const enum ProcessStateEnum { 
	ProcessState_BOL=0, 
	ProcessState_EOL, 
	ProcessState_FBL, 
	ProcessState_FOL, 
	ProcessState_through, 
	ProcessState_error, 
};

// 通常、太字、1/4サイズの定義
const enum FontTypeEnum {
	FontType_normal=0, 
	FontType_bold, 
	FontType_quarter, 
};

// ワク線の描画種別(nothing:描画しない、open:ワク無し、roof:開始行、follow:継続行、stool:終了行
const enum FrameTypeEnum {
	FrameType_nothing=-1, 
	FrameType_open=0, 
	FrameType_roof, 
	FrameType_follow, 
	FrameType_stool, 
};

// リンクの種別(ページ内部リンク:0、ページ外部リンク:1、画像拡大リンク:2
enum LinkType { 
	LinkType_noLink = 0, 
	LinkType_internal, 
	LinkType_external, 
	LinkType_picture, 
};

// 文字情報。背景色やフォント種別などか切り変わるまでを一つのブロックとして扱う。
class TextProperty
{
public:
	CRect			drawRect;				// 描画領域(実行時に設定される)
	FontTypeEnum	fontType;				// フォントの種別
	bool			isBold;					// 太字の有無(true:有り、false:無し)
	COLORREF		foregroundColor;		// 文字色
	COLORREF		backgroundColor;		// 背景色
	bool			isUpHanging;			// 上付き有無(true:有り、false:無し)
	bool			isDownHanging;			// 下付き有無(true:有り、false:無し)
	bool			isUnderLine;			// 下線の有無(true:有り、false:無し)
	CString			lineText;				// 行出力に使われるテキスト(400文字で足りる？)
	TextProperty();
	~TextProperty();
};


// 外字情報。一行に収まる小さな画像の出力範囲とリソース番号(ファイル名)を扱う。
class GaijiProperty
{
public:
	CRect			drawRect;
	CString			resourceID;
	GaijiProperty();
	~GaijiProperty();
};


// 下線情報。文字修飾としてリンクとは明示的に区別する場合に作ったけど現在は未使用。
class UnderLineProperty
{
public:
	CPoint		drawPoint;	// 下線描画位置
	int			width;		// 描画幅
	COLORREF	penColor;	// 描画ペン色
	UnderLineProperty();
	~UnderLineProperty();
};


// 枠線、背景色の指定。
class FrameProperty{
public:
	FrameTypeEnum	frameType;		// ワクの種別
	COLORREF	backgroundColor;	// 背景色
	COLORREF	penColor;			// 枠色
	FrameProperty();
	~FrameProperty();
};


// リンク情報。
// 下線を引いた範囲でひとつのグラップルを担当するので、行をまたぐ場合は同一のリンク情報が複数個作成される事もある。
class LinkProperty
{
public:
	LinkType	linkType;
	unsigned int linkID;		// 画面の先頭から右に向かって順番に振られるID。キー操作で次のリンクへの移動時に使用。
	unsigned int jumpUID;		// 呼び出し先
	unsigned int anchorIndex;	// 呼び出し先のページ内アンカー
	CString		paramStr;		// 汎用パラメータ
	CRect		grappleRect;	// タップに反応する矩形領域
	LinkProperty();
	~LinkProperty();
};


// 画像情報。インライン描画する際の縮小表示するための情報を保持。
class ImageProperty{
public:
	CRect	drawRect;
	int		imageNumber;	// 画像番号
	int		width;			// 画像幅
	int		height;			// 画像高さ
	int		rowNumber;		// 画像を行分割したときの位置
	ImageProperty();
	~ImageProperty();
};


// 行情報。各種Propertyを取りまとめる。
class RowProperty
{
public:
// 一行にひとつだけでおｋな情報
	unsigned int rowNumber;			// 描画上の行番号
	unsigned int anchorIndex;		// ページ内アンカーの通し番号
	int			indentLevel;		// インデントの階層(-1:インデントなし、0:レベル0指定、1:レベル2指定)
	int			breakLimitPixel;	// 折り返し位置のピクセル数
	bool		isPicture;			// 画像フラグ

// 要素数の個数だけ準備される情報
	CPtrArray*			textProperties;
	CPtrArray*			gaijiProperties;
	CPtrArray*			linkProperties;

	FrameProperty		frameProperty[FrameNestLevel];	// 要素が最大2つなので固定の配列として持つ
	ImageProperty		imageProperty;					// 画像は一行に一つなので配列にしない
	RowProperty();
	~RowProperty();
};



// ページ情報。行情報とページ内リンクを取りまとめる。
class MainInfo
{
public:
	unsigned long uid;
	CString uidName;			// 元ファイル名
	CString	recordName;			// 項目名は127文字まで
	int		anchorIndex[PageAnchorMax];	// ページ内リンクの情報(最大48個まで)
	int		propertyCount;				// lineInfoに保持される行情報の件数
	CPtrArray*	rowInfo;				// 行情報
	MainInfo();
	~MainInfo();
};



// 次行へ持ち越されるかもしれない情報
class BridgeProperty
{
public:	
	bool			isBold;					// 太字
	FontTypeEnum	fontType;				// フォントの種別
	COLORREF		foregroundColor;		// 文字色
	COLORREF		backgroundColor;		// 背景色
	bool			isUpHanging;			// 上付き有り？(true:有り、false:無し)
	bool			isDownHanging;			// 下付き有り？(true:有り、false:無し)
	bool			isUnderLine;			// 下線有り？(true:有り、false:無し)
	bool			isLink;					// リンク有り？(true:有り、false:無し)
	LinkType		linkType;				// リンクの種別
	int				jumpID;					// リンクの飛び先ID
	int				pageAnchor;				// リンクの飛び先IDのページ内アンカー
	int				indentLevel;			// インデントの位置
	int				picLine;				// 画像の分割した行位置
	int				inPageAnchor;			// 現在ページ内のアンカー
	BridgeProperty();
	~BridgeProperty();
};



// p,h1,h2,h3でも跨がなければならない情報
class BigBridgeProperty
{
public:	
	CString				remainStr;						// 出力できなくて持ち越した文字列
	int					frameNestLevel;					// 枠情報のネスト位置
	int					screenWidth;					// 画面の幅
	int					remainWidth;					// 描画に使える残り幅
	int					startWidth;						// 出力開始位置のX座標
	int					linkID;							// リンクの連番
	FrameProperty		frameProperty[FrameNestLevel];	// 要素が最大2つなので固定の配列として持つ
	bool				frameTopThrough;				// ワクの上端を描画完了が終わったらtrueになる
	BigBridgeProperty();
	~BigBridgeProperty();
};

}

using namespace Ran2;

// 絵文字対応描画コントロール「らんらん」
class Ran2View : public CWnd
{
	DECLARE_DYNAMIC(Ran2View)
	int								CurrentDPI;
	CString							uidStr;				// 現在描画中のUID名(テストでのみ使用)
	int								currentUIDNumber;	// 現在描画中のUID番号

	CPen		underLinePen;		// 汎用の黒ペン
	CBrush		blueBrush;			// 汎用の青ブラシ
	CBrush*		oldBrush;			// 復帰用のポインタ

	CFont*		normalFont;			// 通常フォント
	CFont*		boldFont;			// 太字表示で使うフォント
	CFont*		qFont;				// 上付き/下付きで使う1/4サイズのフォント
	CFont*		qBoldFont;			// 上付き/下付きで使う1/4サイズの太字フォント

	CFont*		oldFont;			// 切り替え前のフォント
	int			topOffset;			// 上端の余白(ピクセル)
	int			leftOffset;			// 左端の余白(ピクセル)
	int			frameOffset;		// ネストする枠線の余白(ピクセル)
	int			NormalWidthOffset;	// タグ指定がないときの画面幅差分

	CRect		viewRect;			// 描画領域：矩形
	int			screenWidth;		// 描画領域：横幅
	int			screenHeight;		// 描画領域：縦幅
	int			charHeightOffset;	// 行間のマージン
	int			gaijiWidthOffset;	// 外字の文字間のマージン

	int			charSpacing;		// 文字の前後スペース幅の総量(ABC幅のAとC)
	int			charHeight;			// 通常文字の高さ
	int			charWidth;			// 通常文字の幅(boldの太さに変更)
	int			boldCharWidth;		// 太文字の幅
	int			charQHeight;		// 1/4文字の高さ
	int			charQWidth;			// 1/4文字の幅
	int			currentCharWidth;	// 現在出力に使っている文字の幅(折り返し処理で使用)

	int			hangingOffset;				// 上付き、下付きの突き出し量
	CDC*		backDC;						// 画像描画用DC
	CDC*		dummyDC;					// 見積用
	CDC*		memDC;						// 裏画面DC
	CBitmap*	memBMP;						// 裏画面バッファ
	CBitmap*	oldBMP;						// 旧画面の情報
	int			viewLineMax;				// 現在のフォントで行表示可能な数
	COLORREF	normalBkColor,reverseBkColor;	// 通常時背景色と反転時背景色
	COLORREF	normalTextColor,reverseTextColor,markTextColor;	// 通常時文字色、反転時文字色、特殊マーク色
	int			drawOffsetLine;				// 現在描画を行っている行位置

	CImageList*	m_pImageList;				// 画像キャッシュへのポインタ

	// ドラッグ関連情報
	bool		m_bDragging;				// マウスドラッグ中
	CPoint		m_ptDragStart;				// ドラッグ開始位置
	int			m_dragStartLine;			// ドラッグ開始時の行番号
	int			m_offsetPixelY;				// オフセットピクセル数

	DWORD		m_dwLastLButtonUp;			// 前回左クリックされた時刻

	MainInfo*	parsedRecord;

#ifdef DEBUG
	MEMORYSTATUS	memState;			
#endif
public:
	Ran2View();
	virtual ~Ran2View();

	// 1画面で表示可能な行数
	int		GetViewLineMax() { return(viewLineMax); }
	// 全行数
	int		GetAllLineCount() {
		if (parsedRecord != NULL &&
			parsedRecord->rowInfo != NULL)
		{
			return parsedRecord->rowInfo->GetSize();
		}
		return 0;
	}

	void	PurgeMainRecord();	// mainRecordの破棄

	// クラス登録
	static BOOL RegisterWndClass(HINSTANCE hInstance);
	static BOOL UnregisterWndClass(HINSTANCE hInstance);

	int		ChangeViewFont(int newHeight, LPCTSTR szFontFace);
	CString	CalcTextByWidth(CDC* dstDC,CString srcStr,int width);
	int		GetDrawOffsetLine(){ return(drawOffsetLine); }	// 現在の描画開始位置の取得

	// 描画
	int		DrawDetail(int startLine);	// 任意の行から描画
	void	Refresh();					// 現在位置を再描画

	// データの読み込み
	int		LoadDetail(CStringArray* bodyArray, CImageList* pImageList);

	// 小分けした描画関数
	void	DrawTextProperty(int line,CPtrArray* textProperties);
	void	DrawGaijiProperty(int line,CPtrArray* gaijiProperties);
	void	DrawFrameProperty(int line,RowProperty* rowProperty);
	int		DrawImageProperty(int line,RowProperty* rowProperty);

	// ワクのtop/bottom終了すりかえ
	void	ChangeFrameProperty(BigBridgeProperty* bigBridgeInfo);

	// 大跨ぎ情報のリセット
	void	ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,BridgeProperty* bridgeInfo,ProcessStateEnum mode=ProcessState_through,int width=0);

	// 新行情報の追加
	void	AddNewRowProperty(CPtrArray* rowPropertyArray,bool forceNewRow=false);

	// ハッシュから行情報の振り分け
	ProcessStateEnum SetRowProperty(HtmlRecord* hashRecord,RowProperty* rowRecord,BridgeProperty* bridgeInfo,BigBridgeProperty* bigBridgeInfo);
	bool SetMainRecordData(HtmlRecord* hashRecord,MainInfo* mainRecord);	// MainInfoをオンメモリで作っちゃった場合の出力(テスト用)
	MainInfo* ParseDatData2(CStringArray* datArray,int width);				// MainInfoのCStringArrayからの構築

	// 解像度の判別など
	int		GetScreenDPI();
	bool	IsVGA();
	bool	IsPoratrait();

	// 指定のUIDから構築CStringArrayを構築
	CStringArray* PrepareDatArray(int uidNumber);
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
	void ResetDragOffset(void);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


#endif	//#define AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_
