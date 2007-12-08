#if !defined(AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_)
#define AFX_RAN2VIEW_H__185C28F5_417B_4C7B_8F56_3015E3F45645__INCLUDED_

//#pragma once
//#include "DIBSectionLite.h"

namespace Ran2 {

const int framePixel = 1;					// ���N���̑���(�s�N�Z����)
const int lineVirtualHeightPixel = 3;		// ���z�s�Ԃ̑���
const int charInfoBlockSize = 64;	// �s���ɓ��߂邱�Ƃ̂ł��镶���C�����̍ő�v�f��
const int lineTextLength = 512;		// �s�P�ʂ̕����̍ő�̍ő咷
const int FrameNestLevel = 2;		// ���N����2�i�K�܂Ńl�X�g�\�Ƃ���
const int PageAnchorMax = 16;		// �y�[�W�������N�̍ő吔
const int pictureNameLength = 16;	// �摜�t�@�C����ID���̍ő咷

// Html�^�O�𕪉����ď���ۑ�����
class	HtmlRecord
{
public:
	CString	key;			// �^�O��
	CString value;			// �l
	CString	parameter;		// �I�v�V�����l(�Z���n���l�A�����N��A���J�[�Ȃ�)
};


// SetRowProperty���Ԃ��������̏��
//	BOL:(BeginOfLine)	�J�n�^�O�B�V�s�̍쐬���K�v�B
//	EOL:(EndOfLine)		���^�O�B�s�̏I���Ƃ��Ĉ����B
//	FOL:(FollowOfLine)	�����̌p���B�s���܂�������ێ�����EOL�܂ŌJ��Ԃ��B
//	FBL:(ForceBreakLine)�����I�ȉ��s�B�s�܂�������ێ�����̂�EOL�Ƃ͋�ʂ��Ĉ����B
const enum ProcessStateEnum { 
	ProcessState_BOL=0, 
	ProcessState_EOL, 
	ProcessState_FBL, 
	ProcessState_FOL, 
	ProcessState_through, 
	ProcessState_error, 
};

// �ʏ�A�����A1/4�T�C�Y�̒�`
const enum FontTypeEnum {
	FontType_normal=0, 
	FontType_bold, 
	FontType_quarter, 
};

// ���N���̕`����(nothing:�`�悵�Ȃ��Aopen:���N�����Aroof:�J�n�s�Afollow:�p���s�Astool:�I���s
const enum FrameTypeEnum {
	FrameType_nothing=-1, 
	FrameType_open=0, 
	FrameType_roof, 
	FrameType_follow, 
	FrameType_stool, 
};

// �����N�̎��(�y�[�W���������N:0�A�y�[�W�O�������N:1�A�摜�g�僊���N:2
enum LinkType { 
	LinkType_noLink = 0, 
	LinkType_internal, 
	LinkType_external, 
	LinkType_picture, 
};

// �������B�w�i�F��t�H���g��ʂȂǂ��؂�ς��܂ł���̃u���b�N�Ƃ��Ĉ����B
class TextProperty
{
public:
	CRect			drawRect;				// �`��̈�(���s���ɐݒ肳���)
	FontTypeEnum	fontType;				// �t�H���g�̎��
	bool			isBold;					// �����̗L��(true:�L��Afalse:����)
	COLORREF		foregroundColor;		// �����F
	COLORREF		backgroundColor;		// �w�i�F
	bool			isUpHanging;			// ��t���L��(true:�L��Afalse:����)
	bool			isDownHanging;			// ���t���L��(true:�L��Afalse:����)
	bool			isUnderLine;			// �����̗L��(true:�L��Afalse:����)
	CString			lineText;				// �s�o�͂Ɏg����e�L�X�g(400�����ő����H)
	TextProperty();
	~TextProperty();
};


// �O�����B��s�Ɏ��܂鏬���ȉ摜�̏o�͔͈͂ƃ��\�[�X�ԍ�(�t�@�C����)�������B
class GaijiProperty
{
public:
	CRect			drawRect;
	CString			resourceID;
	GaijiProperty();
	~GaijiProperty();
};


// �������B�����C���Ƃ��ă����N�Ƃ͖����I�ɋ�ʂ���ꍇ�ɍ�������ǌ��݂͖��g�p�B
class UnderLineProperty
{
public:
	CPoint		drawPoint;	// �����`��ʒu
	int			width;		// �`�敝
	COLORREF	penColor;	// �`��y���F
	UnderLineProperty();
	~UnderLineProperty();
};


// �g���A�w�i�F�̎w��B
class FrameProperty{
public:
	FrameTypeEnum	frameType;		// ���N�̎��
	COLORREF	backgroundColor;	// �w�i�F
	COLORREF	penColor;			// �g�F
	FrameProperty();
	~FrameProperty();
};


// �����N���B
// �������������͈͂łЂƂ̃O���b�v����S������̂ŁA�s���܂����ꍇ�͓���̃����N��񂪕����쐬����鎖������B
class LinkProperty
{
public:
	LinkType	linkType;
	unsigned int linkID;		// ��ʂ̐擪����E�Ɍ������ď��ԂɐU����ID�B�L�[����Ŏ��̃����N�ւ̈ړ����Ɏg�p�B
	unsigned int jumpUID;		// �Ăяo����
	unsigned int anchorIndex;	// �Ăяo����̃y�[�W���A���J�[
	CString		paramStr;		// �ėp�p�����[�^
	CRect		grappleRect;	// �^�b�v�ɔ��������`�̈�
	LinkProperty();
	~LinkProperty();
};


// �摜���B�C�����C���`�悷��ۂ̏k���\�����邽�߂̏���ێ��B
class ImageProperty{
public:
	CRect	drawRect;
	int		imageNumber;	// �摜�ԍ�
	int		width;			// �摜��
	int		height;			// �摜����
	int		rowNumber;		// �摜���s���������Ƃ��̈ʒu
	ImageProperty();
	~ImageProperty();
};


// �s���B�e��Property�����܂Ƃ߂�B
class RowProperty
{
public:
// ��s�ɂЂƂ����ł����ȏ��
	unsigned int rowNumber;			// �`���̍s�ԍ�
	unsigned int anchorIndex;		// �y�[�W���A���J�[�̒ʂ��ԍ�
	int			indentLevel;		// �C���f���g�̊K�w(-1:�C���f���g�Ȃ��A0:���x��0�w��A1:���x��2�w��)
	int			breakLimitPixel;	// �܂�Ԃ��ʒu�̃s�N�Z����
	bool		isPicture;			// �摜�t���O

// �v�f���̌����������������
	CPtrArray*			textProperties;
	CPtrArray*			gaijiProperties;
	CPtrArray*			linkProperties;

