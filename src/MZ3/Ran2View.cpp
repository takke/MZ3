/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// Ran2View.cpp : �����t�@�C��
//
#include "stdafx.h"
#include "Ran2View.h"
#include "resourceppc.h"

#ifndef WINCE
	#include <gdiplus.h>
	using namespace Gdiplus;
#endif

IMPLEMENT_DYNAMIC(Ran2View, CWnd)

// �ėp�J���[(�V�X�e���̏��F�����ǂ��ƂŒ����ł���l�ɒ�`���Ă���)
const static COLORREF solidBlack = COLORREF(RGB(0x00,0x00,0x00));
const static COLORREF solidBlue = RGB(0x00,0x00,0xFF);
const static COLORREF solidWhite = RGB(0xFF,0xFF,0xFF);
const static COLORREF solidPink = RGB(0xFF,0xC0,0xCB);
const static COLORREF solidDarkBlue = RGB(0x00,0x00,0x8B);
const static COLORREF lightBlue = COLORREF(RGB(0x94,0xD2,0xF1));
const static COLORREF lightGray = COLORREF(RGB(0xD0,0xD0,0xD0));

// �^�C�}�[ID
#define TIMERID_AUTOSCROLL	1
#define TIMERID_ANIMEGIF	2
#define TIMERID_PANSCROLL	3

// �^�C�}�[�C���^�[�o�� [msec]
#define TIMER_INTERVAL_AUTOSCROLL	20L
#define TIMER_INTERVAL_ANIMEGIF		100L
#define	TIMER_INTERVAL_PANSCROLL	10L


const int emojiFixHeight = 16;	// ���܂̂Ƃ���Œ蒷�̊G���������Ȃ��̂ł���ł����H

/// �v���p�e�B�̃R���X�g���N�^
MainInfo::MainInfo() : rowInfo(NULL)
{
	// �A���J�[�ʒu��-1�ŏ�����
	memset(this->anchorIndex,0xFFFFFFFF,sizeof(int)*PageAnchorMax);
	this->anchorIndex[0] = 0;	// �A���J�[�̖����f�[�^������̂�0�s�ڂ�K���ݒ�

	rowInfo = new CPtrArray();
}

/// �v���p�e�B�̃f�X�g���N�^
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


/// �v���p�e�B�̃R���X�g���N�^
RowProperty::RowProperty()
{
	rowNumber = 0;			// �`���̍s�ԍ�
	anchorIndex = 0;		// �y�[�W���A���J�[�̒ʂ��ԍ�
	indentLevel = -1;		// �C���f���g�̊K�w(-1:�C���f���g�Ȃ��A0:���x��0�w��A1:���x��2�w��)
	breakLimitPixel =0;		// �܂�Ԃ��ʒu�̃s�N�Z����
	isPicture = false;		// �摜�t���O

	textProperties = new CPtrArray();
	gaijiProperties = new CPtrArray();
	linkProperties = new CPtrArray();
	imageProperty.imageNumber = -1;

}

/// �v���p�e�B�̃f�X�g���N�^
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


/// �v���p�e�B�̃R���X�g���N�^
ImageProperty::ImageProperty()
{
}

/// �v���p�e�B�̃f�X�g���N�^
ImageProperty::~ImageProperty()
{
}


/// �v���p�e�B�̃R���X�g���N�^
LinkProperty::LinkProperty()
{
}

/// �v���p�e�B�̃f�X�g���N�^
LinkProperty::~LinkProperty()
{
}


/// �v���p�e�B�̃R���X�g���N�^
FrameProperty::FrameProperty()
{
}

/// �v���p�e�B�̃f�X�g���N�^
FrameProperty::~FrameProperty()
{
}


/// �v���p�e�B�̃R���X�g���N�^
UnderLineProperty::UnderLineProperty()
{
}

/// �v���p�e�B�̃f�X�g���N�^
UnderLineProperty::~UnderLineProperty()
{
}


/// �v���p�e�B�̃R���X�g���N�^
GaijiProperty::GaijiProperty()
{
}

/// �v���p�e�B�̃f�X�g���N�^
GaijiProperty::~GaijiProperty()
{
}


/// �v���p�e�B�̃R���X�g���N�^
TextProperty::TextProperty()
{
	fontType = FontType_normal;
	isBold = false;
	foregroundColor  = solidBlack;
	backgroundColor = solidWhite;
	isUpHanging = false;
	isDownHanging = false;
	isUnderLine = false;
	linkID = -1;			// �����N�A�Ԃ̏�����
	imglinkID = -1;			// �摜�����N�A�Ԃ̏�����
	movlinkID = -1;			// ���惊���N�A�Ԃ̏�����
}

/// �v���p�e�B�̃f�X�g���N�^
TextProperty::~TextProperty()
{
}


/// �v���p�e�B�̃R���X�g���N�^
BridgeProperty::BridgeProperty()
{
	fontType = FontType_normal;	// �t�H���g�̎��
	isBold = false;					// �����̗L��(true:�L��Afalse:����)
	foregroundColor = solidBlack;	// �����F
	backgroundColor = solidWhite;	// �w�i�F
	isUpHanging = false;			// ��t���L��(true:�L��Afalse:����)
	isDownHanging = false;			// ���t���L��(true:�L��Afalse:����)
	isUnderLine = false;			// �����̗L��(true:�L��Afalse:����)
	isLink = false;
	linkType = LinkType_noLink;
	jumpID = -1;					// �����N�̔�ѐ�ID
	pageAnchor = -1;				// �����N�̔�ѐ�ID�̃y�[�W���A���J�[
	inPageAnchor = -1;				// ���݃y�[�W���̃A���J�[
	indentLevel = -1;				// �C���f���g�̈ʒu
	picLine = 0;					// �摜�̕��������s�ʒu
}

/// �v���p�e�B�̃f�X�g���N�^
BridgeProperty::~BridgeProperty()
{
}


/// �v���p�e�B�̃R���X�g���N�^
BigBridgeProperty::BigBridgeProperty()
{
	remainStr = TEXT("");
	frameNestLevel = -1;	// ���N������Ƃ���0�`1�̒l���Ƃ�
	linkID = 0;			// �����N�A�Ԃ̏�����
	imglinkID = 0;		// �摜�����N�A�Ԃ̏�����
	movlinkID = 0;		// ���惊���N�A�Ԃ̏�����
	frameTopThrough = false;
	for(int i=0 ; i<FrameNestLevel ; i++){
		frameProperty[i].backgroundColor = solidWhite;
		frameProperty[i].penColor = solidWhite;
		frameProperty[i].frameType = FrameType_nothing;
	}
}

/// �v���p�e�B�̃f�X�g���N�^
BigBridgeProperty::~BigBridgeProperty()
{
}


/// �`��R���g���[���u�����v�R���X�g���N�^
Ran2View::Ran2View()
	: m_bDragging(false)
	, m_dragStartLine(0)
	, m_offsetPixelY(0)
	, m_dwFirstLButtonUp(0)
	, m_pImageList(NULL)
	, m_drawStartTopOffset(0)
	, m_viewLineMax(0)
	, topOffset(0)		// ��ʏ㕔����̗]��
	, normalFont(NULL)
	, boldFont(NULL)
	, qFont(NULL)
	, qBoldFont(NULL)
	, oldFont(NULL)
	, parsedRecord(NULL)
#ifndef WINCE
	, m_isAnime(false)
#endif
	// �p���X�N���[���p
	, m_offsetPixelX(0)				// ���X�N���[���\���I�t�Z�b�g�l
	, m_dPxelX(0)					// ���X�N���[������
	, m_bPanDragging(false)			// ���h���b�O���t���O
	, m_bScrollDragging(false)		// �c�h���b�O���t���O
	, m_dwPanScrollLastTick(0)		// �p���X�N���[���J�n����
	
	// �`��p�����o�[
#ifndef WINCE
	, m_graphics(NULL)
#endif
	, m_memDC(NULL)
	, m_memBMP(NULL)
	, m_memPanDC(NULL)
	, m_memPanBMP(NULL)

	// �p���X�N���[�����t���O
	, m_bAutoScrolling(false)
{
	// �����o�̏�����
	// ��ʉ𑜓x���擾
	currentDPI = this->GetScreenDPI();

	// �ėp�y���̍쐬
	underLinePen.CreatePen(PS_SOLID,1,solidBlack);
	DarkBlueunderLinePen.CreatePen(PS_SOLID,1,solidDarkBlue);

	// �ėp�u���V�̍쐬
	blueBrush.CreateSolidBrush(solidBlue);
}


/// �f�X�g���N�^
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


// Ran2View ���b�Z�[�W �n���h��

BOOL Ran2View::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("\r\n��Create�N�����̎c������:%d KB\r\n"),memState.dwAvailPhys/1024);
#endif

	CPaintDC	cdc(pParentWnd);	// �_�C�A���O�̃N���C�A���g�̈���x�[�X�Ƃ���B

	screenWidth  = rect.right - rect.left;
	screenHeight = rect.bottom - rect.top;

	m_memDC = new CDC();
	m_memPanDC = new CDC();

	// �o�b�N�o�b�t�@�̐���
	if (!MyMakeBackBuffers(cdc)) {
		return FALSE;
	}

	CRect r = rect;
	r.right = 100;
	r.bottom = 100;

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, r, pParentWnd, nID, pContext);
}


/**
 * �T�C�Y�ύX�C�x���g
 *
 * �����f�[�^�̍č\�z
 */
void Ran2View::OnSize(UINT nType, int cx, int cy)
{
	screenWidth = cx;
	screenHeight = cy;

	if (charHeight + charHeightOffset > 0) {
		m_viewLineMax = (screenHeight / (charHeight + charHeightOffset));	// 0�I���W���Ȃ̂Œ��ӁI
	}
#ifdef DEBUG
	wprintf( L"OnSize\n" );
	wprintf( L"m_viewLineMax : %d\n", m_viewLineMax );
#endif

	// �o�b�N�o�b�t�@�̃T�C�Y���������ꍇ�͍Đ���
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
	if (bmp.bmWidth < screenWidth ||
		bmp.bmHeight < screenHeight*2) 
	{
		CPaintDC	cdc(GetParent());	// �_�C�A���O�̃N���C�A���g�̈���x�[�X�Ƃ���B
		MyMakeBackBuffers(cdc);
	}

	CWnd::OnSize(nType, cx, cy);
}


/**
 * �o�b�N�o�b�t�@�̐���
 */
bool Ran2View::MyMakeBackBuffers(CPaintDC& cdc)
{
	//--- ���
#ifndef WINCE
	if( m_graphics != NULL ){
		delete m_graphics;
	}
#endif

	// �o�b�N�o�b�t�@�̉��
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
	}

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
	m_memBMP = new CBitmap();

	// �p���X�N���[���p�o�b�t�@�̉��
	if( m_memPanDC != NULL ){
		m_memPanDC->DeleteDC();
	}

	if( m_memPanBMP != NULL ){
		m_memPanBMP->DeleteObject();
		delete m_memPanBMP;
	}
	m_memPanBMP = new CBitmap();

	
	//--- �o�b�t�@����
	// �o�b�N�o�b�t�@�̊m��
	// ��ʂ̍�����n�{���ė]�T���������Ă݂�
	if (m_memBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight*2) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}
	m_drawStartTopOffset = screenHeight/2;

	m_memDC->CreateCompatibleDC(&cdc);
	m_memDC->SetBkMode(OPAQUE);	// ���߃��[�h�ɐݒ�
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	// �p���X�N���[���p�o�b�t�@�̊m��
	// ��ʂ̑傫���������m�ۂ���
	if (m_memPanBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}

	m_memPanDC->CreateCompatibleDC(&cdc);
	m_memPanDC->SetBkMode(OPAQUE);	// ���߃��[�h�ɐݒ�
	m_oldPanBMP = m_memPanDC->SelectObject(m_memPanBMP);

