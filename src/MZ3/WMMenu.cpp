/**
 * based on the sample from http://wind-master.dip.jp/
 */
// WMMenu.cpp: CWMMenu �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "WMMenu.h"
#include "resourceppc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************
 | �萔��`
 *****************************************************************************
*/
// �A�C�e���̃T�C�Y�E�]��
#define CX_LSP		20	// ���̗]���i�A�C�R���`�敔���j
#define CX_RSP		16	// �E�̗]���i���j���[������ƃV���[�g�J�b�g������̊Ԃ̗]���j
#define CX_STRSP	4	// ������̗]��
#define CY_ITEM		22	// �A�C�e���̍���
#define CX_ITEMSP	( CX_LSP + CX_STRSP )	// �A�C�e���̕�����ȊO�̗]���S�̂̃T�C�Y

#define ICON_SIZE	16	// �A�C�R���̃T�C�Y�i���E�����j
#define X_ICON		( ( CX_LSP - ICON_SIZE ) / 2 )
#define Y_ICON		( ( CY_ITEM - ICON_SIZE ) / 2 )


// �J���[
#define CL_SELECTED		RGB( 120, 160, 200 )	// �I��
#define CL_SELBORDER	RGB(   0,  50, 128 )	// �I���̋��E��
#define CL_NORMAL_STR	RGB(   0,   0,   0 )	// �ʏ�̕�����
#define CL_SELECTED_STR	RGB( 255, 255, 255 )	// �I����Ԃ̕�����
#define CL_GRAYED_STR	RGB( 140, 140, 140 )	// �����̕�����
#define CL_SEPARATOR	RGB( 128, 128, 128 )	// �Z�p���[�^
#define CL_BACKGROUND	RGB( 241, 240, 239 )	// �w�i
#define CL_BGCHECK		RGB( 241, 240, 239 )	// �`�F�b�N�̔w�i
#define CL_CHECKBORDER	RGB(  64,  64,  64 )	// �`�F�b�N�̋��E��



/*****************************************************************************
 | �֐��� | CWMMenu
 +--------+-------------------------------------------------------------------
 |  ����  | �R���X�g���N�^
 +--------+-------------------------------------------------------------------
 |  ����  | �Ȃ�
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
CWMMenu::CWMMenu(CImageList* pImageList)
{
	m_cxMaxStr = 0;
	m_cxMaxAccelerator = 0;
	m_pImageList = pImageList;
}


/*****************************************************************************
 | �֐��� | ~CWMMenu
 +--------+-------------------------------------------------------------------
 |  ����  | �f�X�g���N�^
 +--------+-------------------------------------------------------------------
 |  ����  | �Ȃ�
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
CWMMenu::~CWMMenu()
{
	DestroyMenu();
}


/*****************************************************************************
 | �֐��� | SetItemBitmap
 +--------+-------------------------------------------------------------------
 |  ����  | ���j���[�A�C�e���Ƀr�b�g�}�b�v���֘A�t����
 +--------+-------------------------------------------------------------------
 |  ����  | pMenuBmps         ���j���[�Ɋ֘A�t����A�R�}���h�h�c�ƃr�b�g�}�b
 |        |                   �v�������\���̂̔z��
 |        +-------------------------------------------------------------------
 |        | nBmps             pMenuBmp�̔z��
 +--------+-------------------------------------------------------------------
 | �߂�l | ����I���Ȃ�O�ȊO��Ԃ��A����ȊO�͂O��Ԃ�
 *****************************************************************************
*/
BOOL CWMMenu::SetItemBitmaps( WMMENUBITMAP* pMenuBmps, int nBmps )
{
	ASSERT( pMenuBmps );
//	ASSERT( nBmps );

	for ( int i = 0; i < nBmps; i++ )
	{
		// ���j���[�̃A�C�e�����擾
		MENUITEMINFO miInfo;
		::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
		miInfo.cbSize = sizeof(MENUITEMINFO);
		miInfo.fMask = MIIM_DATA;

		if ( ::GetMenuItemInfo( m_hMenu, pMenuBmps[ i ].nID, FALSE, &miInfo ) )
		{
			WMMENUITEM* pMenuItem = (WMMENUITEM*)miInfo.dwItemData;
			ASSERT( pMenuItem );

			pMenuItem->imageIndex = pMenuBmps[ i ].imageIndex;
//			pMenuItem->hBitmap = pMenuBmps[ i ].hBitmap;
/*			pMenuItem->hBitmap = (HBITMAP)::LoadImage( AfxGetInstanceHandle()
													 , MAKEINTRESOURCE( IDB_BITMAP2 )
													 , IMAGE_BITMAP
													 , ICON_SIZE, ICON_SIZE
													 , LR_LOADMAP3DCOLORS );
*/
//			if ( ! pMenuItem->hBitmap )
//			{
//				return FALSE;
//			}
		}
	}

	return TRUE;
}