	FrameProperty		frameProperty[FrameNestLevel];	// �v�f���ő�2�Ȃ̂ŌŒ�̔z��Ƃ��Ď���
	ImageProperty		imageProperty;					// �摜�͈�s�Ɉ�Ȃ̂Ŕz��ɂ��Ȃ�
	RowProperty();
	~RowProperty();
};



// �y�[�W���B�s���ƃy�[�W�������N�����܂Ƃ߂�B
class MainInfo
{
public:
	unsigned long uid;
	CString uidName;			// ���t�@�C����
	CString	recordName;			// ���ږ���127�����܂�
	int		anchorIndex[PageAnchorMax];	// �y�[�W�������N�̏��(�ő�48�܂�)
	int		propertyCount;				// lineInfo�ɕێ������s���̌���
	CPtrArray*	rowInfo;				// �s���
	MainInfo();
	~MainInfo();
};



// ���s�֎����z����邩������Ȃ����
class BridgeProperty
{
public:	
	bool			isBold;					// ����
	FontTypeEnum	fontType;				// �t�H���g�̎��
	COLORREF		foregroundColor;		// �����F
	COLORREF		backgroundColor;		// �w�i�F
	bool			isUpHanging;			// ��t���L��H(true:�L��Afalse:����)
	bool			isDownHanging;			// ���t���L��H(true:�L��Afalse:����)
	bool			isUnderLine;			// �����L��H(true:�L��Afalse:����)
	bool			isLink;					// �����N�L��H(true:�L��Afalse:����)
	LinkType		linkType;				// �����N�̎��
	int				jumpID;					// �����N�̔�ѐ�ID
	int				pageAnchor;				// �����N�̔�ѐ�ID�̃y�[�W���A���J�[
	int				indentLevel;			// �C���f���g�̈ʒu
	int				picLine;				// �摜�̕��������s�ʒu
	int				inPageAnchor;			// ���݃y�[�W���̃A���J�[
	BridgeProperty();
	~BridgeProperty();
};



// p,h1,h2,h3�ł��ׂ��Ȃ���΂Ȃ�Ȃ����
class BigBridgeProperty
{
public:	
	CString				remainStr;						// �o�͂ł��Ȃ��Ď����z����������
	int					frameNestLevel;					// �g���̃l�X�g�ʒu
	int					screenWidth;					// ��ʂ̕�
	int					remainWidth;					// �`��Ɏg����c�蕝
	int					startWidth;						// �o�͊J�n�ʒu��X���W
	int					linkID;							// �����N�̘A��
	FrameProperty		frameProperty[FrameNestLevel];	// �v�f���ő�2�Ȃ̂ŌŒ�̔z��Ƃ��Ď���
	bool				frameTopThrough;				// ���N�̏�[��`�抮�����I�������true�ɂȂ�
	BigBridgeProperty();
	~BigBridgeProperty();
};

}

