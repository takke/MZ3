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
COLORREF solidBlack = COLORREF(RGB(0x00,0x00,0x00));
COLORREF solidBlue = RGB(0x00,0x00,0xFF);
COLORREF solidWhite = RGB(0xFF,0xFF,0xFF);
COLORREF solidPink = RGB(0xFF,0xC0,0xCB);
COLORREF solidDarkBlue = RGB(0x00,0x00,0x8B);
COLORREF lightBlue = COLORREF(RGB(0x94,0xD2,0xF1));
COLORREF lightGray = COLORREF(RGB(0xD0,0xD0,0xD0));

#define TIMERID_AUTOSCROLL	1
#define TIMERID_ANIMEGIF	2
#define TIMERID_PANSCROLL	3

const int emojiFixHeight = 16;	// ���܂̂Ƃ���Œ蒷�̊G���������Ȃ��̂ł���ł����H

// �e�v���p�e�B�̃R���X�g���N�^/�f�X�g���N�^
MainInfo::MainInfo() : rowInfo(NULL)
{
	// �A���J�[�ʒu��-1�ŏ�����
	memset(this->anchorIndex,0xFFFFFFFF,sizeof(int)*PageAnchorMax);
	this->anchorIndex[0] = 0;	// �A���J�[�̖����f�[�^������̂�0�s�ڂ�K���ݒ�

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
	linkID = -1;			// �����N�A�Ԃ̏�����
	imglinkID = -1;			// �摜�����N�A�Ԃ̏�����
	movlinkID = -1;			// ���惊���N�A�Ԃ̏�����
}


TextProperty::~TextProperty()
{
}


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


BridgeProperty::~BridgeProperty()
{
}


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

BigBridgeProperty::~BigBridgeProperty()
{
}


// �`��R���g���[���u�����v�R���X�g���N�^
Ran2View::Ran2View()
	: m_bDragging(false)
	, m_dragStartLine(0)
	, m_offsetPixelY(0)
	, m_dwFirstLButtonUp(0)
	, m_pImageList(NULL)
	, m_drawStartTopOffset(0)
{
	// �����o�̏�����
	// ��ʉ𑜓x���擾
	CurrentDPI = this->GetScreenDPI();
	topOffset = 0;	// ��ʏ㕔����̗]��

	normalFont = NULL;
	boldFont = NULL;
	qFont = NULL;
	qBoldFont = NULL;
	oldFont = NULL;
	parsedRecord = NULL;
#ifndef WINCE
	m_isAnime = false;
#endif

	// �ėp�y���̍쐬
	underLinePen.CreatePen(PS_SOLID,1,solidBlack);
	DarkBlueunderLinePen.CreatePen(PS_SOLID,1,solidDarkBlue);

	// �ėp�u���V�̍쐬
	blueBrush.CreateSolidBrush(solidBlue);

	// �p���X�N���[���p
	m_offsetPixelX = 0;				// ���X�N���[���\���I�t�Z�b�g�l
	m_dPxelX = 0;					// ���X�N���[������
	m_bPanDragging = false;			// ���h���b�O���t���O
	m_bScrollDragging = false;		// �c�h���b�O���t���O
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




// Ran2View ���b�Z�[�W �n���h��

BOOL Ran2View::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("\r\n��Create�N�����̎c������:%d KB\r\n"),memState.dwAvailPhys/1024);
#endif

	CPaintDC	cdc(pParentWnd);	// �_�C�A���O�̃N���C�A���g�̈���x�[�X�Ƃ���B

	screenWidth = rect.right - rect.left;
	screenHeight = rect.bottom - rect.top;

	backDC = new CDC();
	backDC->CreateCompatibleDC(&cdc);
	backDC->SetBkMode(OPAQUE);	// ���߃��[�h�ɐݒ�

	memBMP = new CBitmap();
	// ��ʂ̍�����n�{���ė]�T���������Ă݂�
	if( memBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight*2) != TRUE ){
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return(FALSE);
	}
	m_drawStartTopOffset = screenHeight/2;

	memDC = new CDC();
	memDC->CreateCompatibleDC(&cdc);
	memDC->SetBkMode(OPAQUE);	// ���߃��[�h�ɐݒ�
	oldBMP = memDC->SelectObject(memBMP);

	// �p���X�N���[���p�o�b�t�@�̊m��
	memBackBMP = new CBitmap();
	// ��ʂ̑傫���������m�ۂ���
	if( memBackBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight) != TRUE ){
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return(FALSE);
	}

	memBackDC = new CDC();
	memBackDC->CreateCompatibleDC(&cdc);
	memBackDC->SetBkMode(OPAQUE);	// ���߃��[�h�ɐݒ�
	oldBackBMP = memBackDC->SelectObject(memBackBMP);

#ifndef WINCE
	m_graphics = new Graphics(memDC->m_hDC);
#endif

	CRect r = rect;
	r.right = 100;
	r.bottom = 100;

	// �����N�\���p
	activeLinkID = -1;			// �|�C���g���̃����N�A��
	activeimgLinkID = -1;		// �|�C���g���̉摜�����N�A��
	activemovLinkID = -1;		// �|�C���g���̓��惊���N�A��

	// �p���X�N���[�����t���O
	bAutoScrolling = false;

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, r, pParentWnd, nID, pContext);
}

void Ran2View::OnSize(UINT nType, int cx, int cy)
{
	screenWidth = cx;
	screenHeight = cy;

	if (charHeight + charHeightOffset > 0) {
		viewLineMax = (screenHeight / (charHeight + charHeightOffset));	// 0�I���W���Ȃ̂Œ��ӁI
	}

	// �o�b�N�o�b�t�@�̃T�C�Y���������ꍇ�͍Đ���
	BITMAP bmp;
	GetObject(memBMP->m_hObject, sizeof(BITMAP), &bmp);
	if (bmp.bmWidth < screenWidth ||
		bmp.bmHeight < screenHeight*2) 
	{
		// ���
#ifndef WINCE
		if( m_graphics != NULL ){
			delete m_graphics;
		}
#endif
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

		// �p���X�N���[���p�o�b�t�@�̉��
		if( memBackDC != NULL ){
			memBackDC->DeleteDC();
		}

		if( memBackBMP != NULL ){
			memBackBMP->DeleteObject();
			delete memBackBMP;
			memBackBMP = new CBitmap();
		}

		// �o�b�N�o�b�t�@����
		CPaintDC	cdc(GetParent());	// �_�C�A���O�̃N���C�A���g�̈���x�[�X�Ƃ���B
		if( memBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight*2) != TRUE ){
			MessageBox(TEXT("CreateCompatibelBitmap error!"));
			return;
		}
		m_drawStartTopOffset = screenHeight/2;

		memDC->CreateCompatibleDC(&cdc);
		memDC->SetBkMode(OPAQUE);	// ���߃��[�h�ɐݒ�
		oldBMP = memDC->SelectObject(memBMP);

		// �p���X�N���[���p�o�b�t�@�̊m��
		if( memBackBMP->CreateCompatibleBitmap(&cdc,screenWidth,screenHeight) != TRUE ){
			MessageBox(TEXT("CreateCompatibelBitmap error!"));
			return;
		}

		memBackDC->CreateCompatibleDC(&cdc);
		memBackDC->SetBkMode(OPAQUE);	// ���߃��[�h�ɐݒ�
		oldBackBMP = memBackDC->SelectObject(memBackBMP);

#ifndef WINCE
		m_graphics = new Graphics(memDC->m_hDC);
#endif
	}

	CWnd::OnSize(nType, cx, cy);
}