#ifndef WINCE
	m_graphics = new Graphics(m_memDC->m_hDC);
#endif

	return true;
}


/**
 * �E�B���h�E�j���C�x���g
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

	// �p���X�N���[���p�o�b�t�@�̉��
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
	TRACE(TEXT("��Create�I�����̎c������:%d KB\r\n"),memState.dwAvailPhys/1024);
#endif

	return CWnd::DestroyWindow();
}


/**
 * �\���J�����̕ύX
 *
 * @param newHeight	�V�t�H���g�̕�����
 * @return �ύX��t�H���g�̕�����
 */
int	Ran2View::ChangeViewFont(int newHeight, LPCTSTR szFontFace)
{
	// ���Ƀt�H���g���I���ς݂Ȃ�߂��B
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
	// 1/4�t�H���g�̕������ƍ������K��
	oldFont = m_memDC->SelectObject(qFont);
	CSize	charQSize = m_memDC->GetTextExtent(CString(TEXT("W"))); 
	charQHeight = charQSize.cy;
	charQSize = m_memDC->GetTextExtent(CString(TEXT("��"))); 
	charQWidth = charQSize.cx;

	// Bold�̕����擾����
	oldFont = m_memDC->SelectObject(boldFont);

	// �u���v���K��̕����Ƃ��ĕ������ƍ������K��
	CSize	charSize = m_memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = m_memDC->GetTextExtent(CString(TEXT("��"))); 
	boldCharWidth = charSize.cx;

	// �ʏ�T�C�Y�̃t�H���g��ݒ�
	oldFont = m_memDC->SelectObject(normalFont);
	// �u���v���K��̕����Ƃ��ĕ������ƍ������K��
	charSize = m_memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = m_memDC->GetTextExtent(CString(TEXT("��"))); 
	charWidth = charSize.cx;
	currentCharWidth = charWidth;

	// �X�y�[�V���O���̎擾
	ABC	abcInfo;
	m_memDC->GetCharABCWidths(TEXT('��'),TEXT('��'),&abcInfo);
	charSpacing = abcInfo.abcA + abcInfo.abcC;

	// ��ʉE���̗]���ʂ̐ݒ�
	NormalWidthOffset = (int)(charWidth * 1.3);

	// ��t���A���t���̓˂��o���ʂ�ݒ�
	hangingOffset = charHeight / 2;

	// ��[�A���[�A���N�̗]����ݒ�
	leftOffset = charWidth / 4;
	topOffset = charHeight / 4;
	frameOffset = charHeight / 2;

	// �s�Ԃ̃I�t�Z�b�g��
	charHeightOffset = lineVirtualHeightPixel;

	// �O���̃I�t�Z�b�g��
	gaijiWidthOffset = 1;
//	charRealHeightOffset = realLineHeightPixel;

	// VGA/WVGA�Ή��@�͌Œ�l�̃s�N�Z������2�{����̂�Y�ꂸ�ɁI
	if( IsVGA() == true ){
		charHeightOffset *= 2;
		gaijiWidthOffset *= 2;
	}

	// ���ɕ␳�Ƃ����Ȃ���΂��̂܂ܐݒ肵���l��Ԃ��B
	return(newHeight);
}


/**
 * �`��C�x���g
 */
void Ran2View::OnPaint()
{
	CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

	DrawToScreen(&dc);
}


/**
 * �`��
 *
 * �I�t�X�N���[�����畨���f�o�C�X�ւ̓]��
 */
void Ran2View::DrawToScreen(CDC* pDC)
{
	int sx = 0;
	int dx = 0;
	int wid = screenWidth;

	// �p���X�N���[������
	if( m_offsetPixelX > 0 ){
		// �E�ɂ���Ă���ꍇ
		sx = 0;
		dx = m_offsetPixelX;
		wid = screenWidth - m_offsetPixelX;

		// �ύX�O��ʂ������ɕ\������
		pDC->BitBlt( 0 , 0 , m_offsetPixelX , screenHeight , m_memPanDC , wid , 0 , SRCCOPY );

		// �ړ����킩��悤�ɏc��������
		pDC->MoveTo( m_offsetPixelX - 1 , 0);
		pDC->LineTo( m_offsetPixelX - 1 , screenHeight );
	} else if( m_offsetPixelX < 0 ) {
		// ���ɂ���Ă���ꍇ
		sx = -m_offsetPixelX;
		dx = 0;
		wid = screenWidth + m_offsetPixelX;

		// �ύX�O��ʂ��E���ɕ\������
		pDC->BitBlt( wid , 0 , sx , screenHeight , m_memPanDC , 0 , 0 , SRCCOPY );

		// �ړ����킩��悤�ɏc��������
		pDC->MoveTo( wid + 1 , 0);
		pDC->LineTo( wid + 1 , screenHeight );
	}

	// �I�t�Z�b�g�����ʕ�����Ε\���s�v
	if( abs( m_offsetPixelX ) > abs( screenWidth ) ){
		return;
	}

	// �ύX���ʂ��I�t�Z�b�g�ɍ��킹�ĕ\������
//	TRACE( L"m_drawStartTopOffset,m_offsetPixelY : %5d %5d\n", m_drawStartTopOffset,m_offsetPixelY );
	pDC->BitBlt( dx, 0, wid, screenHeight, m_memDC, sx, m_drawStartTopOffset -m_offsetPixelY, SRCCOPY );
//	pDC->Rectangle(0, 0, screenWidth, screenHeight);
}


/**
 * �\������t�܂ŉ��������邩���Čv�Z��������B
 *
 * @param srcStr �Ώۂ̕�����
 * @param width	 �\����
 * @return �\�������Ɏ��܂钷���̕�����
 */
CString	Ran2View::CalcTextByWidth(CDC* dstDC,CString srcStr,int width)
{
	int	length = 2;	// �擪����̕�����
	CString dstStr = srcStr;	// �߂�l�p�̕�����
	bool isExit = false;

	// 0�����̏ꍇ�̓k���[
	if( length <= 0 )
		return(dstStr);

	// �Z��������Ŕ͈͓��Ƀo�V���Ǝ��܂�Ȃ炻�̂܂ܕԂ�
	if( dstDC->GetTextExtent(dstStr).cx < width ){
		return(dstStr);
	}

	// �擪����\�������Ɏ��܂�܂Œ������v��
	dstStr = srcStr.Left(length);

	while( !isExit ){
		dstStr = srcStr.Left(length - 1); 
		if( dstDC->GetTextExtent(dstStr).cx > width ){
			// �͂ݏo�����ŏI�ʒu����1�����������`�ŕԂ�
			dstStr = srcStr.Left(length - 2); 
			isExit = true;
		}
		// �������̏���𒴂����ꍇ��������
		if( srcStr.GetLength() < length ){
			isExit = true;
		}
		length++;
	}

	return(dstStr);
}


/**
 * �s���̐ݒ�
 *
 * ParseDatData2 ����Ăяo�����
 *
 * @param hashRecord    �U�蕪����^�O���
 * @param rowRecord     �U�蕪���Ώې�
 * @param bridgeInfo    �s���L��(p,h1,h2,h2,h3�̏I��)�Ń��Z�b�g����镶���C�����
 * @param bigBridgeInfo end�L��(end_kakomi_hoge)�Ń��Z�b�g�����s�C�����(���܂̂Ƃ���g���̂�)
 *
 * @return ProcessState_EndOfLine:      ���^�O�ōs���I�� <br>
 *         ProcessState_BeginOfLine:    �J�n�^�O�ŐV�s���K�v <br>
 *         follow:                      �����p�����Ď��s <br>
 *         ProcessState_ForceBreakLine: �����p�����ĉ��s <br>
 */