/*****************************************************************************
 | �֐��� | ToOwnerDraw
 +--------+-------------------------------------------------------------------
 |  ����  | ���j���[�S�Ă��I�[�i�[�`��ɂ���
 +--------+-------------------------------------------------------------------
 |  ����  | pMenu  CMenu�ւ̃|�C���^
 |        +-------------------------------------------------------------------
 |        | pMenuBmps         ���j���[�Ɋ֘A�t����A�R�}���h�h�c�ƃr�b�g�}�b
 |        |                   �v�������\���̂̔z��i�K�v�Ȃ��ꍇ��NULL)
 |        +-------------------------------------------------------------------
 |        | nBmps             pMenuBmp�̔z�񐔁ipMenuBmp��NULL�̏ꍇ�͖����j
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::ToOwnerDraw( CMenu* pMenu, WMMENUBITMAP* pMenuBmps, int nBmps )
{
	ASSERT( pMenu );
	ASSERT( pMenu->m_hMenu );

	UINT i = 0;

	while ( 1 )
	{
		// ���j���[�̃A�C�e�����擾
		MENUITEMINFO miInfo;
		::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
		miInfo.cbSize = sizeof (MENUITEMINFO);
		miInfo.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;

		TCHAR buf[ 256 ];
		miInfo.dwTypeData = buf;
		miInfo.cch = 256;
		if ( ! ::GetMenuItemInfo( pMenu->m_hMenu, i, TRUE, &miInfo ) )
		{
			break;
		}

		miInfo.fMask = ( pMenu == this ) ? 0 : MIIM_TYPE;

		// ���j���[�̃A�C�e���f�[�^�ɕێ�����\���̂̏���
		WMMENUITEM* pMenuItem = NULL;
		if ( ! ( miInfo.fType & MFT_SEPARATOR ) )
		{
			pMenuItem = new WMMENUITEM;

			// TAB�O�ƌ���킯��
			TCHAR* psz = _tcschr( buf, '\t' );
			if ( psz )
			{
				*psz = '\0';
				psz++;
				pMenuItem->sItemAccelerator = psz;
			}

			pMenuItem->pThisMenu	= (CWMMenu*)pMenu;
			pMenuItem->pSubMenu		= NULL;
			pMenuItem->sItemString	= buf;
			pMenuItem->imageIndex	= -1;

			miInfo.fMask |= MIIM_DATA;
			miInfo.dwItemData = (DWORD)pMenuItem;
		}

		// �T�u���j���[����������ACWMMenu��V��������ăA�^�b�`����
		if ( miInfo.hSubMenu )
		{
			ASSERT( pMenuItem );

			pMenuItem->pSubMenu = new CWMMenu( m_pImageList );
			pMenuItem->pSubMenu->Attach( miInfo.hSubMenu );
			ToOwnerDraw( pMenuItem->pSubMenu, pMenuBmps, nBmps );

			if ( pMenuBmps )
			{
			#ifdef _DEBUG
				BOOL bRet = 
			#endif

				pMenuItem->pSubMenu->SetItemBitmaps( pMenuBmps, nBmps );

			#ifdef _DEBUG
				ASSERT( bRet );
			#endif
			}
		}

		// �I�[�i�[�`��t���O���Z�b�g
		if ( pMenu != this )
		{
			miInfo.fType |= MFT_OWNERDRAW;
		}

		if ( ! ::SetMenuItemInfo( pMenu->m_hMenu, i, TRUE, &miInfo ) )
		{
			break;
		}
		
		i++;
	}
}


/*****************************************************************************
 | �֐��� | CreateGrayedBitmap
 +--------+-------------------------------------------------------------------
 |  ����  | �����A�C�R����`�悷�邽�߂̃r�b�g�}�b�v�𐶐�
 +--------+-------------------------------------------------------------------
 |  ����  | pDC  CDC�ւ̃|�C���^
 |        +-------------------------------------------------------------------
 |        | w    ��
 |        +-------------------------------------------------------------------
 |        | h    ����
 +--------+-------------------------------------------------------------------
 | �߂�l | ����I��������r�b�g�}�b�v�n���h���A����ȊO��NULL��Ԃ�
 +--------+-------------------------------------------------------------------
 |  ���l  | �r�b�g�}�b�v�� DeleteObject()�Ŕj�����Ȃ��Ă͂Ȃ�Ȃ�
 *****************************************************************************
*/
HBITMAP CWMMenu::CreateGrayedBitmap( CDC* pDC, int w, int h )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	// �r�b�g�}�b�v��񐶐�
	DWORD dwSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 16;
	BITMAPINFO* pbmi = (BITMAPINFO*)new BYTE[ dwSize ];
	::ZeroMemory( pbmi, dwSize );

	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= w;
	pbmi->bmiHeader.biHeight		= h;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= 4;
	pbmi->bmiHeader.biCompression	= BI_RGB;

	// �p���b�g�̐ݒ�
	for ( int i = 0; i < 16; i++ )
	{
		COLORREF clr;
		switch ( i )
		{
			case 1:	clr = ::GetSysColor( COLOR_3DSHADOW );	break; 
			case 2:	clr = ::GetSysColor( COLOR_3DLIGHT );	break; 
			default:
				clr = ::GetSysColor( COLOR_3DFACE );
				break;
		}
		pbmi->bmiColors[ i ].rgbRed		= GetRValue( clr );
		pbmi->bmiColors[ i ].rgbGreen	= GetGValue( clr );
		pbmi->bmiColors[ i ].rgbBlue	= GetBValue( clr );
	}

	BYTE* pBits;
	HBITMAP hBitmap = ::CreateDIBSection( pDC->GetSafeHdc(), pbmi, DIB_RGB_COLORS, (PVOID*)&pBits, 0, 0 );

	delete [] (BYTE*)pbmi;

	return hBitmap;
}