BOOL Ran2View::DestroyWindow()
{
#ifndef WINCE 
	m_isAnime = false;
	KillTimer(TIMERID_ANIMEGIF);

	if( m_graphics != NULL ){
		//m_graphics->ReleaseHDC(memDC->m_hDC);
		delete m_graphics;
	}
#endif

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

	// �p���X�N���[���p�o�b�t�@�̉��
	if( memBackDC != NULL ){
		memBackDC->DeleteDC();
		delete memBackDC;
	}

	if( memBackBMP != NULL ){
		memBackBMP->DeleteObject();
		delete memBackBMP;
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



// �\���J�����̕ύX
// �����F	newHeight	�V�t�H���g�̕�����
// �߂�l�F	�ύX��t�H���g�̕�����
int	Ran2View::ChangeViewFont(int newHeight, LPCTSTR szFontFace)
{
	// ���Ƀt�H���g���I���ς݂Ȃ�߂��B
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
	// 1/4�t�H���g�̕������ƍ������K��
	oldFont = memDC->SelectObject(qFont);
	CSize	charQSize = memDC->GetTextExtent(CString(TEXT("W"))); 
	charQHeight = charQSize.cy;
	charQSize = memDC->GetTextExtent(CString(TEXT("��"))); 
	charQWidth = charQSize.cx;

	// Bold�̕����擾����
	oldFont = memDC->SelectObject(boldFont);

	// �u���v���K��̕����Ƃ��ĕ������ƍ������K��
	CSize	charSize = memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = memDC->GetTextExtent(CString(TEXT("��"))); 
	boldCharWidth = charSize.cx;

	// �ʏ�T�C�Y�̃t�H���g��ݒ�
	oldFont = memDC->SelectObject(normalFont);
	// �u���v���K��̕����Ƃ��ĕ������ƍ������K��
	charSize = memDC->GetTextExtent(CString(TEXT("W"))); 
	charHeight = charSize.cy;
	charSize = memDC->GetTextExtent(CString(TEXT("��"))); 
	charWidth = charSize.cx;
	currentCharWidth = charWidth;

	// �X�y�[�V���O���̎擾
	ABC	abcInfo;
	memDC->GetCharABCWidths(TEXT('��'),TEXT('��'),&abcInfo);
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


void Ran2View::OnPaint()
{
	CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

	DrawToScreen(&dc);
}

// �p���X�N���[���J�n
// �X�N���[�������ibForword�j�ɏ]���I�t�Z�b�g�l�ƍ�����ݒ肵�ă^�C�}�[���N������
void Ran2View::StartPanDraw( bool bForword)
{
	if( bForword ) {
		// �E�����փX�N���[��
		KillTimer( TIMERID_PANSCROLL );
		// ���ֈ��ʂ��ꂽ�Ƃ��납��J�n
		m_offsetPixelX = - screenWidth;
		// WM�̏ꍇ�͍������ӂ₵�Ĉړ��X�e�b�v�����炷
#ifndef WINCE
		m_dPxelX = screenWidth / 15 ;
#else
		m_dPxelX = screenWidth / 5 ;
#endif
		// �p���X�N���[�����t���O�ݒ�
		bAutoScrolling = true;
		// �p���X�N���[���J�n
		SetTimer( TIMERID_PANSCROLL, 10L, NULL );
	} else {
		// �������փX�N���[��
		KillTimer( TIMERID_PANSCROLL );
		// �E�ֈ��ʂ��ꂽ�Ƃ��납��J�n
		m_offsetPixelX = screenWidth;
		// WM�̏ꍇ�͍������ӂ₵�Ĉړ��X�e�b�v�����炷
#ifndef WINCE
		m_dPxelX = -screenWidth / 15 ;
#else
		m_dPxelX = -screenWidth / 5 ;
#endif
		// �p���X�N���[�����t���O�ݒ�
		bAutoScrolling = true;
		// �p���X�N���[���J�n
		SetTimer( TIMERID_PANSCROLL, 10L, NULL );
	}
}

void Ran2View::DrawToScreen(CDC* pDC)
{
	int y = -m_offsetPixelY;
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
		pDC->BitBlt( 0 , 0 , m_offsetPixelX , screenHeight , memBackDC , wid , 0 , SRCCOPY );

		// �ړ����킩��悤�ɏc��������
		pDC->MoveTo( m_offsetPixelX - 1 , 0);
		pDC->LineTo( m_offsetPixelX - 1 , screenHeight );
	} else if( m_offsetPixelX < 0 ) {
		// ���ɂ���Ă���ꍇ
		sx = -m_offsetPixelX;
		dx = 0;
		wid = screenWidth + m_offsetPixelX;

		// �ύX�O��ʂ��E���ɕ\������
		pDC->BitBlt( wid , 0 , sx , screenHeight , memBackDC , 0 , 0 , SRCCOPY );

		// �ړ����킩��悤�ɏc��������
		pDC->MoveTo( wid + 1 , 0);
		pDC->LineTo( wid + 1 , screenHeight );

	}

	// �I�t�Z�b�g�����ʕ�����Ε\���s�v
	if( abs( m_offsetPixelX ) > abs( screenWidth ) ){
		return;
	}

	// �ύX���ʂ��I�t�Z�b�g�ɍ��킹�ĕ\������
	TRACE( L"m_drawStartTopOffset,m_offsetPixelY : %5d %5d\n", m_drawStartTopOffset,m_offsetPixelY );
	pDC->BitBlt( dx, 0, wid, screenHeight, memDC, sx, m_drawStartTopOffset +y, SRCCOPY );
//	pDC->Rectangle(0, 0, screenWidth, screenHeight);
}


// �\������t�܂ŉ��������邩���Čv�Z��������B
// ����	srcStr	... �Ώۂ̕�����
//		width	... �\����
// �߂�l ... �\�������Ɏ��܂钷���̕�����
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


void Ran2View::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}



LRESULT Ran2View::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WindowProc(message, wParam, lParam);
}



