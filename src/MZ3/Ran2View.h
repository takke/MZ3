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

	#include "Ran2Image.h"	// �A�j��GIF�`��N���X
#endif

#include <vector>

/// �����r���[�p�e��f�[�^��`
namespace Ran2 {

const int framePixel = 1;				///< ���N���̑���(�s�N�Z����)
const int lineVirtualHeightPixel = 3;	///< ���z�s�Ԃ̑���
const int FrameNestLevel = 2;			///< ���N����2�i�K�܂Ńl�X�g�\�Ƃ���
const int PageAnchorMax = 16;			///< �y�[�W�������N�̍ő吔

/// HTML �^�O�̎��
const enum TAG_TYPE { 
// �J�n�^�O
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
// �I���^�O
	Tag_end_p, Tag_end_blue, Tag_end_underline,
	Tag_end_sub, Tag_end_sup, Tag_end_bold, Tag_end_blockquote,
	Tag_end_link, Tag_end_img, Tag_end_mov,
	Tag_end_h1, Tag_end_h2, Tag_end_h3, 
	Tag_end_kakomi_blue, Tag_end_kakomi_gray, Tag_end_kakomi_gray2, 
	Tag_end_kakomi_white, Tag_end_kakomi_white2, 
};

/// HTML�^�O�𕪉����ď���ۑ�����
class	HtmlRecord
{
public:
	TAG_TYPE type;			///< �^�O���
	CString  value;			///< �l
//	CString	 parameter;		///< �I�v�V�����l(�Z���n���l�A�����N��A���J�[�Ȃ�)
};


/// SetRowProperty���Ԃ��������̏��
const enum ProcessStateEnum { 
	ProcessState_BeginOfLine=0,		///< �J�n�^�O�B�V�s�̍쐬���K�v�B
	ProcessState_EndOfLine,			///< ���^�O�B�s�̏I���Ƃ��Ĉ����B
	ProcessState_ForceBreakLine,	///< �����I�ȉ��s�B�s�܂�������ێ�����̂�EndOfLine�Ƃ͋�ʂ��Ĉ����B
	ProcessState_FollowOfLine,		///< �����̌p���B�s���܂�������ێ�����EndOfLine�܂ŌJ��Ԃ��B
	ProcessState_through, 
	ProcessState_error, 
};


/// �ʏ�A�����A1/4�T�C�Y�̒�`
const enum FontTypeEnum {
	FontType_normal=0,		///< �ʏ�
	FontType_bold,			///< ����
	FontType_quarter,		///< 1/4 �T�C�Y
};


/// ���N���̕`����
const enum FrameTypeEnum {
	FrameType_nothing=-1,	///< �`�悵�Ȃ�
	FrameType_open=0,		///< ���N����
	FrameType_roof,			///< �J�n�s
	FrameType_follow,		///< �p���s
	FrameType_stool,		///< �I���s
};


/// �����N�̎��
enum LinkType { 
	LinkType_noLink = 0,	///< �����N�Ȃ�
	LinkType_internal,		///< �y�[�W���������N
	LinkType_external,		///< �y�[�W�O�������N
	LinkType_picture,		///< �摜�g�僊���N
	LinkType_movie,			///< mixi���[�r�[�����N
};


/// �������B�w�i�F��t�H���g��ʂȂǂ��؂�ς��܂ł���̃u���b�N�Ƃ��Ĉ����B
class TextProperty
{
public:
	CRect			drawRect;				///< �`��̈�(���s���ɐݒ肳���)
	FontTypeEnum	fontType;				///< �t�H���g�̎��
	bool			isBold;					///< �����̗L��(true:�L��Afalse:����)
	COLORREF		foregroundColor;		///< �����F
	COLORREF		backgroundColor;		///< �w�i�F
	bool			isUpHanging;			///< ��t���L��(true:�L��Afalse:����)
	bool			isDownHanging;			///< ���t���L��(true:�L��Afalse:����)
	bool			isUnderLine;			///< �����̗L��(true:�L��Afalse:����)
	CString			lineText;				///< �s�o�͂Ɏg����e�L�X�g(400�����ő����H)
	int				linkID;					///< �����NID
	int				imglinkID;				///< �����NID
	int				movlinkID;				///< �����NID
	TextProperty();
	~TextProperty();
};


/// �O�����B��s�Ɏ��܂鏬���ȉ摜�̏o�͔͈͂ƃ��\�[�X�ԍ�(�t�@�C����)�������B
class GaijiProperty
{
public:
	CRect			drawRect;
	CString			resourceID;
	GaijiProperty();
	~GaijiProperty();
};


/// �������B�����C���Ƃ��ă����N�Ƃ͖����I�ɋ�ʂ���ꍇ�ɍ�������ǌ��݂͖��g�p�B
class UnderLineProperty
{
public:
	CPoint		drawPoint;		///< �����`��ʒu
	int			width;			///< �`�敝
	COLORREF	penColor;		///< �`��y���F
	UnderLineProperty();
	~UnderLineProperty();
};


/// �g���A�w�i�F�̎w��B
class FrameProperty
{
public:
	FrameTypeEnum	frameType;			///< ���N�̎��
	COLORREF		backgroundColor;	///< �w�i�F
	COLORREF		penColor;			///< �g�F
	FrameProperty();
	~FrameProperty();
};


/**
 * �����N���B
 *
 * �������������͈͂łЂƂ̃O���b�v����S������̂ŁA�s���܂����ꍇ�͓���̃����N��񂪕����쐬����鎖������B
 */
class LinkProperty
{
public:
	LinkType	linkType;
	unsigned int linkID;		///< ��ʂ̐擪����E�Ɍ������ď��ԂɐU����ID�B�L�[����Ŏ��̃����N�ւ̈ړ����Ɏg�p�B
	unsigned int imglinkID;		///< ��ʂ̐擪����E�Ɍ������ď��ԂɐU����ID�B�L�[����Ŏ��̃����N�ւ̈ړ����Ɏg�p�B
	unsigned int movlinkID;		///< ��ʂ̐擪����E�Ɍ������ď��ԂɐU����ID�B�L�[����Ŏ��̃����N�ւ̈ړ����Ɏg�p�B
	unsigned int jumpUID;		///< �Ăяo����
	unsigned int anchorIndex;	///< �Ăяo����̃y�[�W���A���J�[
	CString		paramStr;		///< �ėp�p�����[�^
	CRect		grappleRect;	///< �^�b�v�ɔ��������`�̈�
	LinkProperty();
	~LinkProperty();
};


/**
 * �摜���
 *
 * �C�����C���`�悷��ۂ̏k���\�����邽�߂̏���ێ��B
 */
class ImageProperty{
public:
	CRect	drawRect;
	int		imageNumber;	///< �摜�ԍ�
	int		width;			///< �摜��
	int		height;			///< �摜����
	int		rowNumber;		///< �摜���s���������Ƃ��̈ʒu
	ImageProperty();
	~ImageProperty();
};


/// �s���B�e��Property�����܂Ƃ߂�B
class RowProperty
{
public:
// ��s�ɂЂƂ����ł����ȏ��
	unsigned int rowNumber;			///< �`���̍s�ԍ�
	unsigned int anchorIndex;		///< �y�[�W���A���J�[�̒ʂ��ԍ�
	int			indentLevel;		///< �C���f���g�̊K�w(-1:�C���f���g�Ȃ��A0:���x��0�w��A1:���x��2�w��)
	int			breakLimitPixel;	///< �܂�Ԃ��ʒu�̃s�N�Z����
	bool		isPicture;			///< �摜�t���O

// �v�f���̌����������������
	CPtrArray*			textProperties;
	CPtrArray*			gaijiProperties;
	CPtrArray*			linkProperties;