using namespace Ran2;

// �G�����Ή��`��R���g���[���u�����v
class Ran2View : public CWnd
{
	DECLARE_DYNAMIC(Ran2View)
	int								CurrentDPI;
	CString							uidStr;				// ���ݕ`�撆��UID��(�e�X�g�ł̂ݎg�p)
	int								currentUIDNumber;	// ���ݕ`�撆��UID�ԍ�

	CPen		underLinePen;		// �ėp�̍��y��
	CBrush		blueBrush;			// �ėp�̐u���V
	CBrush*		oldBrush;			// ���A�p�̃|�C���^

	CFont*		normalFont;			// �ʏ�t�H���g
	CFont*		boldFont;			// �����\���Ŏg���t�H���g
	CFont*		qFont;				// ��t��/���t���Ŏg��1/4�T�C�Y�̃t�H���g
	CFont*		qBoldFont;			// ��t��/���t���Ŏg��1/4�T�C�Y�̑����t�H���g

	CFont*		oldFont;			// �؂�ւ��O�̃t�H���g
	int			topOffset;			// ��[�̗]��(�s�N�Z��)
	int			leftOffset;			// ���[�̗]��(�s�N�Z��)
	int			frameOffset;		// �l�X�g����g���̗]��(�s�N�Z��)
	int			NormalWidthOffset;	// �^�O�w�肪�Ȃ��Ƃ��̉�ʕ�����

	CRect		viewRect;			// �`��̈�F��`
	int			screenWidth;		// �`��̈�F����
	int			screenHeight;		// �`��̈�F�c��
	int			charHeightOffset;	// �s�Ԃ̃}�[�W��
	int			gaijiWidthOffset;	// �O���̕����Ԃ̃}�[�W��

	int			charSpacing;		// �����̑O��X�y�[�X���̑���(ABC����A��C)
	int			charHeight;			// �ʏ핶���̍���
	int			charWidth;			// �ʏ핶���̕�(bold�̑����ɕύX)
	int			boldCharWidth;		// �������̕�
	int			charQHeight;		// 1/4�����̍���
	int			charQWidth;			// 1/4�����̕�
	int			currentCharWidth;	// ���ݏo�͂Ɏg���Ă��镶���̕�(�܂�Ԃ������Ŏg�p)