ProcessStateEnum Ran2View::SetRowProperty(HtmlRecord* hashRecord, 
										  RowProperty* rowRecord,
										  BridgeProperty* bridgeInfo,
										  BigBridgeProperty* bigBridgeInfo)
{
	static int imageDummyLineCount = 0; // �摜�̋�s�o�͌v���p
	ProcessStateEnum processState = ProcessState_through; 
	bool isBreak = false;		// �s�P�ʂ̏������I���x�Ƀ��R�[�h�������o���t���O

	// �^�O���̐U�蕪��
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
		// �J�n�^�O
		processState = ProcessState_BeginOfLine;
		break;

	case Tag_end_p:
	case Tag_end_h1:
	case Tag_end_h2:
	case Tag_end_h3:
		// �I���^�O
		processState = ProcessState_EndOfLine;
		break;

	case Tag_br:
		// ���s�Ƃ�����������
		processState = ProcessState_ForceBreakLine;
		break;

	case Tag_nothing:
		// �G���[�^�O�̏ꍇ�͂����ŃX�L�b�v
		return(ProcessState_error);
	}

	// �s���ׂ��ł��ς��Ȃ���񂩂珈��
	// �݂͂̐ݒ�(���N�������A�w�i������)
	if( currentTag == Tag_kakomi_blue || currentTag == Tag_h1 || currentTag == Tag_h2 ){
		bigBridgeInfo->frameNestLevel++;
		int nestLevel = bigBridgeInfo->frameNestLevel;

		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_open;	// ���N�̎��
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = lightBlue;		// �w�i�F
		bigBridgeInfo->frameProperty[nestLevel].penColor = lightBlue;				// �g�F
	// �݂͂̐ݒ�(���N�������A�w�i�����D�F)
	}else if( currentTag == Tag_kakomi_gray ){
		bigBridgeInfo->frameNestLevel++;
		int nestLevel = bigBridgeInfo->frameNestLevel;

		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_open;	// ���N�̎��
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = lightGray;		// �w�i�F
		bigBridgeInfo->frameProperty[nestLevel].penColor = lightGray;				// �g�F

	// �݂͂̐ݒ�(���N�����A�w�i�����D�F)
	}else if( currentTag == Tag_kakomi_gray2 ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_roof;		// ���N�̎��
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = lightGray;		// �w�i�F
		bigBridgeInfo->frameProperty[nestLevel].penColor = lightBlue;				// �g�F
		bigBridgeInfo->frameTopThrough = false;

	// �݂͂̐ݒ�(���N�������A�w�i�����F)
	}else if( currentTag == Tag_kakomi_white ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_roof;		// ���N�̎��
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = solidWhite;		// �w�i�F
		bigBridgeInfo->frameProperty[nestLevel].penColor = solidBlack;				// �g�F
		bigBridgeInfo->frameTopThrough = false;

	// �݂͂̐ݒ�(���N�����A�w�i�����F)
	}else if( currentTag == Tag_kakomi_white2 ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_roof;		// ���N�̎��
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = solidWhite;		// �w�i�F
		bigBridgeInfo->frameProperty[nestLevel].penColor = solidBlue;				// �g�F
		bigBridgeInfo->frameTopThrough = false;

	// �݂͂̐ݒ�(<blockquote>:���N���Ȃ��A�w�i���s���N)
	}else if( currentTag == Tag_blockquote ){
		bigBridgeInfo->frameNestLevel++;

		int nestLevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[nestLevel].frameType = FrameType_open;		// ���N�̎��
		bigBridgeInfo->frameProperty[nestLevel].backgroundColor = solidPink;		// �w�i�F
		bigBridgeInfo->frameProperty[nestLevel].penColor = solidPink;				// �g�F
		bigBridgeInfo->frameTopThrough = false;
	}

	// ���N���̓]�L
	for(int i=0 ; i<FrameNestLevel ; i++){
		rowRecord->frameProperty[i].frameType = bigBridgeInfo->frameProperty[i].frameType;
		rowRecord->frameProperty[i].backgroundColor = bigBridgeInfo->frameProperty[i].backgroundColor;
		rowRecord->frameProperty[i].penColor = bigBridgeInfo->frameProperty[i].penColor;
	}

	// �݂͂̏I��(���N����)
	if( currentTag == Tag_end_kakomi_gray2 || currentTag == Tag_end_kakomi_white ||
			currentTag == Tag_end_kakomi_white2 || currentTag == Tag_end_blockquote ){

		int currentlevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[currentlevel].frameType = FrameType_stool;

		bigBridgeInfo->frameNestLevel--;

	// �݂͂̏I��(���N����)
	}else if( currentTag == Tag_end_kakomi_blue || currentTag == Tag_end_kakomi_gray ||
			currentTag == Tag_end_kakomi_gray2 || currentTag == Tag_end_kakomi_white ||
			currentTag == Tag_end_kakomi_white2 || currentTag == Tag_end_h1 || 
			currentTag == Tag_end_h2 ){

		// ���N���̃N���A 
		int currentlevel = bigBridgeInfo->frameNestLevel;
		bigBridgeInfo->frameProperty[currentlevel].frameType = FrameType_nothing;
		bigBridgeInfo->frameProperty[currentlevel].backgroundColor = solidWhite;
		bigBridgeInfo->frameProperty[currentlevel].penColor = solidWhite;
		bigBridgeInfo->frameNestLevel--;
	}

	// �y�[�W���A���J�[�̏��
	if( currentTag == Tag_anchor ){
		bridgeInfo->inPageAnchor = _wtol(hashRecord->value.GetBuffer(0));

	// �C���f���g���x���̐ݒ�(�s�܂�������)
	}else if( currentTag == Tag_level0 ){
		bridgeInfo->indentLevel = 0;

	}else if( currentTag == Tag_level1 ){
		bridgeInfo->indentLevel = 1;

	}else if( currentTag == Tag_level2 ){
		bridgeInfo->indentLevel = 2;

	// bool�l���������̐ݒ�
	}else if( currentTag == Tag_sub ){
		bridgeInfo->isDownHanging = true;

	}else if( currentTag == Tag_sup ){
		bridgeInfo->isUpHanging = true;

	// �����̐ݒ�
	}else if( currentTag == Tag_bold ){
		bridgeInfo->isBold = true;

	// �����F�̐ݒ�
	}else if( currentTag == Tag_blue ){
		bridgeInfo->foregroundColor = solidBlue;

	// �����̊J�n
	}else if( currentTag == Tag_underline ){
		bridgeInfo->isUnderLine = true;

	// ���t���̏I��
	}else if( currentTag == Tag_end_sub ){
		bridgeInfo->isDownHanging = false;
	// ��t���̏I��
	}else if( currentTag == Tag_end_sup ){
		bridgeInfo->isUpHanging = false;

	// �����̏I��
	}else if( currentTag == Tag_end_bold ){
		bridgeInfo->isBold = false;

	// �����F�̐ݒ�I��
	}else if( currentTag == Tag_end_blue ){
		bridgeInfo->foregroundColor = solidBlack;

	// �����̏I��
	}else if( currentTag == Tag_end_underline ){
		bridgeInfo->isUnderLine = false;

	// ������̐ݒ�
	}else if( currentTag == Tag_text || currentTag == Tag_entity ||  bigBridgeInfo->remainStr.GetLength() > 0){
//		processState = ProcessState_ForceBreakLine;	// ��{�͈�s�P�ʂŉ��s����
		processState = ProcessState_through;	// ��{�͉��s�Ȃ��Ƃ���B

		TextProperty* newText = new TextProperty();
		CString srcStr;
		// �����z��������ꍇ�͍Đݒ�
		if( bigBridgeInfo->remainStr.GetLength() > 0 ){
			srcStr = bigBridgeInfo->remainStr;
		}else{
			srcStr = hashRecord->value;
		}

		// �e�L�X�g�Ǝ��̎Q�Ƃ͍ŏI�I�ȏo�͂��ł��Ȃ������ꍇ�A���̍s�֏o�͏��𑗂�
		int remainWidth = bigBridgeInfo->remainWidth;

		// �������̌v���O�Ƀt�H���g�̎�ʂ�bridgeInfo����擾���ēK�؂ȃt�H���g�ɐ؂�ւ���
		// �����̎�
		if( bridgeInfo->isBold == true ){
			newText->isBold = true;
			if( bridgeInfo->isUpHanging == true || bridgeInfo->isDownHanging == true ){
				// �t�H���g�C�����̓o�^
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
				// �t�H���g�C�����̓o�^
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

		// ��s�Ɏ��܂�Ȃ��ꍇ�͎����z��
		if( cutStr.GetLength() < srcStr.GetLength() ){
			bigBridgeInfo->remainStr = srcStr.Mid(cutStr.GetLength());

			if( bigBridgeInfo->remainStr.GetLength() > 0 ){
				// �����z�����ꍇ�͎����z��������������܂ŌJ��Ԃ�
				processState = ProcessState_FollowOfLine;
			}
	
		}else{
			bigBridgeInfo->remainStr = TEXT("");	// �����z����������N���A
		}

		newText->foregroundColor = bridgeInfo->foregroundColor;
		newText->backgroundColor = bridgeInfo->backgroundColor;

		// ���N������̏ꍇ�̓��N���̔w�i�F��D��ɐݒ�
		if( bigBridgeInfo->frameNestLevel >= 0 ){
			newText->backgroundColor = bigBridgeInfo->frameProperty[bigBridgeInfo->frameNestLevel].backgroundColor;
		}

		// ��s�Ɏ��܂�ꍇ�͏o�͂ɏ����������̒��������E���獷������
		int blockWidth = m_memDC->GetTextExtent(cutStr).cx;
		bigBridgeInfo->remainWidth -= blockWidth;

		// ���������v�Z�����̂Ńt�H���g��߂�
		m_memDC->SelectObject(oldFont);

		// �Ώە�����̓]�L
		newText->lineText = cutStr;

		// �o�͗̈�̐ݒ�
		int sx = bigBridgeInfo->startWidth + gaijiWidthOffset;

		// ���N������ꍇ�͊J�n�ʒu�����炷
		if( bigBridgeInfo->frameNestLevel >= 0 ){
			sx += (bigBridgeInfo->frameNestLevel+1) * (leftOffset+framePixel);
		}

		CRect drawRect = CRect(sx,0,sx+blockWidth,0);	// Y���W�͎��s���ɉ��߂����̂ŕs�v
		newText->drawRect = drawRect; 

		// ����������ꍇ�̓t���O�𗧂Ă�
		if( bridgeInfo->isUnderLine == true ){
			newText->isUnderLine = true;
		}

		// ���̃e�L�X�g�̊J�n���W���X�V
		bigBridgeInfo->startWidth += blockWidth;

		// ��������̒ǋL
//		TRACE(TEXT("%d�s��%d����[%s]��ǉ����܂���\r\n"),
//			rowRecord->rowNumber,rowRecord->textProperties->GetSize(),newText->lineText);
		
		if( newText->lineText.GetLength() > 0 ){
			// �e�L�X�g���ƂɃ����NID��ݒ肷��
			newText->linkID = -1;
			newText->imglinkID = -1;
			newText->movlinkID = -1;
			// �����N������ꍇ�̓t���O�𗧂Ăă^�b�v�����̈��o�^
			if( bridgeInfo->isLink == true ){
				newText->isUnderLine = true;
				newText->foregroundColor = solidDarkBlue;

				LinkProperty* newLink = new LinkProperty();

				// �����N���̎擾
				newLink->linkType = bridgeInfo->linkType;
				newLink->jumpUID = bridgeInfo->jumpID;
				newLink->anchorIndex = bridgeInfo->pageAnchor;

				// �����N�A�Ԃ�o�^���ɃC���N�������g
				newLink->linkID = bigBridgeInfo->linkID;
				newLink->imglinkID = bigBridgeInfo->imglinkID;
				newLink->movlinkID = bigBridgeInfo->movlinkID;

				// �`�悷��̈�������N���Ƃ��ēo�^
				CRect tapRect = CRect(sx,0,sx+blockWidth,0);	// Y���W�͎��s���ɉ��߂����̂ŕs�v
				newLink->grappleRect = tapRect;
				rowRecord->linkProperties->Add(newLink);
				// �����N��ʂɂ�胊���N�A�Ԃ�ݒ肷��
				switch( bridgeInfo->linkType ){
					case LinkType_external:
						// �����N
						newText->linkID = bigBridgeInfo->linkID;
						break;
					case LinkType_picture:
						// �摜�����N
						newText->imglinkID = bigBridgeInfo->imglinkID;
						break;
					case LinkType_movie:
						// ���惊���N
						newText->movlinkID = bigBridgeInfo->movlinkID;
						break;
				}
			}
			rowRecord->textProperties->Add(newText);
		}else{
			delete newText;
		}

	// �O���̐ݒ�
	}else if( currentTag == Tag_gaiji ){
		GaijiProperty* newGaiji = NULL;

//		int blockWidth = charWidth;	// �������Ɠ���Ƃ���
		int blockWidth = emojiFixHeight + gaijiWidthOffset;	// �G������16�s�N�Z���Œ�iCImageList�𗘗p���邽�߁j

#ifndef WINCE
		// �����̑傫���ɍ��킹�Ċg��(�G�����ȉ��̃t�H���g�̏ꍇ�͂��̂܂�܂Ȃ̂ŏd�Ȃ邩����)
		if( emojiFixHeight < charHeight ){
			double gaijiScale = (double)charHeight / (double)emojiFixHeight;
			blockWidth = (int)(blockWidth*gaijiScale);
		}
#endif
		// �O�������Ɏ��܂�Ȃ��ꍇ�͉��s���čă`�������W�x��
		if( blockWidth > bigBridgeInfo->remainWidth ){
			// �����z�����ꍇ�͎����z��������������܂ŌJ��Ԃ�
			processState = ProcessState_FollowOfLine;
		}else{
			newGaiji = new GaijiProperty();
			// [m:xx] ���� xx �𒊏o���A���\�[�XID�Ƃ���
			newGaiji->resourceID = hashRecord->value.Mid(3, hashRecord->value.GetLength()-4);	// ���\�[�X���̒u�����s���ꍇ�͂����ł��������āI

			// �o�͗̈�̐ݒ�
			int sx = bigBridgeInfo->startWidth + (gaijiWidthOffset);
			CRect drawRect = CRect(sx,0,sx+blockWidth,0);	// Y���W�͎��s���ɉ��߂����̂ŕs�v
			newGaiji->drawRect = drawRect;

			// ���̃e�L�X�g�̊J�n���W���X�V(�O���̓J�c�J�c�Ȃ̂ŕ����̃X�y�[�V���O��O��ɓ����)
			bigBridgeInfo->startWidth += (blockWidth + gaijiWidthOffset);
			bigBridgeInfo->remainWidth -= (blockWidth + gaijiWidthOffset);
			// �z��ɒǋL
			rowRecord->gaijiProperties->Add(newGaiji);
		}
	// �����N�̐ݒ�
	}else if( currentTag == Tag_link ){
		// �����N�̗̈�o�^��text�̗̈�쐬���ɂ܂Ƃ߂čs��
		bridgeInfo->isLink = true;
//		bridgeInfo->jumpID = 0;

		// ����ID�Ȃ���������N�Ƃ��Ĉ��� // ���������Ă��Ȃ��̂Ŕ��肵�Ȃ�
		//if( bridgeInfo->jumpID == currentUIDNumber ){
		//	bridgeInfo->linkType = LinkType_internal;
		//}else{
			bridgeInfo->linkType = LinkType_external;	// �����N�^�C�v���O�������N
		//}

//		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
		bridgeInfo->pageAnchor = 0;
	// �����N�^�O�̏I��
	}else if( currentTag == Tag_end_link ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
		bigBridgeInfo->linkID++;	// �����N�A�Ԃ��C���N�������g
	// �摜�����N�̐ݒ�
	}else if( currentTag == Tag_img ){
		// �����N�̗̈�o�^��text�̗̈�쐬���ɂ܂Ƃ߂čs��
		bridgeInfo->isLink = true;
//		bridgeInfo->jumpID = 0;

		bridgeInfo->linkType = LinkType_picture;		// �����N�^�C�v���摜�����N

//		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
		bridgeInfo->pageAnchor = 0;
	// �摜�����N�^�O�̏I��
	}else if( currentTag == Tag_end_img ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
		bigBridgeInfo->imglinkID++;		// �摜�����N�A�Ԃ��C���N�������g
	// ���惊���N�̐ݒ�
	}else if( currentTag == Tag_mov ){
		// �����N�̗̈�o�^��text�̗̈�쐬���ɂ܂Ƃ߂čs��
		bridgeInfo->isLink = true;
//		bridgeInfo->jumpID = 0;

		bridgeInfo->linkType = LinkType_movie;			// �����N�^�C�v�����惊���N

//		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
		bridgeInfo->pageAnchor = 0;
	// ���惊���N�^�O�̏I��
	}else if( currentTag == Tag_end_mov ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
		bigBridgeInfo->movlinkID++;		// ���惊���N�A�Ԃ��C���N�������g
	}else{
		// ����ȊO�̃G���[�̏ꍇ
		//CString logStr;
		//logStr.Format(TEXT("Through tag!!(%s:%s)\r\n"),hashRecord->key,hashRecord->value);
		//OutputDebugString(logStr);
	}

	return(processState);
}



/// �V�s���̒ǉ�
void Ran2View::AddNewRowProperty(CPtrArray* rowPropertyArray,bool forceNewRow)
{
	RowProperty* newRowRecord = new RowProperty();
	newRowRecord->rowNumber = rowPropertyArray->GetSize(); // �s�ԍ���ݒ�
	newRowRecord->breakLimitPixel = screenWidth - NormalWidthOffset - (leftOffset*2);

	// frameProperty��������
	newRowRecord->frameProperty[0].frameType = FrameType_nothing;
	newRowRecord->frameProperty[1].frameType = FrameType_nothing;

	rowPropertyArray->Add(newRowRecord);
}


/// ��ׂ����̃��Z�b�g
void Ran2View::ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,BridgeProperty* bridgeInfo,ProcessStateEnum mode,int width)
{
	// �o�͉\���ƊJ�n�ʒu�������Z�b�g
	if( width != 0 ){
		bigBridgeInfo->screenWidth = width;		// ��ʂ̃N���C�A���g�̈��ݒ�
	}
	bigBridgeInfo->remainWidth = (screenWidth - NormalWidthOffset - (leftOffset*2)) - ((leftOffset+framePixel)*3*(bigBridgeInfo->frameNestLevel+1));
	bigBridgeInfo->startWidth = leftOffset;

	// BOL�����擪�̓˂��o�����I�t�Z�b�g����̂Œ��ӁI
	if( mode == ProcessState_BeginOfLine ){
		// �C���f���g���ݒ肳��Ă��鎞�͓˂��o���ʂ�ݒ�
		if( bridgeInfo->indentLevel != -1 ){
			// �J�n�s�����˂��o���ďo�͂���(�J�n�ʒu���ꕶ����)
			int indentOffset = (bridgeInfo->indentLevel) * (charWidth);
			// ���N������ꍇ�͎n�_���X�ɂ��炷
			indentOffset += (bigBridgeInfo->frameNestLevel+1) * (charWidth); 

			bigBridgeInfo->startWidth += indentOffset;
			bigBridgeInfo->remainWidth -= indentOffset;
		}
	}else{
		// �C���f���g���ݒ肳��Ă��鎞�͓˂��o���ʂ�ݒ�
		if( bridgeInfo->indentLevel != -1 ){
			// �C���f���g�ʂ̌v�Z
			int indentOffset = ((bridgeInfo->indentLevel+1) * (charWidth));
			// ���N������ꍇ�͎n�_���X�ɂ��炷
			indentOffset += (bigBridgeInfo->frameNestLevel+1) * (charWidth); 

			bigBridgeInfo->startWidth += indentOffset;
			bigBridgeInfo->remainWidth -= indentOffset;
		}
	}
}