	FrameProperty		frameProperty[FrameNestLevel];	///< �v�f���ő�2�Ȃ̂ŌŒ�̔z��Ƃ��Ď���
	ImageProperty		imageProperty;					///< �摜�͈�s�Ɉ�Ȃ̂Ŕz��ɂ��Ȃ�
	RowProperty();
	~RowProperty();
};


/**
 * �y�[�W���
 *
 * �s���ƃy�[�W�������N�����܂Ƃ߂�B
 */
class MainInfo
{
public:
	unsigned long uid;
	CString uidName;					///< ���t�@�C����
	CString	recordName;					///< ���ږ���127�����܂�
	int		anchorIndex[PageAnchorMax];	///< �y�[�W�������N�̏��(�ő�48�܂�)
	int		propertyCount;				///< lineInfo�ɕێ������s���̌���
	CPtrArray*	rowInfo;				///< �s���
	MainInfo();
	~MainInfo();
};


/**
 * ���s�֎����z����邩������Ȃ����
 */
class BridgeProperty
{
public:	
	bool			isBold;					///< ����
	FontTypeEnum	fontType;				///< �t�H���g�̎��
	COLORREF		foregroundColor;		///< �����F
	COLORREF		backgroundColor;		///< �w�i�F
	bool			isUpHanging;			///< ��t���L��H(true:�L��Afalse:����)
	bool			isDownHanging;			///< ���t���L��H(true:�L��Afalse:����)
	bool			isUnderLine;			///< �����L��H(true:�L��Afalse:����)
	bool			isLink;					///< �����N�L��H(true:�L��Afalse:����)
	LinkType		linkType;				///< �����N�̎��
	int				jumpID;					///< �����N�̔�ѐ�ID
	int				pageAnchor;				///< �����N�̔�ѐ�ID�̃y�[�W���A���J�[
	int				indentLevel;			///< �C���f���g�̈ʒu
	int				picLine;				///< �摜�̕��������s�ʒu
	int				inPageAnchor;			///< ���݃y�[�W���̃A���J�[
	BridgeProperty();
	~BridgeProperty();
};



/**
 * p,h1,h2,h3�ł��ׂ��Ȃ���΂Ȃ�Ȃ����
 */
class BigBridgeProperty
{
public:	
	CString			remainStr;						///< �o�͂ł��Ȃ��Ď����z����������
	int				frameNestLevel;					///< �g���̃l�X�g�ʒu
	int				screenWidth;					///< ��ʂ̕�
	int				remainWidth;					///< �`��Ɏg����c�蕝
	int				startWidth;						///< �o�͊J�n�ʒu��X���W
	int				linkID;							///< �����N�̘A��
	int				imglinkID;						///< �摜�����N�̘A��
	int				movlinkID;						///< ���惊���N�̘A��
	FrameProperty	frameProperty[FrameNestLevel];	///< �v�f���ő�2�Ȃ̂ŌŒ�̔z��Ƃ��Ď���
	bool			frameTopThrough;				///< ���N�̏�[��`�抮�����I�������true�ɂȂ�
	BigBridgeProperty();
	~BigBridgeProperty();
};

/// �����X�N���[�����
struct MouseMoveInfo {
	DWORD	tick;	///< ����
	CPoint	pt;		///< �ʒu