// �s���̐ݒ�
// �߂�l�F(EOL:[EndOfLine]���^�O�ōs���I�� follow:�����p�����Ď��s forceBR:�����p�����ĉ��s�j
// BOL:(BeginOfLine)�J�n�^�O�ŐV�s���K�v
// EOL:(EndOfLine)���^�O�ōs���I��
// follow:�����p�����Ď��s

// �p�����[�^�F
// hashRecord: �U�蕪����^�O���
// rowRecord: �U�蕪���Ώې�
// bridgeInfo: �s���L��(p,h1,h2,h2,h3�̏I��)�Ń��Z�b�g����镶���C�����
// bigBridgeInfo: end�L��(end_kakomi_hoge)�Ń��Z�b�g�����s�C�����(���܂̂Ƃ���g���̂�)
ProcessStateEnum Ran2View::SetRowProperty(HtmlRecord* hashRecord,RowProperty* rowRecord,BridgeProperty* bridgeInfo,BigBridgeProperty* bigBridgeInfo)
{
	static int imageDummyLineCount = 0; // �摜�̋�s�o�͌v���p
	ProcessStateEnum processState = ProcessState_through; 
	CString logStr;
	bool isBreak = false;		// �s�P�ʂ̏������I���x�Ƀ��R�[�h�������o���t���O

	// �^�O���̐U�蕪��
	const enum tagStatus { 
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
		Tag_end_sub, Tag_end_sup, Tag_end_b, Tag_end_blockquote,
		Tag_end_link, Tag_end_img, Tag_end_mov,
		Tag_end_h1, Tag_end_h2, Tag_end_h3, 
		Tag_end_kakomi_blue, Tag_end_kakomi_gray, Tag_end_kakomi_gray2, 
		Tag_end_kakomi_white, Tag_end_kakomi_white2, 
	};
	const wchar_t* tagText[] = { 
	// �J�n�^�O(27��)
		TEXT("dummy"), 
		TEXT("h1"), TEXT("h2"), TEXT("h3"), 
		TEXT("p"), TEXT("blue"), TEXT("underline"), 
		TEXT("sub"), TEXT("sup"), TEXT("text"), 
		TEXT("entity"), TEXT("gaiji"), TEXT("img"), TEXT("mov"),
		TEXT("br"), TEXT("anchor"), TEXT("link"), 
		TEXT("level0"), TEXT("level1"),	TEXT("level2"), 
		TEXT("kakomi_blue"), TEXT("kakomi_gray"), TEXT("kakomi_gray2"), 
		TEXT("kakomi_white"), TEXT("kakomi_white2"),
		TEXT("b"), TEXT("blockquote"),
	// �I���^�O(18��)
		TEXT("end_p"), TEXT("end_blue"), TEXT("end_underline"),
		TEXT("end_sub"), TEXT("end_sup"), TEXT("end_b"), TEXT("end_blockquote"),
		TEXT("end_link"), TEXT("end_img"), TEXT("end_mov"),
		TEXT("end_h1"), TEXT("end_h2"), TEXT("end_h3"),	
		TEXT("end_kakomi_blue"), TEXT("end_kakomi_gray"), TEXT("end_kakomi_gray2"), 
		TEXT("end_kakomi_white"), TEXT("end_kakomi_white2"), 
		NULL,
	};

	tagStatus currentTag = (tagStatus)0;	// �U�蕪���ł��Ȃ��^�O�̓G���[�����Ƃ���

	// �^�O�̈ꎞ�U�蕪��
	for(int i=0 ;tagText[i]!=NULL; i++){
		if( hashRecord->key.Compare(tagText[i]) == 0 ){
			currentTag = (tagStatus)i;

			// �J�n�^�O
			if( currentTag == Tag_p ||
				currentTag == Tag_h1 ||
				currentTag == Tag_h2 ||
				currentTag == Tag_h3 ||
				currentTag == Tag_level0 ||
				currentTag == Tag_level1 ||
				currentTag == Tag_level2 ||
				currentTag == Tag_blockquote )
			{
				processState = ProcessState_BOL;
			}			

			// �I���^�O
			if( currentTag == Tag_end_p ||
				currentTag == Tag_end_h1 || 
				currentTag == Tag_end_h2 ||
				currentTag == Tag_end_h3)
			{
				processState = ProcessState_EOL;
			}

			// ���s�Ƃ�����������
			if( currentTag == Tag_br ){
				processState = ProcessState_FBL;
			}

			// �G���[�^�O�̏ꍇ�͂����ŃX�L�b�v
			if( currentTag == Tag_nothing ){
				return(ProcessState_error);
			}
		}
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

	// �݂͂̐ݒ�([blockquote]:���N���Ȃ��A�w�i���s���N)
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
	}else if( currentTag == Tag_end_b ){
		bridgeInfo->isBold = false;

	// �����F�̐ݒ�I��
	}else if( currentTag == Tag_end_blue ){
		bridgeInfo->foregroundColor = solidBlack;

	// �����̏I��
	}else if( currentTag == Tag_end_underline ){
		bridgeInfo->isUnderLine = false;

	// ������̐ݒ�
	}else if( currentTag == Tag_text || currentTag == Tag_entity ||  bigBridgeInfo->remainStr.GetLength() > 0){
//		processState = ProcessState_FBL;	// ��{�͈�s�P�ʂŉ��s����
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

				oldFont = memDC->SelectObject(qBoldFont);
				currentCharWidth = charQWidth;
				newText->fontType = FontType_quarter;
			}else{
				oldFont = memDC->SelectObject(boldFont);
				currentCharWidth = boldCharWidth;
			}
		}else{
			if( bridgeInfo->isUpHanging == true || bridgeInfo->isDownHanging == true ){
				// �t�H���g�C�����̓o�^
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

		// ��s�Ɏ��܂�Ȃ��ꍇ�͎����z��
		if( cutStr.GetLength() < srcStr.GetLength() ){
			bigBridgeInfo->remainStr = srcStr.Mid(cutStr.GetLength());

			if( bigBridgeInfo->remainStr.GetLength() > 0 ){
				// �����z�����ꍇ�͎����z��������������܂ŌJ��Ԃ�
				processState = ProcessState_FOL;
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
		int blockWidth = memDC->GetTextExtent(cutStr).cx;
		bigBridgeInfo->remainWidth -= blockWidth;

		// ���������v�Z�����̂Ńt�H���g��߂�
		memDC->SelectObject(oldFont);

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
			processState = ProcessState_FOL;
		}else{
			newGaiji = new GaijiProperty();
			// [m:xx] ���� xx �𒊏o���A���\�[�XID�Ƃ���
			newGaiji->resourceID = hashRecord->value.Mid(3,hashRecord->value.GetLength()-4);	// ���\�[�X���̒u�����s���ꍇ�͂����ł��������āI

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

		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
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

		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
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

		bridgeInfo->pageAnchor = _wtol(hashRecord->parameter);
	// ���惊���N�^�O�̏I��
	}else if( currentTag == Tag_end_mov ){
		bridgeInfo->isLink = false;
		bridgeInfo->linkType = LinkType_noLink;
		bridgeInfo->jumpID = 0;
		bridgeInfo->pageAnchor = 0;
		bigBridgeInfo->movlinkID++;		// ���惊���N�A�Ԃ��C���N�������g
	}else{
		// ����ȊO�̃G���[�̏ꍇ
		//logStr.Format(TEXT("Through tag!!(%s:%s)\r\n"),hashRecord->key,hashRecord->value);
		//OutputDebugString(logStr);
	}

	return(processState);
}



// �V�s���̒ǉ�
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


// ��ׂ����̃��Z�b�g
void Ran2View::ResetBigBridgeProperty(BigBridgeProperty* bigBridgeInfo,BridgeProperty* bridgeInfo,ProcessStateEnum mode,int width)
{
	// �o�͉\���ƊJ�n�ʒu�������Z�b�g
	if( width != 0 ){
		bigBridgeInfo->screenWidth = width;		// ��ʂ̃N���C�A���g�̈��ݒ�
	}
	bigBridgeInfo->remainWidth = (screenWidth - NormalWidthOffset - (leftOffset*2)) - ((leftOffset+framePixel)*3*(bigBridgeInfo->frameNestLevel+1));
	bigBridgeInfo->startWidth = leftOffset;

	// BOL�����擪�̓˂��o�����I�t�Z�b�g����̂Œ��ӁI
	if( mode == ProcessState_BOL ){
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


// ���N��top/bottom�I�����肩��
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


// DAT�t�@�C������̃f�[�^�\�z
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
	//memBackDC->FillSolidRect( 0 , 0 , screenWidth , screenHeight , RGB(128,128,128) );
	memBackDC->BitBlt( 0 , 0 , screenWidth , screenHeight , memDC , 0 , m_drawStartTopOffset , SRCCOPY );

	if( bodyArray != NULL ){
		PurgeMainRecord();
		parsedRecord = ParseDatData2(bodyArray,this->screenWidth); // dat�̕ϊ�����
		bodyArray->FreeExtra();
		bodyArray->RemoveAll();
//		delete bodyArray;
	}

	// �`��ɕK�v�ȍs����Ԃ�
	if( parsedRecord != NULL ){
		rc = parsedRecord->rowInfo->GetSize() - viewLineMax; 
	}
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));


#ifdef DEBUG
	::GlobalMemoryStatus(&memState);
	TRACE(TEXT("��LoadDetail�I�����̎c������:%d Bytes\r\n"),memState.dwAvailPhys);
#endif

#ifndef WINCE
	// GIF�A�j���͊����X�N���[����1/3���炢�̕p�x�̍X�V�ł����H
	SetTimer( TIMERID_ANIMEGIF, 100L, NULL );
#endif
	return(rc);
}


// �`��̓����
int	Ran2View::DrawDetail(int startLine, bool bForceDraw)
{
#ifndef WINCE
//	for(int cacheIndex=0 ; cacheIndex < ran2ImageArray.GetSize() ; cacheIndex++){
//		Ran2Image* image = (Ran2Image*)ran2ImageArray.GetAt(cacheIndex);
//		delete image;
//	}
//	ran2ImageArray.RemoveAll();
#endif

#ifdef DEBUG
	wprintf( L"DrawDetail: %5d\n" , startLine);
#endif

	// �`��J�n���C����ۑ�����
	int istartLine = startLine;

	if (istartLine<0) {
		// �擪����яo���Ă����0�Ɉ����߂�
		istartLine = 0;
	}
	// �ǂ̍s����`�悵������ۑ����Ă���
	drawOffsetLine = istartLine;

	// ���R�[�h�̓W�J�~�X��͈͊O�̎w��͒e��
	if( parsedRecord == NULL || istartLine > parsedRecord->rowInfo->GetSize() ){
		return(0);
	}

	//// �p���X�N���[���p�ɒ��O�̕\����Ԃ̃R�s�[�����i���ʕ��j
	////memBackDC->FillSolidRect( 0 , 0 , screenWidth , screenHeight , RGB(128,128,128) );
	//memBackDC->BitBlt( 0 , 0 , screenWidth , screenHeight , memDC , 0 , m_drawStartTopOffset , SRCCOPY );

	// �h��Ԃ�
//	memDC->PatBlt( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, WHITENESS );
//	memDC->FillSolidRect( 0, 0, screenWidth, screenHeight+(charHeight+charHeightOffset)*N_OVER_OFFSET_LINES, RGB(255,255,255) );
	BITMAP bmp;
	GetObject(memBMP->m_hObject, sizeof(BITMAP), &bmp);
	memDC->FillSolidRect( 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255,255,255) );

//	TRACE( L"[DrawRect]\n" );
//	TRACE( L" %dx%d\n", bmp.bmWidth, bmp.bmHeight );
//	TRACE( L" %dx%d\n", screenWidth, screenHeight );

	// �I�t�Z�b�g�X�N���[���p��N�s�]���ɕ`�悷��B
	const int N_OVER_OFFSET_LINES = 2;
	for(int i=-N_OVER_OFFSET_LINES; i<=viewLineMax+N_OVER_OFFSET_LINES ; i++){
		int targetLine = istartLine + i;

		if (targetLine < 0) {
			continue;
		}

		// �͈͂��z������X���[
		if( parsedRecord->rowInfo->GetSize() <= targetLine ){
			break;
		}

		RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(targetLine);

		if( row != NULL ){
			TRACE(TEXT("%d�̕`����J�n���܂�\r\n"), targetLine);
			// �t���[���̕`��B�摜���ݒ肳��Ă�����`�悵�Ȃ�

			if( row->imageProperty.imageNumber == -1 ){
				this->DrawFrameProperty(i,row);
			}

			// �e�L�X�g�v�f�̏o��(�����A�����N�����A�Z���n���܂�)
			this->DrawTextProperty(i,row->textProperties);

			// �O���v�f�̏o��
			this->DrawGaijiProperty(i,row->gaijiProperties);
		}

		TRACE( L" line : %d\n", targetLine );
	}
//	memDC->Rectangle( 0, 0, 10, 10 );

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


// �s���̃e�L�X�g�v���p�e�B�̔z���`�悷��
// line: �`�悵�����s
// rowProperty: �s�̏��
void Ran2View::DrawFrameProperty(int line,RowProperty* rowProperty)
{
	for(int i=0 ;i<FrameNestLevel ; i++){
		// ���N�̕`�悪�ݒ肳��Ă��鎞�̂ݕ`��
		if(	rowProperty->frameProperty[i].frameType != FrameType_nothing ){
			CRect drawRect;
			// bottom�̎��ȊO�Ɍ����Ĕw�i��`��
			if( !(( rowProperty->frameProperty[i].frameType == FrameType_stool ) &&
				( rowProperty->textProperties->GetCount() == 0)) ){ 
				// �w�i�F�̕`��
				CBrush backBrush(rowProperty->frameProperty[i].backgroundColor);
				int sx = leftOffset + (i * frameOffset);
				int ex = (screenWidth - NormalWidthOffset) - leftOffset - (i * frameOffset);
				int sy = m_drawStartTopOffset + topOffset + (line*(charHeight+charHeightOffset));
				int ey = sy + (charHeight+charHeightOffset);
				drawRect = CRect(sx,sy,ex,ey);
				memDC->FillRect(drawRect,&backBrush);
			}

			CPen framePen(PS_SOLID,1,rowProperty->frameProperty[i].penColor);
			// ��[�ƍ��E��`��
			if( rowProperty->frameProperty[i].frameType == FrameType_roof ){

				CPen* oldPen = memDC->SelectObject(&framePen);
				memDC->MoveTo(drawRect.left,drawRect.top);
				memDC->LineTo(drawRect.right,drawRect.top);

				memDC->MoveTo(drawRect.left,drawRect.top);
				memDC->LineTo(drawRect.left,drawRect.bottom);

				memDC->MoveTo(drawRect.right,drawRect.top);
				memDC->LineTo(drawRect.right,drawRect.bottom);

				memDC->SelectObject(oldPen);
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

				CPen* oldPen = memDC->SelectObject(&framePen);
				if( rowProperty->textProperties->GetCount() != 0 ){
					// �I�s�Ƀe�L�X�g������ꍇ�͍��E�̘g����`�悷��
					memDC->MoveTo(drawRect.left,drawRect.top);
					memDC->LineTo(drawRect.left,drawRect.bottom);

					memDC->MoveTo(drawRect.right,drawRect.top);
					memDC->LineTo(drawRect.right,drawRect.bottom);
				}
				memDC->MoveTo(drawRect.left,drawRect.bottom-framePixel);
				memDC->LineTo(drawRect.right,drawRect.bottom-framePixel);
				memDC->SelectObject(oldPen);

			// ���E�̘g�������`��
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



// �s���̃e�L�X�g�v���p�e�B�̔z���`�悷��
// line: �`�悵�����s
// textProperties: textProperty���܂Ƃ߂��z��
void Ran2View::DrawTextProperty(int line,CPtrArray* textProperties)
{
	CString	logStr;
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
		if( ((activeLinkID >=0) && (activeLinkID == text->linkID) ) ||
			((activeimgLinkID >=0) && (activeimgLinkID == text->imglinkID) ) ||
			((activemovLinkID >=0) && (activemovLinkID == text->movlinkID) ) ){
				// �|�C���g���Ă��郊���N�A�Ԃ��e�L�X�g���̂��̂ƈ�v���Ă���Δ��]�t���O��ݒ�
				bReverse = true;
		}

		CRect drawRect = CRect(text->drawRect.left,sy,text->drawRect.right,ey);
		if( bReverse ){
			// ���]�\���p�ɍ����h��Ԃ�
			memDC->FillSolidRect( &drawRect , solidBlack );
		}

		// �A���_�[���C���̕`��
		if( text->isUnderLine == true ){ 
			CPen* oldPen = NULL;
			if( text->foregroundColor == solidDarkBlue ){
				oldPen = memDC->SelectObject(&DarkBlueunderLinePen);
			} else {
				oldPen = memDC->SelectObject(&underLinePen);
			}
			memDC->MoveTo(drawRect.left, drawRect.bottom-charHeightOffset);
			memDC->LineTo(drawRect.right, drawRect.bottom-charHeightOffset);
			oldPen = memDC->SelectObject(oldPen);
		}

		// �����F�ƃt�H���g�̐؂�ւ�
		if( bReverse ) {
			// ���]�\���p �����F�����A�w�i�F����
			memDC->SetTextColor(solidWhite);
			memDC->SetBkColor(solidBlack);
		} else {
			// �m�[�}���\���p �e�L�X�g��񂩂�F���擾
			memDC->SetTextColor(text->foregroundColor);
			memDC->SetBkColor(text->backgroundColor);
		}

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


// �s���̃e�L�X�g�v���p�e�B�̔z���`�悷��
// line: �`�悵�����s
// textProperties: textProperty���܂Ƃ߂��z��
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
				m_pImageList->Draw( memDC, imageIdx, CPoint(gaiji->drawRect.left, sy), ILD_TRANSPARENT );
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


// dat�t�@�C��������s���N���X�ւ̕ϊ�����2(Unicode�ɕϊ�����Ă��鎖���O��)
// �t�@�C����CArchive�ň�s�Âǂނ̂ł͂Ȃ��A�ꊇ�œǂݍ����CStringArray�֕������Ă��珈�����s��
MainInfo* Ran2View::ParseDatData2(CStringArray* datArray,int width)
{
	CString logStr;	// �G���[�o�͗p

	MainInfo* newMainRecord = NULL;
	newMainRecord = new MainInfo();

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
	this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BOL,width);

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
				hashRecord->key = TEXT("text");
			} else {
				// �O��
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
		}else if( lineStr.Compare(TEXT("[blockquote]")) == 0 ) {
			// ���p�J�n
			hashRecord->key = TEXT("blockquote");
		}else if( lineStr.Compare(TEXT("[/blockquote]")) == 0 ) {
			// ���p�I��
			hashRecord->key = TEXT("end_blockquote");
		}else if( lineStr.Compare(TEXT("[a]")) == 0 ) {
			// �����N�J�n
			hashRecord->key = TEXT("link");
		}else if( lineStr.Compare(TEXT("[/a]")) == 0 ) {
			// �����N�I��
			hashRecord->key = TEXT("end_link");
		}else if( lineStr.Compare(TEXT("[img]")) == 0 ) {
			// �摜�����N�J�n
			hashRecord->key = TEXT("img");
		}else if( lineStr.Compare(TEXT("[/img]")) == 0 ) {
			// �摜�����N�I��
			hashRecord->key = TEXT("end_img");
		}else if( lineStr.Compare(TEXT("[mov]")) == 0 ) {
			// ���惊���N�J�n
			hashRecord->key = TEXT("mov");
		}else if( lineStr.Compare(TEXT("[/mov]")) == 0 ) {
			// ���惊���N�I��
			hashRecord->key = TEXT("end_mov");
		}else{
			hashRecord->key = TEXT("text");
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
		ProcessStateEnum rc = this->SetRowProperty(hashRecord,currentRowRecord,&bridgeInfo,&bigBridgeInfo);

		// �y�[�W���A���J�[�̍s�ʒu���L���b�V������
		if( bridgeInfo.inPageAnchor != -1 ){
			newMainRecord->anchorIndex[bridgeInfo.inPageAnchor] = rowPropertyArray->GetSize() - 1;
		}

		// �J�n�^�O�̓C���f���g�ʒu�̍Đݒ肾��
		if( rc == ProcessState_BOL ){

			// ��܂����������Z�b�g
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo,ProcessState_BOL);
		// �����z�����̌J��Ԃ������̏ꍇ 
		}else if( rc == ProcessState_FOL ){
			// �����z������Ȃ��Ȃ�܂ŌJ��Ԃ�
			while( rc == ProcessState_FOL ){
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
				rc = this->SetRowProperty(hashRecord,currentRowRecord,&bridgeInfo,&bigBridgeInfo);
			}

			// �V�s���̒ǉ�
//			this->AddNewRowProperty(rowPropertyArray);
			// ��܂����������Z�b�g
//			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ���N��top/bottom�̏�Ԃ������ւ���
			this->ChangeFrameProperty(&bigBridgeInfo);

			//TRACE(TEXT(" �J�z�I��[%d]\r\n"),rowPropertyArray->GetSize());
		// �������s�͐V�K���R�[�h��New���ĒǋL 
		}else if( rc == ProcessState_FBL ){
			// �V�s���̒ǉ�
			this->AddNewRowProperty(rowPropertyArray);
			// ��܂����������Z�b�g
			this->ResetBigBridgeProperty(&bigBridgeInfo,&bridgeInfo);
			// ���N��top/bottom�̏�Ԃ������ւ���
			this->ChangeFrameProperty(&bigBridgeInfo);

		// �I���^�O�͐V�������R�[�h��New���ĒǋL��ɍs�܂����������Z�b�g
		}else if( rc == ProcessState_EOL ){
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



// mainRecord�̔j��
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


// DPI�l�����W�X�g������擾
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


void Ran2View::OnLButtonDblClk(UINT nFlags, CPoint point)
{

	CWnd::OnLButtonDblClk(nFlags, point);
}


void Ran2View::ResetDragOffset(void)
{
	KillTimer(TIMERID_AUTOSCROLL);
	KillTimer(TIMERID_PANSCROLL);	// �p���X�N���[����~

	bAutoScrolling = false;			// �p���X�N���[�����t���O�N���A

	m_offsetPixelY = 0;
	m_offsetPixelX = 0;				// �p���X�N���[���I�t�Z�b�g������
}


void Ran2View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// �e�̌Ăяo��
	::SendMessage( GetParent()->GetSafeHwnd(), WM_RBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );

	CWnd::OnRButtonUp(nFlags, point);
}


// �^�b�v�ɂ�郊���N�ʒu�̒T��
void Ran2View::OnLButtonUp(UINT nFlags, CPoint point)
{
	CString logStr;
	int dx = m_ptDragStart.x - point.x;
	int dy = m_ptDragStart.y - point.y;

	if (m_bDragging) {
		//m_bDragging = false;		// �h���b�O���t���O�͌�̏����Ŕ���Ɏg��
		//::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
		ReleaseCapture();

		// �c�h���b�O�J�n���f�p�I�t�Z�b�g�l
#ifndef WINCE
			// win32�̏ꍇ���s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
		int ddy = ( charHeight+charHeightOffset ) / 2 + 1 ;
#else
			// WM�̏ꍇ��s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
		int ddy = charHeight+charHeightOffset;
#endif
		if( m_bPanDragging ) {
			// ���h���b�O��
			//if( abs( dx ) < screenWidth / 3 ){
			//	// �}�E�X�����ɖ߂����牡�h���b�O���L�����Z���@�����������₵���̂ŕۗ�
			//	m_bPanDragging = false ;
			//}
		} else if( m_bScrollDragging ) { 
			// �c�h���b�O��

		} else {
			// �h���b�O�������m�肵�Ă��Ȃ�����
			if( abs(dx) > abs(dy) && abs( dx ) > screenWidth / 3) {
				// �������̈ړ��ʂ��傫���Ĉړ��ʂ���ʂ�1/3�ȏ�̏ꍇ
				// ���h���b�O
				m_bPanDragging = true;
			} else if(  abs(dx) < abs(dy) && abs( dy ) > ddy ) {
				// �c�����̈ړ��ʂ��傫���Ĉړ��ʂ��h���b�O�J�n�I�t�Z�b�g�ȏ�̏ꍇ
				if( GetAllLineCount()-GetViewLineMax() > 0 ) {
					// �c�X�N���[���\�Ȃ��
					// �c�h���b�O
					m_bScrollDragging = true;
				}
			}
		}
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
		activeLinkID = -1;
		activeimgLinkID = -1;
		activemovLinkID = -1;

		// �_�u���N���b�N�Ƃ݂Ȃ�
		// �e�̌Ăяo��
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	} else {
		// �h���b�O����
#ifndef WINCE
		m_dwFirstLButtonUp = GetTickCount();
#else
		// WM�̏ꍇ�_�u���N���b�N���肵�Ȃ�
		m_dwFirstLButtonUp = 0;
#endif
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
				CBrush * oldbrs = memDC->SelectObject( &brs );
				memDC->RoundRect( screenWidth  / 8 , m_drawStartTopOffset + screenHeight / 4 , screenWidth * 7 / 8 , m_drawStartTopOffset + screenHeight * 3 / 4 , 10 , 10  );
				memDC->SelectObject( &oldbrs );
				brs.DeleteObject();
				brs.CreateSolidBrush( RGB( 32 , 32 , 32 ) );
				oldbrs = memDC->SelectObject( &brs );
				CPoint pts[3];
				pts[0].x = screenWidth * 5 / 8;
				pts[0].y = m_drawStartTopOffset + screenHeight * 3 / 8;
				pts[1].x = screenWidth * 5 / 8;
				pts[1].y = m_drawStartTopOffset + screenHeight * 5 / 8;
				pts[2].x = screenWidth * 3 / 8;
				pts[2].y = m_drawStartTopOffset + screenHeight / 2;
				memDC->Polygon( pts , 3 );
				memDC->SelectObject( &oldbrs );
				brs.DeleteObject();

				OnPaint();
				// ���̃R�����g��\��
				::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM( 10 , 1000 ));
			} else {
				// �E����
				CBrush brs;
				brs.CreateSolidBrush( lightGray );
				CBrush * oldbrs = memDC->SelectObject( &brs );
				memDC->RoundRect( screenWidth  / 8 , m_drawStartTopOffset + screenHeight / 4 , screenWidth * 7 / 8 , m_drawStartTopOffset + screenHeight * 3 / 4 , 10 , 10  );
				memDC->SelectObject( &oldbrs );
				brs.DeleteObject();
				brs.CreateSolidBrush( RGB( 32 , 32 , 32 ) );
				oldbrs = memDC->SelectObject( &brs );
				CPoint pts[3];
				pts[0].x = screenWidth * 3 / 8;
				pts[0].y = m_drawStartTopOffset + screenHeight * 3 / 8;
				pts[1].x = screenWidth * 3 / 8;
				pts[1].y = m_drawStartTopOffset + screenHeight * 5 / 8;
				pts[2].x = screenWidth * 5 / 7;
				pts[2].y = m_drawStartTopOffset + screenHeight / 2;
				memDC->Polygon( pts , 3 );
				memDC->SelectObject( &oldbrs );
				brs.DeleteObject();

				OnPaint();
				// �O�̃R�����g��\��
				::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONDBLCLK, (WPARAM)nFlags, (LPARAM)MAKELPARAM( 10 , 0 ));
			}
		} else if( m_bScrollDragging ) {
			// �c�����Ƀh���b�O
			m_autoScrollInfo.push( GetTickCount(), point );
			double speed = m_autoScrollInfo.calcMouseMoveSpeedY();
			TRACE( L"! speed   : %5.3f [px/msec]\n", speed );

			KillTimer( TIMERID_AUTOSCROLL );
			bAutoScrolling = false;
			if( GetAllLineCount()-GetViewLineMax() > 0 ) {
				// �X�N���[���\�Ȃ�΁i�Ɣ��肵�Ȃ��ƃX�N���[���s�\��ԂŃ^�C�}�[����������j
				// �����X�N���[���J�n
				m_dwAutoScrollStartTick = GetTickCount();
				m_yAutoScrollMax = 0;
				bAutoScrolling = true;
				SetTimer( TIMERID_AUTOSCROLL, 20L, NULL );
			} else {
				// �����N�����N���A���čĕ\������
				activeLinkID = -1;
				activeimgLinkID = -1;
				activemovLinkID = -1;
				DrawDetail( drawOffsetLine , true );
			}
		} else {
			// ���N���b�N�����̏������s��

			// �^�b�v�ʒu�̍s�ԍ����擾
			int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

			// Row�z�񂩂�̎擾�ʒu���Z�o
			int rowNumber = drawOffsetLine + tapLine;

			// �^�b�v�ʒu���͈͓����z����ꍇ�͉������Ȃ�
			if( parsedRecord->rowInfo->GetSize() > rowNumber  && rowNumber >= 0 ){
				RowProperty* row = (RowProperty*)parsedRecord->rowInfo->GetAt(rowNumber);
				for(int i=0 ; i<row->linkProperties->GetSize() ; i++){
					LinkProperty* linkInfo = (LinkProperty*)row->linkProperties->GetAt(i);

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
			if( !bAutoScrolling ){
				activeLinkID = -1;
				activeimgLinkID = -1;
				activemovLinkID = -1;
				DrawDetail( drawOffsetLine , true );
			}
		}

		//�h���b�O������~
		m_bDragging = false;
		m_bPanDragging = false;
		m_bScrollDragging = false;
		activeLinkID = -1;
		activeimgLinkID = -1;
		activemovLinkID = -1;
		
		// �e�̌Ăяo��
		::SendMessage( GetParent()->GetSafeHwnd(), WM_LBUTTONUP, (WPARAM)nFlags, (LPARAM)MAKELPARAM(point.x, point.y) );
	}

	//CWnd::OnLButtonUp(nFlags, point);
}


// �^�b�v�ɂ�郊���N�̏���
void Ran2View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		return;
		m_bDragging = false;
		::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBABLE_CURSOR) );
	}

	int lastLinkID = activeLinkID;
	int lastimgLinkID = activeimgLinkID;
	int lastmovLinkID = activemovLinkID;

	CString logStr;
	// �^�b�v�ʒu�̍s�ԍ����擾
	int tapLine = (point.y - topOffset - m_offsetPixelY + (charHeightOffset + charHeight)) / (charHeightOffset + charHeight) -1;

	// Row�z�񂩂�̎擾�ʒu���Z�o
	int rowNumber = drawOffsetLine + tapLine;

	bool bLinkArea = false;

	//�����N������OnLButtonUp�ōs��
//	// �^�b�v�ʒu���͈͓����z����ꍇ�͉������Ȃ�
	// �����N�A�ԃN���A
	activeLinkID = -1;
	activeimgLinkID = -1;
	activemovLinkID = -1;
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
							activeLinkID = linkInfo->linkID;
							break;
						case LinkType_picture:
							activeimgLinkID = linkInfo->imglinkID;
							break;
						case LinkType_movie:
							activemovLinkID = linkInfo->movlinkID;
							break;
					}
				}
			}
		}
	}

	if( lastLinkID != activeLinkID ||
		lastimgLinkID != activeimgLinkID ||
		lastmovLinkID != activemovLinkID){
			// �����N�|�C���g��Ԃɕω��������
			// �����N���]�\���p�Ɉ��ʍĕ`�恨���������I
			DrawDetail( drawOffsetLine , true );
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

	SetCapture();
	m_bDragging = true;
	m_ptDragStart = point;
	m_dragStartLine = drawOffsetLine;
	//m_ptDragStart.y -= m_offsetPixelY;
	//m_offsetPixelY = 0;
	// �܂��h���b�O�����͊m�肵�Ă��Ȃ�
	m_bPanDragging = false;
	m_bScrollDragging = false;

	// �����X�N���[����~
	KillTimer( TIMERID_AUTOSCROLL );
	bAutoScrolling = false;

	// �����X�N���[�����
	m_autoScrollInfo.clear();
	m_autoScrollInfo.push( GetTickCount(), point );

//	CWnd::OnLButtonDown(nFlags, point);
}