/// ���N��top/bottom�I�����肩��
void Ran2View::ChangeFrameProperty(BigBridgeProperty* bigBridgeInfo)
{
	// ���N��top�������ւ���
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
 * DAT�t�@�C������̃f�[�^�\�z
 */
int Ran2View::LoadDetail(CStringArray* bodyArray, CImageList* pImageList)
{
#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("��LoadDetail�N�����̎c������:%d bytes\r\n"),memState.dwAvailPhys);
#endif

	m_pImageList = pImageList;

	int rc = 0;
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	m_offsetPixelY = 0;
	m_offsetPixelX = 0;			// ���X�N���[���I�t�Z�b�g��������

	// �p���X�N���[���p�ɒ��O�̕\����Ԃ̃R�s�[�����i���ʕ��j
	//m_memPanDC->FillSolidRect( 0 , 0 , screenWidth , screenHeight , RGB(128,128,128) );
	m_memPanDC->BitBlt( 0 , 0 , screenWidth , screenHeight , m_memDC , 0 , m_drawStartTopOffset , SRCCOPY );

	if( bodyArray != NULL ){
		PurgeMainRecord();
		parsedRecord = ParseDatData2(bodyArray,this->screenWidth); // dat�̕ϊ�����
		bodyArray->FreeExtra();
		bodyArray->RemoveAll();
//		delete bodyArray;
	}

	// �`��ɕK�v�ȍs����Ԃ�
	if( parsedRecord != NULL ){
		rc = parsedRecord->rowInfo->GetSize() - m_viewLineMax; 
	}
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));


#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("��LoadDetail�I�����̎c������:%d Bytes\r\n"),memState.dwAvailPhys);
#endif

#ifndef WINCE
	// GIF�A�j���͊����X�N���[����1/3���炢�̕p�x�̍X�V�ł����H
	SetTimer( TIMERID_ANIMEGIF, TIMER_INTERVAL_ANIMEGIF, NULL );
#endif
	return(rc);
}


/**
 * �C�ӂ̍s����I�t�X�N���[���ɕ`�悷��
 */
int	Ran2View::DrawDetail(int startLine, bool bForceDraw)
{
#ifdef DEBUG
//	wprintf( L"DrawDetail: %5d line, force[%s]\n", startLine, bForceDraw ? L"true" : L"false");
#endif

	if (startLine<0) {
		// �擪����яo���Ă����0�Ɉ����߂�
		startLine = 0;
	}

	// �ǂ̍s����`�悵������ۑ����Ă���
	m_drawOffsetLine = startLine;

	// ���R�[�h�̓W�J�~�X��͈͊O�̎w��͒e��
	if( parsedRecord == NULL || startLine > parsedRecord->rowInfo->GetSize() ){
		return(0);
	}

	//// �p���X�N���[���p�ɒ��O�̕\����Ԃ̃R�s�[�����i���ʕ��j
	////m_memPanDC->FillSolidRect( 0 , 0 , screenWidth , screenHeight , RGB(128,128,128) );
	//m_memPanDC->BitBlt( 0 , 0 , screenWidth , screenHeight , m_memDC , 0 , m_drawStartTopOffset , SRCCOPY );

	// �h��Ԃ�
//	m_memDC->PatBlt( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, WHITENESS );
//	m_memDC->FillSolidRect( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, RGB(255,255,255) );
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
	m_memDC->FillSolidRect( 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255,255,255) );

//	TRACE( L"[DrawRect]\n" );
//	TRACE( L" %dx%d\n", bmp.bmWidth, bmp.bmHeight );
//	TRACE( L" %dx%d\n", screenWidth, screenHeight );

	// �I�t�Z�b�g�X�N���[���p��N�s�]���ɕ`�悷��B
	const int N_OVER_OFFSET_LINES = 2;
	for(int i=-N_OVER_OFFSET_LINES; i<=m_viewLineMax+N_OVER_OFFSET_LINES ; i++){
		int targetLine = startLine + i;

		if (targetLine < 0) {
			continue;
		}

		// �͈͂��z������X���[
		if( parsedRecord->rowInfo->GetSize() <= targetLine ){
			break;
		}

		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(targetLine);
		if( row != NULL ){
			//TRACE(TEXT("%d�̕`����J�n���܂�\r\n"), targetLine);

			// �t���[���̕`��B�摜���ݒ肳��Ă�����`�悵�Ȃ�
			if( row->imageProperty.imageNumber == -1 ){
				this->DrawFrameProperty(i,row);
			}

			// �e�L�X�g�v�f�̏o��(�����A�����N�����A�Z���n���܂�)
			this->DrawTextProperty(i, row->textProperties);

			// �O���v�f�̏o��
			this->DrawGaijiProperty(i, row->gaijiProperties);
		}

		//TRACE( L" line : %d\n", targetLine );
	}
	m_memDC->Rectangle( 0, 0, 10, 10 );

	// �`��
//	this->Invalidate(FALSE);
	if (bForceDraw) {
		// �����I�ɕ`�悷��
		CDC* pDC = GetDC();

		DrawToScreen(pDC);

		ReleaseDC(pDC);
	}

	return(1);
}


/**
 * �s���̃e�L�X�g�v���p�e�B�̔z���`�悷��
 *
 * DrawDetail ����Ăяo�����
 *
 * line: �`�悵�����s
 * rowProperty: �s�̏��
 */