/*****************************************************************************
 | �֐��� | DrawSelected
 +--------+-------------------------------------------------------------------
 |  ����  | �I����`��
 +--------+-------------------------------------------------------------------
 |  ����  | pDC        �`�悷��CDC�ւ̃|�C���^
 |        +-------------------------------------------------------------------
 |        | rItem      ���j���[�A�C�e���̊O�ڎl�p�`
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT�\���̂�itemState�Ɠ���
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::DrawSelected( CDC* pDC, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );
	if ( ! ( itemState & ODS_SELECTED ) || ( itemState & ODS_GRAYED ) )
	{
		return;
	}
	
	// �I���̕`��
	CPen	pen( PS_SOLID, 1, CL_SELBORDER );
	CBrush	brush( CL_SELECTED );

	CPen*	pOldPen		= pDC->SelectObject( &pen );
	CBrush* pOldBrush	= pDC->SelectObject( &brush );

	pDC->Rectangle( &rItem );

	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );
}


/*****************************************************************************
 | �֐��� | DrawString
 +--------+-------------------------------------------------------------------
 |  ����  | ���j���[�̕����񕔕���`��
 +--------+-------------------------------------------------------------------
 |  ����  | pDC        �`�悷��CDC�ւ̃|�C���^
 |        +-------------------------------------------------------------------
 |        | pszStr     �`�悷�镶����
 |        +-------------------------------------------------------------------
 |        | pszAcl     �`�悷��V���[�g�J�b�g������
 |        +-------------------------------------------------------------------
 |        | rItem      ���j���[�A�C�e���̊O�ڎl�p�`
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT�\���̂�itemState�Ɠ���
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::DrawString( CDC* pDC, LPCTSTR pszStr, LPCTSTR pszAcl, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	// �`��F�̐ݒ�
	COLORREF OldClr;
	if ( itemState & ODS_GRAYED )
	{ 
		OldClr = pDC->SetTextColor( CL_GRAYED_STR );
	}
	else if ( itemState & ODS_SELECTED )
	{
		OldClr = pDC->SetTextColor( CL_SELECTED_STR );
	}
	else
	{
		OldClr = pDC->SetTextColor( CL_NORMAL_STR );
	}

	// �w�i�𓧉�
	int OldBkMode = pDC->GetBkMode();
	pDC->SetBkMode( TRANSPARENT );

	// �������`�悷��l�p�`���v�Z
	rItem.left	+= CX_LSP + CX_STRSP;
	rItem.right	= rItem.left + m_cxMaxStr;

	// ������̕`��
	pDC->DrawText( pszStr
				 , _tcslen( pszStr )
				 , &rItem
				 , DT_SINGLELINE | DT_EXPANDTABS | DT_LEFT | DT_VCENTER );

	if ( pszAcl )
	{
		// �V���[�g�J�b�g�������`�悷��l�p�`���v�Z
		rItem.left = rItem.right + CX_RSP;
		rItem.right= rItem.right + m_cxMaxAccelerator;

		// �V���[�g�J�b�g������̕`��
		pDC->DrawText( pszAcl
					 , _tcslen( pszAcl )
					 , &rItem
					 , DT_SINGLELINE | DT_EXPANDTABS | DT_LEFT | DT_VCENTER );
	}

	pDC->SetBkMode( OldBkMode );
	pDC->SetTextColor( OldClr );
}


/*****************************************************************************
 | �֐��� | DrawCheckMark
 +--------+-------------------------------------------------------------------
 |  ����  | �I�`�F�b�N�}�[�N��`��
 +--------+-------------------------------------------------------------------
 |  ����  | pDC        �`�悷��CDC�ւ̃|�C���^
 |        +-------------------------------------------------------------------
 |        | rItem      ���j���[�A�C�e���̊O�ڎl�p�`
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT�\���̂�itemState�Ɠ���
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::DrawCheckMark( CDC* pDC, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	if ( ! ( itemState & ODS_CHECKED ) )
	{
		return;
	}
	
	// �`�F�b�N��`�悷��r�b�g�}�b�v����
	HBITMAP hBitmap;
	if ( itemState & ODS_GRAYED )
	{
		hBitmap = CreateGrayedBitmap( pDC, ICON_SIZE, ICON_SIZE );
	}
	else
	{
		hBitmap = ::CreateCompatibleBitmap( pDC->GetSafeHdc(), ICON_SIZE, ICON_SIZE ); 
	}

	ASSERT( hBitmap );

	// ������DC�̐���
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	HBITMAP hOldBmp	= (HBITMAP)::SelectObject( memDC.GetSafeHdc(), hBitmap );


	// GDI�I�u�W�F�N�g����
	CPen pen( PS_SOLID, 1, CL_CHECKBORDER );
	CBrush brush( CL_BGCHECK );

	// GDI�I�u�W�F�N�g�I��
	CPen*	pOldPen		= memDC.SelectObject( &pen );
	CBrush* pOldBrush	= memDC.SelectObject( &brush );

	// ������DC�Ƀ`�F�b�N��`��
	memDC.Rectangle( CRect( 0, 0, ICON_SIZE, ICON_SIZE ) );
	POINT points[ 6 ] = { 
							{  5,  6 }
						  , {  7,  8 }
						  , { 10,  5 }
						  , { 10,  7 }
						  , {  7, 10 }
						  , {  5,  8 }
						};

	CBrush brushCheck( CL_CHECKBORDER );
	memDC.SelectObject( &brushCheck );
	memDC.Polygon( points, 6 );


	// �`�F�b�N��`�悷��l�p�`���v�Z
	rItem.left	 = X_ICON;
	rItem.top	 = rItem.top + Y_ICON;
	rItem.right	 = rItem.left + ICON_SIZE;
	rItem.bottom = rItem.top + ICON_SIZE;

	// ������DC����pDC�ɓ]��
	pDC->BitBlt( rItem.left, rItem.top, rItem.Width(), rItem.Height(), &memDC, 0, 0, SRCCOPY );

	// GDI�I�u�W�F�N�g�����ɖ߂�
	::SelectObject( memDC.GetSafeHdc(), hOldBmp );
	memDC.SelectObject( pOldBrush );
	memDC.SelectObject( pOldPen );

	::DeleteObject( hBitmap );
}


/*****************************************************************************
 | �֐��� | DrawSeparator
 +--------+-------------------------------------------------------------------
 |  ����  | �Z�p���[�^��`��
 +--------+-------------------------------------------------------------------
 |  ����  | pDC        �`�悷��CDC�ւ̃|�C���^
 |        +-------------------------------------------------------------------
 |        | rItem      ���j���[�A�C�e���̊O�ڎl�p�`
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::DrawSeparator( CDC* pDC, CRect rItem )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	CPen pen( PS_SOLID, 1, CL_SEPARATOR );
	CPen* pOldPen = pDC->SelectObject( &pen );

	pDC->MoveTo( rItem.left  + CX_LSP + CX_STRSP, rItem.top );
	pDC->LineTo( rItem.right - CX_STRSP, rItem.top );

	pDC->SelectObject( pOldPen );
}


/*****************************************************************************
 | �֐��� | DrawBitmap
 +--------+-------------------------------------------------------------------
 |  ����  | �r�b�g�}�b�v��`��
 +--------+-------------------------------------------------------------------
 |  ����  | pDC        �`�悷��CDC�ւ̃|�C���^
 |        +-------------------------------------------------------------------
 |        | imageIndex �`�悷��r�b�g�}�b�v�̃C���f�b�N�X
 |        +-------------------------------------------------------------------
 |        | rItem      ���j���[�A�C�e���̊O�ڎl�p�`
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT�\���̂�itemState�Ɠ���
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::DrawBitmap( CDC* pDC, int imageIndex, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	if (m_pImageList==NULL) {
		return;
	}

	// �r�b�g�}�b�v����
	HBITMAP hWorkBmp;
	if ( itemState & ODS_GRAYED )
	{
		hWorkBmp = CreateGrayedBitmap( pDC, ICON_SIZE, ICON_SIZE );
	}
	else
	{
		hWorkBmp = ::CreateCompatibleBitmap( pDC->GetSafeHdc(), ICON_SIZE, ICON_SIZE ); 
	}

	ASSERT( hWorkBmp );

	// ������DC�̐���
	CDC memWorkDC;
	memWorkDC.CreateCompatibleDC( pDC );
	HBITMAP hOldWorkBmp	= (HBITMAP)::SelectObject( memWorkDC.GetSafeHdc(), hWorkBmp );
	
	// �]���p�r�b�g�}�b�v�ɕ`��
	m_pImageList->DrawIndirect( &memWorkDC, imageIndex, CPoint(0,0), CSize(ICON_SIZE, ICON_SIZE), CPoint(0,0) );

	// �r�b�g�}�b�v��`�悷��l�p�`���v�Z
	CRect rBmp = rItem;
	rBmp.left	= X_ICON;
	rBmp.top	= rBmp.top + Y_ICON;
	rBmp.right	= rBmp.left + ICON_SIZE;
	rBmp.bottom	= rBmp.top + ICON_SIZE;

	// ���E���`��̂��߂Ɏl�p�`���Čv�Z
	rItem.left	= 0;
	rItem.right	= rItem.left + CX_LSP;
	rItem.bottom= rItem.top + CY_ITEM;

	// ���E���̕`��
	if ( itemState & ODS_CHECKED )
	{
		pDC->DrawEdge( &rItem, EDGE_ETCHED, BF_RECT | BF_SOFT | BF_MIDDLE );
	}
	else if ( ( itemState & ODS_SELECTED ) && ! ( itemState & ODS_GRAYED ) )
	{
		pDC->DrawEdge( &rItem, EDGE_BUMP, BF_RECT | BF_SOFT | BF_MIDDLE );
	}

	// ������DC����pDC�ɓ]��
	pDC->BitBlt( rBmp.left, rBmp.top, rBmp.Width(), rBmp.Height(), &memWorkDC, 0, 0, SRCCOPY );

	// GDI�I�u�W�F�N�g�����ɖ߂�
	::SelectObject( memWorkDC.GetSafeHdc(), hOldWorkBmp );

	::DeleteObject( hWorkBmp );
}


/*****************************************************************************
 | �֐��� | PrepareOwnerDraw
 +--------+-------------------------------------------------------------------
 |  ����  | �I�[�i�[�h���[����
 +--------+-------------------------------------------------------------------
 |  ����  | pMenuBmps         ���j���[�Ɋ֘A�t����A�R�}���h�h�c�ƃr�b�g�}�b
 |        |                   �v�������\���̂̔z��i�K�v�Ȃ��ꍇ��NULL)
 |        +-------------------------------------------------------------------
 |        | nBmps             pMenuBmp�̔z�񐔁ipMenuBmp��NULL�̏ꍇ�͖����j
 +--------+-------------------------------------------------------------------
 | �߂�l | ����I���Ȃ�O�ȊO��Ԃ��A����ȊO�͂O��Ԃ�
 *****************************************************************************
*/
BOOL CWMMenu::PrepareOwnerDraw( WMMENUBITMAP* pMenuBmps/*=NULL*/, int nBmps/*=0*/ )
{
	ToOwnerDraw( this, pMenuBmps, nBmps );

	if ( pMenuBmps )
	{
	#ifdef _DEBUG
		BOOL bRet = 
	#endif

		SetItemBitmaps( pMenuBmps, nBmps );

	#ifdef _DEBUG
		ASSERT( bRet );
	#endif
	}

	return TRUE;
}