	int			hangingOffset;				// ��t���A���t���̓˂��o����
	CDC*		backDC;						// �摜�`��pDC
	CDC*		dummyDC;					// ���ϗp
	CDC*		memDC;						// �����DC
	CBitmap*	memBMP;						// ����ʃo�b�t�@
	CBitmap*	oldBMP;						// ����ʂ̏��
	int			viewLineMax;				// ���݂̃t�H���g�ōs�\���\�Ȑ�
	COLORREF	normalBkColor,reverseBkColor;	// �ʏ펞�w�i�F�Ɣ��]���w�i�F
	COLORREF	normalTextColor,reverseTextColor,markTextColor;	// �ʏ펞�����F�A���]�������F�A����}�[�N�F
	int			drawOffsetLine;				// ���ݕ`����s���Ă���s�ʒu

	CImageList*	m_pImageList;				// �摜�L���b�V���ւ̃|�C���^

	// �h���b�O�֘A���
	bool		m_bDragging;				// �}�E�X�h���b�O��
	CPoint		m_ptDragStart;				// �h���b�O�J�n�ʒu
	int			m_dragStartLine;			// �h���b�O�J�n���̍s�ԍ�
	int			m_offsetPixelY;				// �I�t�Z�b�g�s�N�Z����

	DWORD		m_dwLastLButtonUp;			// �O�񍶃N���b�N���ꂽ����

	MainInfo*	parsedRecord;

#ifdef DEBUG
	MEMORYSTATUS	memState;			
#endif
public:
	Ran2View();
	virtual ~Ran2View();

	// 1��ʂŕ\���\�ȍs��
	int		GetViewLineMax() { return(viewLineMax); }
	// �S�s��
	int		GetAllLineCount() {
		if (parsedRecord != NULL &&
			parsedRecord->rowInfo != NULL)
		{
			return parsedRecord->rowInfo->GetSize();
		}
		return 0;
	}

	void	PurgeMainRecord();	// mainRecord�̔j��

	// �N���X�o�^
	static BOOL RegisterWndClass(HINSTANCE hInstance);
	static BOOL UnregisterWndClass(HINSTANCE hInstance);

	int		ChangeViewFont(int newHeight, LPCTSTR szFontFace);
	CString	CalcTextByWidth(CDC* dstDC,CString srcStr,int width);
	int		GetDrawOffsetLine(){ return(drawOffsetLine); }	// ���݂̕`��J�n�ʒu�̎擾

	// �`��
	int		DrawDetail(int startLine);	// �C�ӂ̍s����`��
	void	Refresh();					// ���݈ʒu���ĕ`��

	// �f�[�^�̓ǂݍ���
	int		LoadDetail(CStringArray* bodyArray, CImageList* pImageList);

	// �����������`��֐�
	void	DrawTextProperty(int line,CPtrArray* textProperties);
	void	DrawGaijiProperty(int line,CPtrArray* gaijiProperties);
	void	DrawFrameProperty(int line,RowProperty* rowProperty);
	int		DrawImageProperty(int line,RowProperty* rowProperty);

	// ���N��top/bottom�I�����肩��
	void	ChangeFrameProperty(BigBridgeProperty* bigBridgeInfo);

	// ��ׂ����̃��Z�b�g
	void	ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,BridgeProperty* bridgeInfo,ProcessStateEnum mode=ProcessState_through,int width=0);

	// �V�s���̒ǉ�
	void	AddNewRowProperty(CPtrArray* rowPropertyArray,bool forceNewRow=false);

	// �n�b�V������s���̐U�蕪��
	ProcessStateEnum SetRowProperty(HtmlRecord* hashRecord,RowProperty* rowRecord,BridgeProperty* bridgeInfo,BigBridgeProperty* bigBridgeInfo);
	bool SetMainRecordData(HtmlRecord* hashRecord,MainInfo* mainRecord);	// MainInfo���I���������ō����������ꍇ�̏o��(�e�X�g�p)
	MainInfo* ParseDatData2(CStringArray* datArray,int width);				// MainInfo��CStringArray����̍\�z

	// �𑜓x�̔��ʂȂ�
	int		GetScreenDPI();
	bool	IsVGA();
	bool	IsPoratrait();

	// �w���UID����\�zCStringArray���\�z
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