void Ran2View::DrawFrameProperty(int line,RowProperty* rowProperty)
{
	for(int i=0 ;i<FrameNestLevel ; i++){
		// ���N�̕`�悪�ݒ肳��Ă��鎞�̂ݕ`��
		if(	rowProperty->frameProperty[i].frameType != FrameType_nothing ){
			CRect drawRect;
			// �w�i�F�̕`��
			CBrush backBrush(rowProperty->frameProperty[i].backgroundColor);
			int sx = leftOffset + (i * frameOffset);
			int ex = (screenWidth - NormalWidthOffset) - leftOffset - (i * frameOffset);
			int sy = m_drawStartTopOffset + topOffset + (line*(charHeight+charHeightOffset));
			int ey = sy + (charHeight+charHeightOffset);
			drawRect = CRect(sx,sy,ex,ey);
			m_memDC->FillRect(drawRect,&backBrush);

			CPen framePen(PS_SOLID,1,rowProperty->frameProperty[i].penColor);
			// ��[�ƍ��E��`��
			if( rowProperty->frameProperty[i].frameType == FrameType_roof ){

				CPen* oldPen = m_memDC->SelectObject(&framePen);
				m_memDC->MoveTo(drawRect.left,drawRect.top);
				m_memDC->LineTo(drawRect.right,drawRect.top);

				m_memDC->MoveTo(drawRect.left,drawRect.top);
				m_memDC->LineTo(drawRect.left,drawRect.bottom);

				m_memDC->MoveTo(drawRect.right,drawRect.top);
				m_memDC->LineTo(drawRect.right,drawRect.bottom);

				m_memDC->SelectObject(oldPen);
			// ���[��k���ĕ`��
			}else if( rowProperty->frameProperty[i].frameType == FrameType_stool ){
				int sx = leftOffset + (i * frameOffset);
				int ex = (screenWidth - NormalWidthOffset) - leftOffset - (i * frameOffset);
				int sy;
				if( rowProperty->textProperties->GetCount() == 0 ){
					// �I�s�͎��̍s�ƘA������Ă��܂��̂�-1�s����
					sy = m_drawStartTopOffset + topOffset + ((line-1)*(charHeight+charHeightOffset));
				} else {
					// �I�s�Ƀe�L�X�g������ꍇ�͂��̂܂�
					sy = m_drawStartTopOffset + topOffset + ((line)*(charHeight+charHeightOffset));
				}
				int ey = sy + (charHeight+charHeightOffset);
				CRect drawRect = CRect(sx,sy,ex,ey);

				CPen* oldPen = m_memDC->SelectObject(&framePen);
				if( rowProperty->textProperties->GetCount() != 0 ){
					// �I�s�Ƀe�L�X�g������ꍇ�͍��E�̘g����`�悷��
					m_memDC->MoveTo(drawRect.left,drawRect.top);
					m_memDC->LineTo(drawRect.left,drawRect.bottom);

					m_memDC->MoveTo(drawRect.right,drawRect.top);
					m_memDC->LineTo(drawRect.right,drawRect.bottom);
				}
				m_memDC->MoveTo(drawRect.left,drawRect.bottom-framePixel);
				m_memDC->LineTo(drawRect.right,drawRect.bottom-framePixel);
				m_memDC->SelectObject(oldPen);

			// ���E�̘g�������`��
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
 * �s���̃e�L�X�g�v���p�e�B�̔z���`�悷��
 *
 * DrawDetail ����Ăяo�����
 *
 * @param line �`�悵�����s
 * @param textProperties textProperty���܂Ƃ߂��z��
 */
void Ran2View::DrawTextProperty(int line,CPtrArray* textProperties)
{
	bool bReverse = false;
	for(int j=0 ; j<textProperties->GetSize() ; j++){
		// �e�L�X�g�u���b�N�̏o��(��Ŋ֐�������)
		TextProperty* text = (TextProperty*)textProperties->GetAt(j);
		int sy = m_drawStartTopOffset + topOffset + framePixel + (line*(charHeight+charHeightOffset));
		// ��t���w��̏ꍇ�͕\���ʒu�����炷
		if( text->isDownHanging == true ){
			sy += (charHeight - charQHeight - framePixel);
		}
		int ey = sy + (charHeight+charHeightOffset);

		bReverse = false;
		if( ((m_activeLinkID.anchor >=0) && (m_activeLinkID.anchor == text->linkID) ) ||
			((m_activeLinkID.image >=0) && (m_activeLinkID.image == text->imglinkID) ) ||
			((m_activeLinkID.movie >=0) && (m_activeLinkID.movie == text->movlinkID) ) )
		{
			// �|�C���g���Ă��郊���N�A�Ԃ��e�L�X�g���̂��̂ƈ�v���Ă���Δ��]�t���O��ݒ�
			bReverse = true;
		}

		CRect drawRect = CRect(text->drawRect.left,sy,text->drawRect.right,ey);
		if( bReverse ){
			// ���]�\���p�ɍ����h��Ԃ�
			m_memDC->FillSolidRect( &drawRect , solidBlack );
		}

		// �A���_�[���C���̕`��
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

		// �����F�ƃt�H���g�̐؂�ւ�
		if( bReverse ) {
			// ���]�\���p �����F�����A�w�i�F����
			m_memDC->SetTextColor(text->backgroundColor);
			m_memDC->SetBkColor(text->foregroundColor);
		} else {
			// �m�[�}���\���p �e�L�X�g��񂩂�F���擾
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
 * �s���̃e�L�X�g�v���p�e�B�̔z���`�悷��
 *
 * DrawDetail ����Ăяo�����
 *
 * @param line �`�悵�����s
 * @param textProperties textProperty���܂Ƃ߂��z��
 */
void Ran2View::DrawGaijiProperty(int line,CPtrArray* gaijiProperties)
{
	// �O���̕`��(��Ŋ֐��ɂ���)
	for(int j=0 ; j<gaijiProperties->GetSize() ; j++){
		// �e�L�X�g�u���b�N�̏o��(��Ŋ֐�������)
		GaijiProperty* gaiji = (GaijiProperty*)gaijiProperties->GetAt(j);
		int sy = m_drawStartTopOffset + topOffset + framePixel + (line*(charHeight+charHeightOffset));

		if (m_pImageList!=NULL) {
			int imageIdx = _wtoi(gaiji->resourceID);
			// �����`�F�b�N
			if (0 <= imageIdx && imageIdx < m_pImageList->GetImageCount()) {
#ifdef WINCE
				m_pImageList->Draw( m_memDC, imageIdx, CPoint(gaiji->drawRect.left, sy), ILD_TRANSPARENT );
#else
				CString imagePath = theApp.m_imageCache.GetImagePath(imageIdx);

				// ran2ImageArray ����T��
				// �G�����͍��X�R�O�O��Ȃ̂őS���������Ⴄ
				Ran2Image* targetImage = NULL;
				for (int ran2ImageArrayIdx=0; ran2ImageArrayIdx<ran2ImageArray.GetCount(); ran2ImageArrayIdx++) {
					Ran2Image* image = (Ran2Image*)ran2ImageArray.GetAt(ran2ImageArrayIdx);
					if (image->m_strFilename == imagePath) {
						targetImage = image;
						break;
					}
				}
				if (targetImage==NULL) {
					// �V�K����
					targetImage = new Ran2Image(imagePath);
					ran2ImageArray.Add(targetImage);
				}

				Ran2Image* image = targetImage;
				if( imagePath.GetLength() > 0 && image->GetWidth() > 0 && image->GetHeight()){

					// �����̑傫���ɍ��킹�Ċg��(�G�����ȉ��̃t�H���g�̏ꍇ�͂��̂܂�܂Ȃ̂ŏd�Ȃ邩����)
					int hmWidth = image->GetWidth();
					int hmHeight = image->GetHeight();
					if( hmWidth < charHeight ){
						double gaijiScale = (double)charHeight / (double)hmHeight;
						hmWidth  = (int)(hmWidth *gaijiScale);
						hmHeight = (int)(hmHeight*gaijiScale);
					}

					// �G�����̍������������傫���ꍇ�A�x�[�X���C����ύX����
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
 * dat�t�@�C��������s���N���X�ւ̕ϊ�����2(Unicode�ɕϊ�����Ă��鎖���O��)
 *
 * LoadDetail ����Ăяo�����
 *
 * �t�@�C����CArchive�ň�s�Âǂނ̂ł͂Ȃ��A�ꊇ�œǂݍ����CStringArray�֕������Ă��珈�����s��
 */
MainInfo* Ran2View::ParseDatData2(CStringArray* datArray,int width)
{
	// �ԋp�l
	MainInfo* newMainRecord = new MainInfo();

	// �����C�����̈ꎞ�ۑ���
	CPtrArray*	rowPropertyArray = newMainRecord->rowInfo;

	// Main.uni�t�@�C���̏I�[�܂œ�������܂ŌJ��Ԃ�
	CString	lineStr;
	int		indentLevel = -1;	 // ����q���x��

	int lineNo = 0;
	int frameNestLevel = -1;		// ���N�̃l�X�g�`�惌�x��
	int	anchorCount = 0;			// �y�[�W���A���J�[�̌�
	BridgeProperty		bridgeInfo;		// �s���܂����C�����(�V�s�Ń��Z�b�g�����)
	BigBridgeProperty	bigBridgeInfo;	// �s���܂����C�����(�V�s�ł����Z�b�g����Ȃ�)

	// �ׂ����̃��Z�b�g
	this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BeginOfLine,width);

	// CFileArchive�Ńt�@�C���̏I���܂Ń��[�v������
	// ��ԍŏ��̍s��������ۂŒǉ�
	// (���̃��R�[�h�ɂ͕K��0�̃A���J�[������͂��Ȃ̂ōŌ�Ƀ`�F�b�N�������Ċm�F���邱�ƁI)
	RowProperty* topRowRecord = new RowProperty;
	topRowRecord->breakLimitPixel = screenWidth - NormalWidthOffset - (leftOffset*2);

	rowPropertyArray->Add(topRowRecord);

	int readCount = 0;
	for( int i=0 ; i<datArray->GetSize() ; i++ ){
		CString lineStr = datArray->GetAt(i);
		// �v�f���Ȃ��ꍇ�̓X�L�b�v
		if( lineStr.GetLength() <= 0 ){
			continue;
		}

		// ������Html�p�ɏC���I�I
		HtmlRecord*	hashRecord = new HtmlRecord();

		// �����A�G�����̐U�蕪��
		if( wcsncmp(lineStr, TEXT("[m:"), 3) == 0){
			// ���l, �C���f�b�N�X�`�F�b�N���ʂ�ΊO���A����ȊO�̓e�L�X�g�Ƃ݂Ȃ�
			CString code = lineStr.Mid(3,lineStr.GetLength()-4);
			for (int i=0; i<code.GetLength(); i++) {
				if (!isdigit(code[i])) {
					code = L"";
				}
			}
			if (code.IsEmpty()) {
				// ��O��
				hashRecord->type = Tag_text;
			} else {
				// �O��
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
			// ���p�J�n
			hashRecord->type = Tag_blockquote;
		}else if( lineStr.Compare(TEXT("</blockquote>")) == 0 ) {
			// ���p�I��
			hashRecord->type = Tag_end_blockquote;
		}else if( lineStr.Compare(TEXT("<a>")) == 0 ) {
			// �����N�J�n
			hashRecord->type = Tag_link;
		}else if( lineStr.Compare(TEXT("</a>")) == 0 ) {
			// �����N�I��
			hashRecord->type = Tag_end_link;
		}else if( lineStr.Compare(TEXT("<img>")) == 0 ) {
			// �摜�����N�J�n
			hashRecord->type = Tag_img;
		}else if( lineStr.Compare(TEXT("</img>")) == 0 ) {
			// �摜�����N�I��
			hashRecord->type = Tag_end_img;
		}else if( lineStr.Compare(TEXT("<mov>")) == 0 ) {
			// ���惊���N�J�n
			hashRecord->type = Tag_mov;
		}else if( lineStr.Compare(TEXT("</mov>")) == 0 ) {
			// ���惊���N�I��
			hashRecord->type = Tag_end_mov;
		}else{
			hashRecord->type = Tag_text;
		}
		hashRecord->value = lineStr;

		// �����̔z��̍Ō�����擾
		int lastRowPropertyIndex = rowPropertyArray->GetUpperBound();
		RowProperty* currentRowRecord = NULL;
		if( lastRowPropertyIndex >= 0 ){
			currentRowRecord = (RowProperty*)rowPropertyArray->GetAt(lastRowPropertyIndex);

			if( currentRowRecord == NULL ){
				break; // ���[�v���I��
			}
		}

		// �s���̐U�蕪������
		ProcessStateEnum rc = this->SetRowProperty(hashRecord, currentRowRecord, &bridgeInfo, &bigBridgeInfo);

		// �y�[�W���A���J�[�̍s�ʒu���L���b�V������
		if( bridgeInfo.inPageAnchor != -1 ){
			newMainRecord->anchorIndex[bridgeInfo.inPageAnchor] = rowPropertyArray->GetSize() - 1;
		}

		// �J�n�^�O�̓C���f���g�ʒu�̍Đݒ肾��
		if( rc == ProcessState_BeginOfLine ){

			// ��܂����������Z�b�g
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BeginOfLine);
		// �����z�����̌J��Ԃ������̏ꍇ 
		}else if( rc == ProcessState_FollowOfLine ){
			// �����z������Ȃ��Ȃ�܂ŌJ��Ԃ�
			while( rc == ProcessState_FollowOfLine ){
				//TRACE(TEXT(" �J�z��[%d]\r\n"),rowPropertyArray->GetSize());
				// �V�s���̒ǉ�
				this->AddNewRowProperty(rowPropertyArray);
				int lastRowPropertyIndex = rowPropertyArray->GetUpperBound();
				RowProperty* currentRowRecord = NULL;
				if( lastRowPropertyIndex >= 0 ){
					currentRowRecord = (RowProperty*)rowPropertyArray->GetAt(lastRowPropertyIndex);

					if( currentRowRecord == NULL ){
						break; // ���[�v���I��
					}
				}

				// ��܂����������Z�b�g
				this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
				// ���N��top/bottom�̏�Ԃ������ւ���
				this->ChangeFrameProperty(&bigBridgeInfo);
				// �܂�Ԃ��s���J��Ԃ��`��
				rc = this->SetRowProperty(hashRecord, currentRowRecord, &bridgeInfo, &bigBridgeInfo);
			}

			// �V�s���̒ǉ�
//			this->AddNewRowProperty(rowPropertyArray);
			// ��܂����������Z�b�g
//			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ���N��top/bottom�̏�Ԃ������ւ���
			this->ChangeFrameProperty(&bigBridgeInfo);

			//TRACE(TEXT(" �J�z�I��[%d]\r\n"),rowPropertyArray->GetSize());
		// �������s�͐V�K���R�[�h��New���ĒǋL 
		}else if( rc == ProcessState_ForceBreakLine ){
			// �V�s���̒ǉ�
			this->AddNewRowProperty(rowPropertyArray);
			// ��܂����������Z�b�g
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ���N��top/bottom�̏�Ԃ������ւ���
			this->ChangeFrameProperty(&bigBridgeInfo);

		// �I���^�O�͐V�������R�[�h��New���ĒǋL��ɍs�܂����������Z�b�g
		}else if( rc == ProcessState_EndOfLine ){
			// �V�s���̒ǉ�
			this->AddNewRowProperty(rowPropertyArray);
			// �s�܂����������Z�b�g
			memset(&bridgeInfo,NULL,sizeof(BridgeProperty));
			bridgeInfo.foregroundColor = solidBlack;
			bridgeInfo.backgroundColor = solidWhite;
			bridgeInfo.fontType = FontType_normal;
			bridgeInfo.indentLevel = -1;
			bridgeInfo.pageAnchor = -1;
			bridgeInfo.inPageAnchor = -1;

			// ��܂����������Z�b�g
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ���N��top/bottom�̏�Ԃ������ւ���
			this->ChangeFrameProperty(&bigBridgeInfo);
		}
		readCount++;
		
		delete hashRecord;
	}

	// MainInfo�ARowProperty�̏��ԂŃ������̓��e���t�@�C���փ_���v
	newMainRecord->propertyCount = rowPropertyArray->GetSize();

	return(newMainRecord);

}


/**
 * mainRecord�̔j��
 */
void Ran2View::PurgeMainRecord()
{
	// �s���̔j��
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
		// Ran2Image�̔j���������ł���Ƃ�
//		for(int cacheIndex=0 ; cacheIndex<ran2ImageArray.GetSize() ; cacheIndex++){
//			Ran2Image* image = (Ran2Image*)ran2ImageArray.GetAt(cacheIndex);
//			delete image;
//		}
//		ran2ImageArray.RemoveAll();
#endif
	}
}


// ���݈ʒu���ĕ`��
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


/// DPI�l�����W�X�g������擾
int Ran2View::GetScreenDPI()
{
	// ���W�X�g������̐ݒ���Ȃ�
	HKEY	hSettingKey;
	CString msgStr;

// PPC2003��WM5.0��DPI�̐ݒ肳��Ă���Ƃ��낪�Ⴄ�̂ŗv���ӁI
#ifndef WM2003
	if( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("\\System\\GWE\\Display"),0,KEY_ALL_ACCESS,&hSettingKey) == ERROR_SUCCESS ){
#else
	if( ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("\\Drivers\\Display\\GPE"),0,KEY_ALL_ACCESS,&hSettingKey) == ERROR_SUCCESS ){
#endif
		DWORD	regType = REG_DWORD;
		DWORD	valSize = sizeof(DWORD);
		DWORD	dispos = REG_OPENED_EXISTING_KEY;
		DWORD	DPIVal;
		// DPI�l���擾
		if( hSettingKey != NULL ){
			::RegQueryValueEx(hSettingKey,TEXT("LogicalPixelsY"),NULL,&regType,(LPBYTE)&DPIVal,&valSize);
			::RegCloseKey(hSettingKey);
			return(DPIVal);
		}
	}

	// ���f�ł��Ȃ��ꍇ��96DPI�Ƃ������ɂ��Ă���
	return(96);
}


//-------------------------------------------------------------------------------------------------
// �ȉ��AUser I/F (Behavior) �֘A
//-------------------------------------------------------------------------------------------------


/**
 * �E�B���h�E�v���V�[�W��
 *
 * ���ɏ����Ȃ�
 */
LRESULT Ran2View::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WindowProc(message, wParam, lParam);
}


/**
 * �p���X�N���[���J�n
 *
 * �X�N���[�������idirection�j�ɏ]���I�t�Z�b�g�l�ƍ�����ݒ肵�ă^�C�}�[���N������
 */
void Ran2View::StartPanDraw(PAN_SCROLL_DIRECTION direction)
{
	if (!m_bUsePanScrollAnimation) {
		// �I�v�V�����ŃI�t�ɂȂ��Ă���̂Ŗ�������
		return;
	}

	KillTimer( TIMERID_PANSCROLL );

	switch (direction) {
	case PAN_SCROLL_DIRECTION_RIGHT:
		// �E�����փX�N���[��
		
		// ���ֈ��ʂ��ꂽ�Ƃ��납��J�n
		m_offsetPixelX = - screenWidth;
		// �ړ�����
#ifndef WINCE
		m_dPxelX = screenWidth / 10 ;
#else
		m_dPxelX = screenWidth / 18 ;
#endif
		break;

	case PAN_SCROLL_DIRECTION_LEFT:
		// �������փX�N���[��

		// �E�ֈ��ʂ��ꂽ�Ƃ��납��J�n
		m_offsetPixelX = screenWidth;
		// �ړ�����
#ifndef WINCE
		m_dPxelX = - screenWidth / 10 ;
#else
		m_dPxelX = - screenWidth / 18 ;
#endif
		break;
	}

	// �p���X�N���[�����t���O�ݒ�
	m_bAutoScrolling = true;

	// �p���X�N���[���J�n����
	m_dwPanScrollLastTick = GetTickCount();

	// �p���X�N���[���J�n
	SetTimer( TIMERID_PANSCROLL, TIMER_INTERVAL_PANSCROLL, NULL );
}


/**
 * �c�X�N���[���C�x���g
 */
void Ran2View::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


/**
 * ���_�u���N���b�N�C�x���g
 */
void Ran2View::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
}


/**
 * �����h���b�O�����i�����X�N���[���A�p���X�N���[���j�̒�~
 */
void Ran2View::ResetDragOffset(void)
{
	// �����X�N���[����~
	KillTimer(TIMERID_AUTOSCROLL);
	
	// �p���X�N���[����~
	KillTimer(TIMERID_PANSCROLL);

	// �p���X�N���[�����t���O�N���A
	m_bAutoScrolling = false;

	// �I�t�Z�b�g������
	m_offsetPixelY = 0;
	m_offsetPixelX = 0;
}


/**
 * �E�N���b�N�I���C�x���g
 */
void Ran2View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// �e�̌Ăяo��
	::SendMessage( GetParent()->GetSafeHwnd(), WM_RBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );

	CWnd::OnRButtonUp(nFlags, point);
}


/**
 * ���N���b�N�I���C�x���g
 *
 * �^�b�v�ɂ�郊���N�ʒu�̒T��
 */
void Ran2View::OnLButtonUp(UINT nFlags, CPoint point)
{
	int dx = m_ptDragStart.x - point.x;
	int dy = m_ptDragStart.y - point.y;

	if (m_bDragging) {
		//m_bDragging = false;		// �h���b�O���t���O�͌�̏����Ŕ���Ɏg��
		//::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
		ReleaseCapture();

		// dx,dy�̃h���b�O�ʂɉ����āA�h���b�O�J�n���ǂ����𔻒肷��
		// m_bPanDragging, m_bScrollDragging ���ݒ肳���
		MySetDragFlagWhenMovedPixelOverLimit(dx,dy);
	}

	// �_�u���N���b�N����
	if (m_dwFirstLButtonUp>0 &&
		(GetTickCount() - m_dwFirstLButtonUp) < GetDoubleClickTime())
	{
		// �_�u���N���b�N�ς݂Ȃ̂ŃN���A����
		m_dwFirstLButtonUp = 0;
		// �h���b�O���t���O���N���A
		m_bDragging = false;		
		m_bPanDragging = false;
		m_bScrollDragging = false;
		m_activeLinkID.clear();

		// �_�u���N���b�N�Ƃ݂Ȃ�
		// �e�̌Ăяo��
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	} else {
		// �h���b�O����

		if( m_bUseDoubleClickMove ) {
			// �I�v�V������on�Ȃ�΃_�u���N���b�N����
			m_dwFirstLButtonUp = GetTickCount();
		} else {
			// �I�v�V������off�̏ꍇ�_�u���N���b�N���肵�Ȃ�
			m_dwFirstLButtonUp = 0;
		}

		m_ptFirstLButtonUp = point;

		if( m_bPanDragging ){ 
			// �������Ƀh���b�O
#ifdef DEBUG
			wprintf( L"OnLButtonUp: %5d\n" , dx);
#endif
			if( dx > 0 ) {
				// ������
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

				// �����I�ɕ`�悷��
				// ���̂܂܂ł͎��̃R�����g�������Ă��`�悳��Ă��܂��̂ŕۗ�
				// �i����WM_PAINT�������猩���Ă��܂��͔̂閧���j
				//CDC* pDC = GetDC();
				//DrawToScreen(pDC);
				//ReleaseDC(pDC);
				// ���̃R�����g��\��
				::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM( 10 , 1000 ));
			} else {
				// �E����
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

				// �����I�ɕ`�悷��
				// ���̂܂܂ł͎��̃R�����g�������Ă��`�悳��Ă��܂��̂ŕۗ�
				// �i����WM_PAINT�������猩���Ă��܂��͔̂閧���j
				//CDC* pDC = GetDC();
				//DrawToScreen(pDC);
				//ReleaseDC(pDC);
				// �O�̃R�����g��\��
				::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM( 10 , 0 ));
			}
		} else if( m_bScrollDragging ) {
			// �c�����Ƀh���b�O
			m_autoScrollInfo.push( GetTickCount(), point );
			double speed = m_autoScrollInfo.calcMouseMoveSpeedY();
			TRACE( L"! speed   : %5.3f [px/msec]\n", speed );

			KillTimer( TIMERID_AUTOSCROLL );
			m_bAutoScrolling = false;
			if( GetAllLineCount()-GetViewLineMax() > 0 ) {
				// �X�N���[���\�Ȃ�΁i�Ɣ��肵�Ȃ��ƃX�N���[���s�\��ԂŃ^�C�}�[����������j
				// �����X�N���[���J�n
				m_dwAutoScrollStartTick = GetTickCount();
				m_yAutoScrollMax = 0;
				m_bAutoScrolling = true;
				SetTimer( TIMERID_AUTOSCROLL, TIMER_INTERVAL_AUTOSCROLL, NULL );
			} else {
				// �����N�����N���A���čĕ\������
				m_activeLinkID.clear();
				DrawDetail( m_drawOffsetLine , true );
			}
		} else {
			// ���N���b�N�����̏������s��

			// �^�b�v�ʒu�̍s�ԍ����擾
			int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

			// Row�z�񂩂�̎擾�ʒu���Z�o
			int rowNumber = m_drawOffsetLine + tapLine;

			// �^�b�v�ʒu���͈͓����z����ꍇ�͉������Ȃ�
			if( parsedRecord->rowInfo->GetSize() > rowNumber  && rowNumber >= 0 ){
				RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
				for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
					LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);

					CString logStr;
					logStr.Format(TEXT("�����N����[Type:%d][ID:%d][PA:%d][left:%d]�`[right:%d]\r\n"),
						linkInfo->linkType, linkInfo->linkID , linkInfo->anchorIndex,
						linkInfo->grappleRect.left, linkInfo->grappleRect.right);
					OutputDebugString(logStr);

					// �����N�\���͈͓��ł���΃����N��ʂɂ��e�E�C���h�E���Ăяo���ĊJ��
					if( linkInfo->grappleRect.left <= point.x && linkInfo->grappleRect.right >= point.x ){
						if( linkInfo->linkType == LinkType_internal ){
							// �����i���ꃆ�[�UID�H�j�����N�i���󖢎g�p�H�j
							break;
						}else if( linkInfo->linkType == LinkType_external ){
							// �O�������N�imixiURL�Ahttp:�Attp:�AYouTube�j
							::SendMessage( GetParent()->GetSafeHwnd(), WM_COMMAND, (WPARAM)MAKEWPARAM(  ID_REPORT_URL_BASE + 1 + linkInfo->linkID , 0 ), (LPARAM)NULL );
							//logStr = L"LinkType_external" + logStr;
							//MessageBox( logStr );
							break;
						}else if( linkInfo->linkType == LinkType_picture ){
							// �摜�����N�i���L�A�R�~���R�����g�A�t�H�g�A���o���j
							::SendMessage( GetParent()->GetSafeHwnd(), WM_COMMAND, (WPARAM)MAKEWPARAM(  ID_REPORT_IMAGE + 1 + linkInfo->imglinkID , 0 ), (LPARAM)NULL );
							//logStr = L"LinkType_picture" + logStr;
							//MessageBox( logStr );
							break;
						}else if( linkInfo->linkType == LinkType_movie ){
							// ���惊���N�imixi����j
							::SendMessage( GetParent()->GetSafeHwnd(), WM_COMMAND, (WPARAM)MAKEWPARAM(  ID_REPORT_MOVIE + 1 + linkInfo->movlinkID , 0 ), (LPARAM)NULL );
							//logStr = L"LinkType_movie" + logStr;
							//MessageBox( logStr );
							break;
						}
					}
				}
			}
			// �����N�����N���A���čĕ\������
			if( !m_bAutoScrolling ){
				m_activeLinkID.clear();
				DrawDetail( m_drawOffsetLine , true );
			}
		}

		//�h���b�O������~
		m_bDragging = false;
		m_bPanDragging = false;
		m_bScrollDragging = false;
		m_activeLinkID.clear();
		
		// �e�̌Ăяo��
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	}

	//CWnd::OnLButtonUp(nFlags, point);
}


/**
 * ���N���b�N�J�n�C�x���g
 *
 * �^�b�v�ɂ�郊���N�̏���
 */
void Ran2View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		return;
		m_bDragging = false;
		::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
	}

	LinkID lastLinkID = m_activeLinkID;

	// �^�b�v�ʒu�̍s�ԍ����擾
	int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

	// Row�z�񂩂�̎擾�ʒu���Z�o
	int rowNumber = m_drawOffsetLine + tapLine;

	bool bLinkArea = false;

	//�����N������OnLButtonUp�ōs��
//	// �^�b�v�ʒu���͈͓����z����ꍇ�͉������Ȃ�
	// �����N�A�ԃN���A
	m_activeLinkID.clear();
	if( parsedRecord->rowInfo->GetSize() > rowNumber && rowNumber >= 0 ){
		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
		for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
			LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);
			// �����N�͈͂̊Y���������H
			if( linkInfo->grappleRect.left <= point.x && linkInfo->grappleRect.right >= point.x ){
				// �����N�|�C���g���t���O�𗧂Ă�
				bLinkArea = true;
				if( m_bScrollDragging != true && m_bPanDragging != true ){
					// �h���b�O���łȂ���΃����N�^�C�v�ɍ��킹�ă����N�A�Ԃ�ݒ肷��
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
		// �����N�|�C���g��Ԃɕω��������
		// �����N���]�\���p�Ɉ��ʍĕ`�恨���������I
		DrawDetail( m_drawOffsetLine , true );
	}

	// �h���b�O�J�n
	// �}�E�X�J�[�\���ݒ�
	if (GetAllLineCount()-GetViewLineMax() > 0) {
		// �X�N���[���\
		if( bLinkArea ) {
			// �����N�|�C���g���͎w����
			::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
		} else {
			// �O���u
			::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );
		}
	} else {
		if( bLinkArea ) {
			// �����N�|�C���g���͎w����
			::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
		}
	}

	// �L���v�`���J�n
	SetCapture();

	m_bDragging = true;
	m_ptDragStart = point;
	m_dragStartLine = m_drawOffsetLine;

	// �܂��h���b�O�����͊m�肵�Ă��Ȃ�
	m_bPanDragging = false;
	m_bScrollDragging = false;

	// �����X�N���[����~
	KillTimer( TIMERID_AUTOSCROLL );
	m_bAutoScrolling = false;

	// �����X�N���[�����
	m_autoScrollInfo.clear();
	m_autoScrollInfo.push( GetTickCount(), point );

//	CWnd::OnLButtonDown(nFlags, point);
}