//BOOL CWMMenu::LoadMenu( UINT nIDResource, WMMENUBITMAP* pMenuBmps/*=NULL*/, int nBmps/*=0*/ )
//{
//	return LoadMenu( MAKEINTRESOURCE( nIDResource ), pMenuBmps, nBmps );
//}


/*****************************************************************************
 | �֐��� | DestroyMenu
 +--------+-------------------------------------------------------------------
 |  ����  | ���j���[�j��
 +--------+-------------------------------------------------------------------
 |  ����  | pMenu  �j�����郁�j���[�ւ̃|�C���^
 |        |        �iCWMMenu�N���X�����ȊO�����NULL�ɂ���j
 +--------+-------------------------------------------------------------------
 | �߂�l | ����I���Ȃ�O�ȊO��Ԃ��A����ȊO�͂O��Ԃ�
 *****************************************************************************
*/
BOOL CWMMenu::DestroyMenu( CWMMenu* pMenu/*=NULL*/ )
{
	BOOL bCallOrgDestroyMenu = FALSE;

	if ( ! pMenu )
	{
		if ( ! m_hMenu )
		{
			return FALSE;
		}
		pMenu = this;
		bCallOrgDestroyMenu = TRUE;
	}

	ASSERT( pMenu->m_hMenu );

	UINT i = 0;

	while ( 1 )
	{
		// ���j���[�̃A�C�e�����擾
		MENUITEMINFO miInfo;
		::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
		miInfo.cbSize = sizeof (MENUITEMINFO);
		miInfo.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_DATA;
		if ( ! ::GetMenuItemInfo( pMenu->m_hMenu, i++, TRUE, &miInfo ) )
		{
			break;
		}

		// �Z�p���[�^�ȊO�ɑ΂��ď�������
		if ( ! ( miInfo.fType & MFT_SEPARATOR ) )
		{
			WMMENUITEM* pMenuItem = (WMMENUITEM*)miInfo.dwItemData;

//			ASSERT( pMenuItem );
			if (pMenuItem != NULL) {

				// �T�u���j���[����������A�ċN�Ăяo�����Ĕj��
				if ( pMenuItem->pSubMenu )
				{
					DestroyMenu( pMenuItem->pSubMenu );
					pMenuItem->pSubMenu->Detach();
					delete pMenuItem->pSubMenu;
				}

				delete pMenuItem;
			}
		}
	}

	if ( bCallOrgDestroyMenu )
	{
		CMenu::DestroyMenu();
	}

	return TRUE;
}