	MouseMoveInfo( DWORD tick_, const CPoint& pt_ )
		: tick(tick_)
		, pt(pt_)
	{
	}
};

/// �����X�N���[���Ǘ��N���X
class AutoScrollManager {
public:
	std::vector<MouseMoveInfo> m_moveInfoList;	///< �ړ����̃��O

	/// �ړ����N���A
	void clear() {
		m_moveInfoList.clear();
	}

	/// �ʒu�ǉ�
	void push( DWORD tick, const CPoint& pt ) {
		m_moveInfoList.push_back( MouseMoveInfo(tick,pt) );
		while (m_moveInfoList.size()>4) {
			m_moveInfoList.erase( m_moveInfoList.begin() );
		}
	}

	/// �c�������x�Z�o
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

	/// �c���������x�Z�o
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

	/// �ŏI�ʒu
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
 * �G�����Ή��`��R���g���[���u�����v
 */
class Ran2View : public CWnd
{
public:
	//--- �O������ύX�\�ȃI�v�V����
	//--- TODO: �A�N�Z�b�T��p�ӂ��ׂ�
	bool		m_bUsePanScrollAnimation;		///< �p���X�N���[�����̃A�j���[�V����
	bool		m_bUseHorizontalDragMove;	///< ���h���b�O�ł̍��ڈړ�
	bool		m_bUseDoubleClickMove;		///< �_�u���N���b�N�ł̍��ڈړ�

private:
	DECLARE_DYNAMIC(Ran2View)
	int			currentDPI;				///< DPI�l
	CString		uidStr;					///< ���ݕ`�撆��UID��(�e�X�g�ł̂ݎg�p)
	int			currentUIDNumber;		///< ���ݕ`�撆��UID�ԍ�

	CPen		underLinePen;			///< �ėp�̍��y��
	CPen		DarkBlueunderLinePen;	///< �A���J�p�̐y��
	CBrush		blueBrush;				///< �ėp�̐u���V
	CBrush*		oldBrush;				///< ���A�p�̃|�C���^