/**
 * ���݂̃X�N���[���ʒu�̎擾
 */
int Ran2View::MyGetScrollPos()
{
	if (m_offsetPixelY<0) {
		// pixel�I�t�Z�b�g����̏ꍇ�́u�\���s+1�s�v��Ԃ�
		return m_drawOffsetLine+1;
	} else {
		return m_drawOffsetLine;
	}
}


/**
 * �}�E�X�ړ��C�x���g
 */
void Ran2View::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef DEBUG
//	wprintf( L"OnMouseMove\n" );
#endif

	LinkID lastLinkID    = m_activeLinkID;

	// N�s�N�Z���ړ�������_�u���N���b�N�L�����Z��
	if (m_dwFirstLButtonUp!=0) {
		int dx = point.x - m_ptFirstLButtonUp.x;
		int dy = point.y - m_ptFirstLButtonUp.y;
//		TRACE( L"dx,dy = (%3d,%3d), r^2=%3d\n", dx, dy, dx*dx+dy*dy );
#ifdef WINCE
		// WindowsMobile : �^�b�v�̂��߂��Ȃ�傫�ȃY����������
		// TODO : �{���� DPI �l�Ŏ������������[�U���ɕύX�\�ɂ��ׂ��B
		const int N = 30;
#else
		// Windows : �N���b�N�̂��߃Y���͂��Ȃ菬����
		const int N = 2;
#endif
		if (dx*dx + dy*dy > N*N) {
//			CString msg;
//			msg.Format( L"�_�u���N���b�N�L�����Z��, dx,dy = (%3d,%3d), r^2=%3d\n", dx, dy, dx*dx+dy*dy );
//			MZ3LOGGER_DEBUG( msg );
			m_dwFirstLButtonUp = 0;
		}
	}

	bool bLinkArea = false;

	// �^�b�v�ʒu�̍s�ԍ����擾
	int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

	// Row�z�񂩂�̎擾�ʒu���Z�o
	int rowNumber = m_drawOffsetLine + tapLine;
	// �}�E�X�ʒu���͈͓����z����ꍇ�͉������Ȃ�
	// �����N�A�ԃN���A
	m_activeLinkID.clear();
	if( parsedRecord->rowInfo->GetSize() > rowNumber  && rowNumber >= 0 ){
		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
		for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
			LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);
			// �����N�\���͈͓����H
			if( linkInfo->grappleRect.left <= point.x && linkInfo->grappleRect.right >= point.x ){
				// �����N�|�C���g���t���O�𗧂Ă�
				bLinkArea = true;
				if( m_bScrollDragging != true && m_bPanDragging != true ){
					// �h���b�O���łȂ���΃����N�^�C�v�ɍ��킹�ă����N�A�Ԃ�ݒ肷��
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
				//logStr.Format(TEXT("���W[%d,%d][m_offsetPixelY:%d][charHeightOffset + charHeight:%d][tapLine:%d][drawOffsetLine:%d]\r\n"),
				//	point.x , point.y , m_offsetPixelY,
				//	charHeightOffset + charHeight, tapLine,drawOffsetLine);
				//OutputDebugString(logStr);
			}
		}
	}

	if (m_bDragging) {
		// ���{�^���������ăh���b�O��
		int dx = m_ptDragStart.x - point.x;
		int dy = m_ptDragStart.y - point.y;
#ifdef DEBUG
		if (false) {
			wprintf( L"dx : %5d\n", dx );
			wprintf( L"dy : %5d\n", dx );
		}
#endif

		// dx,dy�̃h���b�O�ʂɉ����āA�h���b�O�J�n���ǂ����𔻒肷��
		// m_bPanDragging, m_bScrollDragging ���ݒ肳���
		MySetDragFlagWhenMovedPixelOverLimit(dx,dy);

		if( GetAllLineCount()-GetViewLineMax() > 0 ) {
			// �c�X�N���[���\�Ȃ��
			if( m_bScrollDragging ){
				// �����X�N���[�������W
				m_autoScrollInfo.push( GetTickCount(), point );

				// �h���b�O����
				ScrollByMoveY( dy );
			}
		}
		
		if( m_bPanDragging ){
			// ���X�N���[�����Ȃ�΃}�E�X�|�C���^�ύX
			if( dx>0 ) {
				// ������
				::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_LEFT_CURSOR) );
			} else {
				// �E����
				::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_RIGHT_CURSOR) );
			}
		} else {
			// �����N�\���͈͓��Ȃ�w�����J�[�\���ɕύX
			if( bLinkArea ) {
				::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
			} else {
				// �X�N���[���\�ł���΁u�O�[�v�̃J�[�\���ɕύX
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
					// �����N�|�C���g��Ԃɕω��������
					// �����N���]�\���p�Ɉ��ʍĕ`�恨���������I
					DrawDetail( m_drawOffsetLine , true );
				}
			}
		}

	} else {
		// ���{�^���𗣂��Ĉړ���

		// �����N�\���͈͓��Ȃ�w�����J�[�\���ɕύX
		if( bLinkArea ) {
			::SetCursor( ::LoadCursor(NULL, IDC_HAND) );
		} else {
			// �X�N���[���\�ł���΁u�p�[�v�̃J�[�\���ɕύX
			if (GetAllLineCount()-GetViewLineMax() > 0) {
				::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
			} else {
				::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
			}
		}
		// �I�[�g�X�N���[�����łȂ���΃����N���]�\���p�ɍĕ`�恨���������I
		if( !m_bAutoScrolling ) {
			if (lastLinkID.anchor != m_activeLinkID.anchor ||
				lastLinkID.image != m_activeLinkID.image ||
				lastLinkID.movie != m_activeLinkID.movie)
			{
				// �����N�|�C���g��Ԃɕω��������
				// �����N���]�\���p�Ɉ��ʍĕ`�恨���������I
				DrawDetail( m_drawOffsetLine, true );
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


/**
 * dx,dy�̃h���b�O�ʂɉ����āA�h���b�O�J�n���ǂ����𔻒肵�A�ϐ���ݒ肷��
 *
 * �h���b�O�J�n���� m_bPanDragging, m_bScrollDragging ��ݒ肷��
 */
void Ran2View::MySetDragFlagWhenMovedPixelOverLimit(int dx, int dy)
{
	// �c�h���b�O�J�n���f�p�I�t�Z�b�g�l
#ifndef WINCE
	// win32�̏ꍇ���s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
	int dyMinLimit = ( charHeight+charHeightOffset ) / 2 + 1 ;
#else
	// WM�̏ꍇ��s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
	int dyMinLimit = charHeight+charHeightOffset;
#endif

	if (m_bPanDragging) {
		// ���h���b�O��
		//if( abs( dx ) < screenWidth / 3 ){
		//	// �}�E�X�����ɖ߂����牡�h���b�O���L�����Z���@�����������₵���̂ŕۗ�
		//	m_bPanDragging = false ;
		//}
	} else if (m_bScrollDragging) {
		// �c�h���b�O��

	} else {
		// �h���b�O�������m�肵�Ă��Ȃ�
		if( m_bUseHorizontalDragMove &&
			( abs(dx) > abs(dy) && abs(dx) > screenWidth / 3 ) ) {
			// �������̈ړ��ʂ��傫���Ĉړ��ʂ���ʂ�1/3�ȏ�̏ꍇ
			// ���h���b�O�J�n
			m_bPanDragging = true;
		} else if(  abs(dx) < abs(dy) && abs(dy) > dyMinLimit ) {
			// �c�����̈ړ��ʂ��傫���Ĉړ��ʂ��h���b�O�J�n�I�t�Z�b�g�ȏ�̏ꍇ
			if( GetAllLineCount()-GetViewLineMax() > 0 ) {
				// �c�X�N���[���\�Ȃ��
				// �c�h���b�O�J�n
				m_bScrollDragging = true;
			}
		}
	}
}


/**
 * �h���b�O���̕`�揈��
 *
 * �`��͈͂𒴂����ꍇ true ��Ԃ�
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
		// �`��Ώۃf�[�^���Ȃ����߃G���[�Ƃ���
		return true;
	}

	bool bLimitOver = false;
	const int lineHeightPixel = (charHeight + charHeightOffset);	// 1�s������̃s�N�Z����
	int offsetLine = dy / lineHeightPixel;	// �h���b�O�J�n�s����̃I�t�Z�b�g�s

	TRACE_ScrollByMoveY( L"---\n" );
	TRACE_ScrollByMoveY( L"m_dragStartLine + offsetLine : %5d\n", m_dragStartLine + offsetLine );
	TRACE_ScrollByMoveY( L"dy               : %5d\n", dy );
	TRACE_ScrollByMoveY( L"m_offsetPixelY   : %5d\n", m_offsetPixelY );
	TRACE_ScrollByMoveY( L"m_viewLineMax    : %5d\n", m_viewLineMax );
	TRACE_ScrollByMoveY( L"m_dragStartLine  : %5d\n", m_dragStartLine );

	if (-dy % lineHeightPixel <=0 || m_dragStartLine + offsetLine > 0) {
		// �s�N�Z���I�t�Z�b�g
		// �i0�s�ڂł���Ώ�����̃X�N���[���͍s��Ȃ��j
		m_offsetPixelY = -dy % lineHeightPixel;

		TRACE_ScrollByMoveY( L"Y�s�N�Z���I�t�Z�b�g�Čv�Z�F%d\n", m_offsetPixelY );
	} else if( m_dragStartLine + offsetLine == 0 ) {
		// 0�s�ڂȂ�ΐ擪�ʒu��0����
		m_offsetPixelY = 0;

		TRACE_ScrollByMoveY( L"0�s�ڂ̂���Y�s�N�Z���I�t�Z�b�g������\n" );
		bLimitOver = true;
	}

	int nAllLine = GetAllLineCount();

	TRACE_ScrollByMoveY( L"nAllLine : %5d\n", nAllLine );
	TRACE_ScrollByMoveY( L"m_dragStartLine +offsetLine +m_viewLineMax : %5d\n", m_dragStartLine +offsetLine +m_viewLineMax );
	TRACE_ScrollByMoveY( L"m_offsetPixelY : %5d\n", m_offsetPixelY );
	TRACE_ScrollByMoveY( L"screenHeight %% lineHeightPixel : %5d\n", screenHeight % lineHeightPixel );

	// nAllLine        : �f�[�^�̑S�s�� [line]
	// m_dragStartLine : �h���b�O�J�n�s [line]
	// offsetLine      : �h���b�O�J�n�s����̃I�t�Z�b�g�s�� [line]
	// m_viewLineMax   : 1��ʂŕ\���\�ȍs�� [line]
	// screenHeight    : 1��ʂ̍��� [px]
	// lineHeightPixel : 1�s������̃s�N�Z���� [px]
	// m_offsetPixelY  : �I�t�Z�b�g�s�N�Z�� [px]

	bool bBottomLimitOver = false;
	if (m_offsetPixelY>=0) {
		if (m_dragStartLine +offsetLine +m_viewLineMax > nAllLine) {
			// �s������
			TRACE_ScrollByMoveY( L"�s������(1)\n" );
			bBottomLimitOver = true;
		} else if (m_dragStartLine +offsetLine +m_viewLineMax == nAllLine && 
		           m_offsetPixelY <= screenHeight % lineHeightPixel)
		{
			// �ŏI�s���������s�N�Z���I�t�Z�b�g����
			TRACE_ScrollByMoveY( L"�ŏI�s���������s�N�Z���I�t�Z�b�g����(1)\n" );
			bBottomLimitOver = true;
		}
	} else {
		// m_offsetPixelY <0 �̏ꍇ�F
		if (m_dragStartLine +offsetLine +m_viewLineMax+1 > nAllLine) {
			// �s������
			TRACE_ScrollByMoveY( L"�s������(2)\n" );
			bBottomLimitOver = true;
		} else if (m_dragStartLine +offsetLine +m_viewLineMax+1 == nAllLine && 
				   m_offsetPixelY <= -(lineHeightPixel -screenHeight % lineHeightPixel))
		{
			// �ŏI�s���������s�N�Z���I�t�Z�b�g����
			TRACE_ScrollByMoveY( L"�ŏI�s���������s�N�Z���I�t�Z�b�g����(2)\n" );
			bBottomLimitOver = true;
		}
	}
	if (bBottomLimitOver) {
		TRACE_ScrollByMoveY( L"... ���ɂ݂͂ł����߁A�ĉ��[�܂Ŗ߂�\n" );
		// ���ɂ͂ݏo���ꍇ�ŉ��[�܂ň����߂�
		offsetLine     = nAllLine - m_viewLineMax - m_dragStartLine;
		m_offsetPixelY = screenHeight % lineHeightPixel;
		bLimitOver = true;
	}

	// �`��
	TRACE_ScrollByMoveY( L"DrawDetail, from %d [line], offset %d [pixel]\n", m_dragStartLine + offsetLine, m_offsetPixelY );
	DrawDetail(m_dragStartLine + offsetLine, true);

	// �X�N���[���ʒu���ω�������������Ȃ��̂ŃI�[�i�[�ɒʒm
	CPoint lastPoint = m_autoScrollInfo.getLastPoint();
	::SendMessage( GetParent()->GetSafeHwnd(), WM_MOUSEMOVE, (WPARAM)0, (LPARAM)MAKELPARAM(lastPoint.x, lastPoint.y+dy) );

	return bLimitOver;
}


/**
 * �^�C�}�[�C�x���g
 */
void Ran2View::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == TIMERID_AUTOSCROLL ){
		// �����X�N���[��
#ifdef DEBUG
		wprintf( L"OnTimer, TIMERID_AUTOSCROLL\n" );
#endif

		// ���z�I�Ȉړ��ʎZ�o
		int dt = GetTickCount() - m_dwAutoScrollStartTick;
		// �[���I�ȃ}�C�i�X�̉����x�Ƃ���B
#ifdef WINCE
		double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.20;	// �}�C�i�X�̉����x, �X�P�[�����O�͓K����
#else
		double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.06;	// �}�C�i�X�̉����x, �X�P�[�����O�͓K����
#endif
		double speed = m_autoScrollInfo.calcMouseMoveSpeedY();

		int dyByAccel    = (int)((accel*dt*dt)/2.0);		// �}�C�i�X�̉����x
		int dyByVelocity = (int)(dt * speed);				// �����ɂ��ړ�	
		int dyAutoScroll = dyByAccel + dyByVelocity;		// LButtonUp ����̈ړ���

#if 0
		wprintf( L" dt : %5d, speed : %5.2f, accel : %5.6f, dy : %5d (%5d,%5d)\n", 
			dt, speed, accel, dyAutoScroll, dyByAccel, dyByVelocity );
#endif
		// �ő�ʒu���߂����i�ɓ_�𒴂����j�A
		// �����x���������l��菬�����A
		// �܂���N�b�o�߂����Ȃ�I��
		if (speed == 0.0 ||
			(speed < 0 && dyAutoScroll > m_yAutoScrollMax) ||
			(speed > 0 && dyAutoScroll < m_yAutoScrollMax) ||
			(fabs(accel)<0.00005) ||
			dt > 5 * 1000)
		{
			KillTimer(nIDEvent);
			m_bAutoScrolling = false;
		} else {
			// dyAutoScroll �������ړ�����B
			CPoint lastPoint = m_autoScrollInfo.getLastPoint();
			int dy = m_ptDragStart.y - lastPoint.y - dyAutoScroll;
			if (ScrollByMoveY( dy )) {
				// �͈͒��߂̂��ߏI��
				KillTimer(nIDEvent);
				m_bAutoScrolling = false;
			}
		}

		m_yAutoScrollMax = dyAutoScroll;
#ifdef DEBUG
		//wprintf( L"m_yAutoScrollMax:%5d\n" , m_yAutoScrollMax);
#endif
	} else if( nIDEvent == TIMERID_PANSCROLL ) {
		// �p���X�N���[��
#ifdef DEBUG
		wprintf( L"OnTimer, TIMERID_PANSCROLL\n" );
#endif
		int dwDt = GetTickCount() - m_dwPanScrollLastTick;
		m_dwPanScrollLastTick = GetTickCount();

		if( m_dPxelX == 0 ) {
			// �ړ��ʃ[���Ȃ疳�����[�v�h�~�̂��ߒ��~
			m_offsetPixelX = 0;
			m_bAutoScrolling = false;
			KillTimer(nIDEvent);
		} else {
			// �ړ�����
			m_offsetPixelX += dwDt * m_dPxelX / 10;
#ifdef DEBUG
			wprintf( L"m_offsetPixelX = %5d, dwDt = %5d\n"  , m_offsetPixelX , dwDt );
#endif
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
			if( m_offsetPixelX == 0 ){
				// ���ʕ��ړ�����
				// �p���X�N���[���I��
				m_bAutoScrolling = false;
				KillTimer(nIDEvent);
				m_dPxelX = 0;
			} 
			// �����I�ɕ`�悷��
			CDC* pDC = GetDC();
			DrawToScreen(pDC);
			ReleaseDC(pDC);
		}

	}

	CWnd::OnTimer(nIDEvent);
}