/*****************************************************************************
 | �֐��� | DrawItem
 +--------+-------------------------------------------------------------------
 |  ����  | �I�[�i�[�`��iCMenu�̉��z�֐��j
 +--------+-------------------------------------------------------------------
 |  ����  | pDis
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::DrawItem( LPDRAWITEMSTRUCT pDis )
{
	if ( pDis->CtlType != ODT_MENU )
	{
		return;
	}

	// ���j���[�̃A�C�e�����擾
	MENUITEMINFO miInfo;
	::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
	miInfo.cbSize = sizeof (MENUITEMINFO);
	miInfo.fMask = MIIM_TYPE | MIIM_DATA;

	if ( ! ::GetMenuItemInfo( m_hMenu, pDis->itemID, FALSE, &miInfo ) )
	{
		ASSERT( FALSE );
		return;
	}

	CDC dc;
	dc.Attach( pDis->hDC );

	// �w�i��h��Ԃ�
	CRect r = pDis->rcItem;
	r.right = CX_LSP;
	dc.FillRect( &r, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );

	r.right = pDis->rcItem.right;
	r.left  = CX_LSP;
	dc.FillRect( &r, &CBrush( CL_BACKGROUND ) );

	
	if ( ! ( miInfo.fType & MFT_SEPARATOR ) )
	{
		// �I����`��
		DrawSelected( &dc, pDis->rcItem, pDis->itemState );

		// �������`��
		WMMENUITEM* pMenuItem = (WMMENUITEM*)miInfo.dwItemData;
		if ( pMenuItem->sItemAccelerator.IsEmpty() )
		{
			DrawString( &dc, pMenuItem->sItemString, NULL, pDis->rcItem, pDis->itemState );
		}
		else
		{
			DrawString( &dc, pMenuItem->sItemString, pMenuItem->sItemAccelerator, pDis->rcItem, pDis->itemState );
		}
		
		// �`�F�b�N�}�[�N��`��
		DrawCheckMark( &dc, pDis->rcItem, pDis->itemState );

		// �r�b�g�}�b�v�`��
		if ( pMenuItem->imageIndex>=0 )
		{
			DrawBitmap( &dc, pMenuItem->imageIndex, pDis->rcItem, pDis->itemState );
		}
	}
	else
	{
		// �Z�p���[�^��`��
		DrawSeparator( &dc, pDis->rcItem );
	}

	dc.Detach();
}


/*****************************************************************************
 | �֐��� | MeasureItem
 +--------+-------------------------------------------------------------------
 |  ����  | ���j���[���̐ݒ�iCMenu�̉��z�֐��j
 +--------+-------------------------------------------------------------------
 |  ����  | lpMeasureItemStruct
 +--------+-------------------------------------------------------------------
 | �߂�l | �Ȃ�
 *****************************************************************************
*/
void CWMMenu::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	if ( lpMeasureItemStruct->CtlType != ODT_MENU )
	{
		return;
	}

	WMMENUITEM* pMenuItem = (WMMENUITEM*)lpMeasureItemStruct->itemData;
	if ( ! pMenuItem )
	{
		// �Z�p���[�^
		lpMeasureItemStruct->itemWidth = 1;
		lpMeasureItemStruct->itemHeight= 1;
		return;
	}

	// ���j���[�������\�����邽�߂̎l�p�`���v�Z����
	HDC hDC = ::GetDC( NULL );

	// ���j���[�t�H���g�̎擾