void Ran2View::OnMouseMove(UINT nFlags, CPoint point)
{
	CString logStr;

	int lastLinkID = activeLinkID;
	int lastimgLinkID = activeimgLinkID;
	int lastmovLinkID = activemovLinkID;

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
	int rowNumber = drawOffsetLine + tapLine;
	// �}�E�X�ʒu���͈͓����z����ꍇ�͉������Ȃ�
	// �����N�A�ԃN���A
	activeLinkID = -1;
	activeimgLinkID = -1;
	activemovLinkID = -1;
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
							activeLinkID = linkInfo->linkID;
							break;
						case LinkType_picture:
							activeimgLinkID = linkInfo->imglinkID;
							break;
						case LinkType_movie:
							activemovLinkID = linkInfo->movlinkID;
							break;
					}
				}
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

		// �c�h���b�O�J�n���f�p�I�t�Z�b�g�l
#ifndef WINCE
			// win32�̏ꍇ���s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
		int ddy = ( charHeight+charHeightOffset ) / 2 + 1 ;
#else
			// WM�̏ꍇ��s�ȓ��̈ړ��̓h���b�O�Ƃ݂Ȃ��Ȃ�
		int ddy = charHeight+charHeightOffset;
#endif
		if( m_bPanDragging ) {
			// ���h���b�O��
			//if( abs( dx ) < screenWidth / 3 ){
			//	// �}�E�X�����ɖ߂����牡�h���b�O���L�����Z���@�����������₵���̂ŕۗ�
			//	m_bPanDragging = false ;
			//}
		} else if( m_bScrollDragging ) { 
			// �c�h���b�O��

		} else {
			// �h���b�O�������m�肵�Ă��Ȃ�
			if( abs(dx) > abs(dy) && abs( dx ) > screenWidth / 3) {
				// �������̈ړ��ʂ��傫���Ĉړ��ʂ���ʂ�1/3�ȏ�̏ꍇ
				// ���h���b�O�J�n
				m_bPanDragging = true;
			} else if(  abs(dx) < abs(dy) && abs( dy ) > ddy ) {
				// �c�����̈ړ��ʂ��傫���Ĉړ��ʂ��h���b�O�J�n�I�t�Z�b�g�ȏ�̏ꍇ
				if( GetAllLineCount()-GetViewLineMax() > 0 ) {
					// �c�X�N���[���\�Ȃ��
					// �c�h���b�O�J�n
					m_bScrollDragging = true;
				}
			}
		}

		if( GetAllLineCount()-GetViewLineMax() > 0 ) {
			// �c�X�N���[���\�Ȃ��
			if( m_bScrollDragging ){
				// �����X�N���[�������W
				m_autoScrollInfo.push( GetTickCount(), point );

				// �h���b�O����
#ifdef DEBUG
	//wprintf( L"OnMouseMove\n" );
#endif
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
				if( lastLinkID != activeLinkID ||
					lastimgLinkID != activeimgLinkID ||
					lastmovLinkID != activemovLinkID){
						// �����N�|�C���g��Ԃɕω��������
						// �����N���]�\���p�Ɉ��ʍĕ`�恨���������I
						DrawDetail( drawOffsetLine , true );
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
		if( !bAutoScrolling ) {
			if( lastLinkID != activeLinkID ||
				lastimgLinkID != activeimgLinkID ||
				lastmovLinkID != activemovLinkID){
					// �����N�|�C���g��Ԃɕω��������
					// �����N���]�\���p�Ɉ��ʍĕ`�恨���������I
					DrawDetail( drawOffsetLine , true );
			}
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

#ifdef DEBUG
	//wprintf( L"---\n" );
	//wprintf( L"m_dragStartLine + d_line       : %5d\n", m_dragStartLine + d_line );
	//wprintf( L"dy           : %5d\n", dy );
	//wprintf( L"offset       : %5d\n", m_offsetPixelY );
	//wprintf( L"viewLineMax  : %5d\n", viewLineMax );
	//wprintf( L"m_dragStartLine  : %5d\n", m_dragStartLine );
#endif

	if (-dy % (charHeight + charHeightOffset) <=0 || drawOffsetLine > 0) {
		// �s�N�Z���I�t�Z�b�g
		// �i0�s�ڂł���Ώ�����̃X�N���[���͍s��Ȃ��j
		m_offsetPixelY = -dy % (charHeight + charHeightOffset);
		TRACE( L"dy,drawOffsetLine,m_offsetPixelY : %5d %5d %5d\n", dy,drawOffsetLine,m_offsetPixelY );
	} else if( drawOffsetLine == 0 ) {
		// 0�s�ڂȂ�ΐ擪�ʒu��0����
		m_offsetPixelY = 0;
	}

	if ( parsedRecord != NULL &&
		parsedRecord->rowInfo != NULL ){
		if ( m_dragStartLine + d_line > parsedRecord->rowInfo->GetSize() - viewLineMax ) {
			// ���ɂ͂ݏo���ꍇ�ŉ��[�܂ň����߂�
			 d_line = parsedRecord->rowInfo->GetSize() - viewLineMax - m_dragStartLine ;
			 m_offsetPixelY = screenHeight % (charHeight + charHeightOffset);
			TRACE( L"***dy,drawOffsetLine,m_offsetPixelY : %5d %5d %5d\n", dy,drawOffsetLine,m_offsetPixelY );
		}
#ifdef DEBUG
	//wprintf( L"m_dragStartLine + d_line       : %5d\n", m_dragStartLine + d_line );
#endif

		// ���̕��ɃX�N���[���\���m�F����
		if ((m_dragStartLine + d_line) <= parsedRecord->rowInfo->GetSize() - viewLineMax ) {
			// �X�N���[���\�Ȃ�΃X�N���[�����ĕ\��
			DrawDetail(m_dragStartLine + d_line , true);

#ifdef DEBUG
	//wprintf( L"m_dragStartLine + d_line       : %5d\n", m_dragStartLine + d_line );
#endif
			// �X�N���[���ʒu���ω�������������Ȃ��̂ŃI�[�i�[�ɒʒm
			CPoint lastPoint = m_autoScrollInfo.getLastPoint();
			::SendMessage( GetParent()->GetSafeHwnd(), WM_MOUSEMOVE, (WPARAM)0, (LPARAM)MAKELPARAM(lastPoint.x, lastPoint.y+dy) );
		}
	}
}


void Ran2View::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == TIMERID_AUTOSCROLL ){
		// �����X�N���[��

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

		TRACE( L" dt : %5d, speed : %5.2f, accel : %5.2f, dy : %5d (%5d,%5d)\n", 
			dt, speed, accel, dyAutoScroll, dyByAccel, dyByVelocity );

		// �ő�ʒu���߂����i�ɓ_�𒴂����j�A�܂���N�b�o�߂����Ȃ�I��
		if (speed == 0.0 ||
			(speed < 0 && dyAutoScroll > m_yAutoScrollMax) ||
			(speed > 0 && dyAutoScroll < m_yAutoScrollMax) ||
			dt > 5 * 1000)
		{
			KillTimer(nIDEvent);
			bAutoScrolling = false;
		} else {
			// dyAutoScroll �������ړ�����B
			CPoint lastPoint = m_autoScrollInfo.getLastPoint();
			int dy = m_ptDragStart.y - lastPoint.y - dyAutoScroll;
#ifdef DEBUG
	//wprintf( L"OnTimer\n" );
#endif
			ScrollByMoveY( dy );
		}

		m_yAutoScrollMax = dyAutoScroll;
#ifdef DEBUG
		//wprintf( L"m_yAutoScrollMax:%5d\n" , m_yAutoScrollMax);
#endif
	} else if( nIDEvent == TIMERID_PANSCROLL ) {
		// �p���X�N���[��
		if( m_dPxelX == 0 ) {
			// �ړ��ʃ[���Ȃ疳�����[�v�h�~�̂��ߒ��~
			m_offsetPixelX = 0;
			bAutoScrolling = false;
			KillTimer(nIDEvent);
		} else {
			// �ړ�����
			m_offsetPixelX += m_dPxelX;
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
				bAutoScrolling = false;
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