	CFont*		normalFont;				///< �ʏ�t�H���g
	CFont*		boldFont;				///< �����\���Ŏg���t�H���g
	CFont*		qFont;					///< ��t��/���t���Ŏg��1/4�T�C�Y�̃t�H���g
	CFont*		qBoldFont;				///< ��t��/���t���Ŏg��1/4�T�C�Y�̑����t�H���g

	CFont*		oldFont;				///< �؂�ւ��O�̃t�H���g
	int			m_drawStartTopOffset;	///< �`��J�n�I�t�Z�b�g(�s�N�Z��)
	int			topOffset;				///< ��[�̗]��(�s�N�Z��)
	int			leftOffset;				///< ���[�̗]��(�s�N�Z��)
	int			frameOffset;			///< �l�X�g����g���̗]��(�s�N�Z��)
	int			NormalWidthOffset;		///< �^�O�w�肪�Ȃ��Ƃ��̉�ʕ�����

	CRect		viewRect;				///< �`��̈�F��`
	int			screenWidth;			///< �`��̈�F����
	int			screenHeight;			///< �`��̈�F�c��
	int			charHeightOffset;		///< �s�Ԃ̃}�[�W��
	int			gaijiWidthOffset;		///< �O���̕����Ԃ̃}�[�W��

	int			charSpacing;			///< �����̑O��X�y�[�X���̑���(ABC����A��C)
	int			charHeight;				///< �ʏ핶���̍���
	int			charWidth;				///< �ʏ핶���̕�(bold�̑����ɕύX)
	int			boldCharWidth;			///< �������̕�
	int			charQHeight;			///< 1/4�����̍���
	int			charQWidth;				///< 1/4�����̕�
	int			currentCharWidth;		///< ���ݏo�͂Ɏg���Ă��镶���̕�(�܂�Ԃ������Ŏg�p)

	int			hangingOffset;				///< ��t���A���t���̓˂��o����

	CDC*		m_memDC;					///< �����DC
	CBitmap*	m_memBMP;					///< ����ʃo�b�t�@
	CBitmap*	m_oldBMP;					///< ����ʂ̏��

	CDC*		m_memPanDC;					///< �p���X�N���[���p�����DC
	CBitmap*	m_memPanBMP;				///< �p���X�N���[���p����ʃo�b�t�@
	CBitmap*	m_oldPanBMP;				///< �p���X�N���[���p����ʂ̏��

	int			m_viewLineMax;				///< ���݂̃t�H���g�ōs�\���\�Ȑ�
	int			m_drawOffsetLine;			///< ���ݕ`����s���Ă���s�ʒu

	COLORREF	normalBkColor;				///< �ʏ펞�w�i�F
	COLORREF	reverseBkColor;				///< ���]���w�i�F
	COLORREF	normalTextColor;			///< �ʏ펞�����F
	COLORREF	reverseTextColor;			///< ���]�������F
	COLORREF	markTextColor;				///< ����}�[�N�F

	/// �����NID�Ǘ��N���X
	class LinkID {
	public:
		int anchor;		///< �A���J�[�����N��ID
		int image;		///< �摜�����N��ID
		int movie;		///< ���惊���N��ID

		/// �R���X�g���N�^
		LinkID() {
			clear();
		}

		/// ������
		void clear() {
			anchor = -1;
			image = -1;
			movie = -1;
		}
	};
	LinkID		m_activeLinkID;				///< �A�N�e�B�u�ȃ����N��ID

	CImageList*	m_pImageList;				///< �摜�L���b�V���ւ̃|�C���^

	// �h���b�O�֘A���
	bool		m_bDragging;				///< �}�E�X�h���b�O��
	CPoint		m_ptDragStart;				///< �h���b�O�J�n�ʒu
	int			m_dragStartLine;			///< �h���b�O�J�n���̍s�ԍ�
	int			m_offsetPixelY;				///< �I�t�Z�b�g�s�N�Z����
	// �p���֘A���
	bool		m_bPanDragging;				///< �������}�E�X�h���b�O��
	bool		m_bScrollDragging;			///< �X�N���[����
	int			m_offsetPixelX;				///< �������I�t�Z�b�g�s�N�Z����
	int			m_dPxelX;					///< �������P�ʎ��Ԉړ��� [pixels/10msec]
	DWORD		m_dwPanScrollLastTick;		///< �p���X�N���[���J�n����

	// �_�u���N���b�N������
	DWORD		m_dwFirstLButtonUp;			///< �ŏ��ɍ��N���b�N���ꂽ����
	CPoint		m_ptFirstLButtonUp;			///< �ŏ��ɍ��N���b�N���ꂽ�ʒu

	AutoScrollManager	m_autoScrollInfo;	///< �����X�N���[�����

	DWORD		m_dwAutoScrollStartTick;	///< �����X�N���[���J�n����
	int			m_yAutoScrollMax;			///< �����X�N���[�����̍ő�ړ���

	bool		m_bAutoScrolling;			///< �����X�N���[����

	MainInfo*	parsedRecord;

#ifndef WINCE
	CPtrArray	ran2ImageArray;				///< �A�j��GIF�̃C���X�^���X�ێ��p
	Graphics*	m_graphics;					///< GDI+�̕`�惆�[�e�B���e�B
	bool		m_isAnime;					///< �A�j��GIF���ꖇ�ł��܂܂�Ă���΃^�C�}�[�X�V���s��
#endif
	bool		m_isMomi2;					///< �����X�N���[�����̃t���O

#ifdef DEBUG
	MEMORYSTATUS	memState;			
#endif

public:
	Ran2View();
	virtual ~Ran2View();

	// �N���X�o�^
	static BOOL RegisterWndClass(HINSTANCE hInstance);
	static BOOL UnregisterWndClass(HINSTANCE hInstance);

	/// 1��ʂŕ\���\�ȍs��
	int		GetViewLineMax() { return m_viewLineMax; }
	
	/// �S�s��
	int		GetAllLineCount() {
		if (parsedRecord != NULL &&
			parsedRecord->rowInfo != NULL)
		{
			return parsedRecord->rowInfo->GetSize();
		}
		return 0;
	}
	
	/// 1�s������̃I�t�Z�b�g�s�N�Z����
	int		GetCharHeightOffset() {
		return charHeightOffset;
	}

	int		ChangeViewFont(int newHeight, LPCTSTR szFontFace);
	int		MyGetScrollPos();

	// �`��
	int		DrawDetail(int startLine, bool bForceDraw=true);

	/// ���݈ʒu���ĕ`��
	void	Refresh();

	// �f�[�^�̓ǂݍ���
	int		LoadDetail(CStringArray* bodyArray, CImageList* pImageList);

	/// �p���X�N���[���̕�����`
	enum PAN_SCROLL_DIRECTION
	{
		PAN_SCROLL_DIRECTION_RIGHT,	///< �E����
		PAN_SCROLL_DIRECTION_LEFT,	///< ������
	};
	void	StartPanDraw(PAN_SCROLL_DIRECTION direction);

	void	ResetDragOffset(void);

private:
	void	MySetDragFlagWhenMovedPixelOverLimit(int dx, int dy);
	void	PurgeMainRecord();
	bool	MyMakeBackBuffers(CPaintDC& cdc);

	CString	CalcTextByWidth(CDC* dstDC,CString srcStr,int width);

	// �����������`��֐�
	void	DrawTextProperty(int line,CPtrArray* textProperties);
	void	DrawGaijiProperty(int line,CPtrArray* gaijiProperties);
	void	DrawFrameProperty(int line,RowProperty* rowProperty);
	int		DrawImageProperty(int line,RowProperty* rowProperty);

	// ���N��top/bottom�I�����肩��
	void	ChangeFrameProperty(BigBridgeProperty* bigBridgeInfo);

	// ��ׂ����̃��Z�b�g
	void	ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,
								   BridgeProperty* bridgeInfo,
								   ProcessStateEnum mode=ProcessState_through,
								   int width=0);

	// �V�s���̒ǉ�
	void	AddNewRowProperty(CPtrArray* rowPropertyArray, bool forceNewRow=false);

	// �n�b�V������s���̐U�蕪��
	ProcessStateEnum SetRowProperty(HtmlRecord* hashRecord,
									RowProperty* rowRecord,
									BridgeProperty* bridgeInfo,
									BigBridgeProperty* bigBridgeInfo);

	// MainInfo��CStringArray����̍\�z
	MainInfo* ParseDatData2(CStringArray* datArray,int width);

	void	DrawToScreen(CDC* pDC);
	bool	ScrollByMoveY(int dy);

	// �𑜓x�̔��ʂȂ�
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