//	NONCLIENTMETRICS nm;
//	nm.cbSize = sizeof(NONCLIENTMETRICS);
//	SystemParametersInfo( SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0 );
//	HFONT hFont = ::CreateFontIndirect( &nm.lfMenuFont );
//	HFONT hOldFont = (HFONT)::SelectObject( hDC, hFont );

	// �������`�悷�邽�߂ɕK�v�Ȏl�p�`���擾����
	CRect rItemStr( 0, 0, 0, 0 );
	::DrawText( hDC, pMenuItem->sItemString, pMenuItem->sItemString.GetLength(), &rItemStr, DT_CALCRECT | DT_SINGLELINE | DT_EXPANDTABS );

	CRect rItemAcl( 0, 0, 0, 0 );
	if ( ! pMenuItem->sItemAccelerator.IsEmpty() )
	{
		::DrawText( hDC, pMenuItem->sItemAccelerator, pMenuItem->sItemAccelerator.GetLength(), &rItemAcl, DT_CALCRECT | DT_SINGLELINE | DT_EXPANDTABS );
	}

//	::SelectObject( hDC, hOldFont );
//	::DeleteObject( hFont );
	::ReleaseDC( NULL, hDC );

	// ���j���[�̕��ƍ�����ݒ�
	pMenuItem->pThisMenu->m_cxMaxStr = max( pMenuItem->pThisMenu->m_cxMaxStr, (UINT)rItemStr.Width() );
	if ( rItemAcl.Width() )
	{
		pMenuItem->pThisMenu->m_cxMaxAccelerator = max( pMenuItem->pThisMenu->m_cxMaxAccelerator, (UINT)rItemAcl.Width() + CX_RSP );
	}

	lpMeasureItemStruct->itemWidth = pMenuItem->pThisMenu->m_cxMaxStr + pMenuItem->pThisMenu->m_cxMaxAccelerator + CX_ITEMSP;
	lpMeasureItemStruct->itemHeight= max( CY_ITEM, rItemStr.Height() );
}
