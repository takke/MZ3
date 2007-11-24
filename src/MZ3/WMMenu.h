/**
 * based on the sample from http://wind-master.dip.jp/
 */
// WMMenu.h: CWMMenu �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMMENU_H__56FB55C1_D713_4904_94E8_5B76F6F791DD__INCLUDED_)
#define AFX_WMMENU_H__56FB55C1_D713_4904_94E8_5B76F6F791DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWMMenu;

/*****************************************************************************
 | WMMENUBITMAP �\����
 *****************************************************************************
*/
typedef struct tagWMMENUBITMAP
{
	UINT	nID;		// �R�}���h�h�c
	int		imageIndex;	// �摜�C���f�b�N�X
} WMMENUBITMAP;


/*****************************************************************************
 | WMMENUITEM �\����
 *****************************************************************************
*/
typedef struct tagWMMENUITEM
{
	CWMMenu*	pThisMenu;			// MeasureItem �Ń|�b�v�A�b�v���T�u���j���[�̃n���h���ɂȂ��Ă��܂��̂Ŏ����ւ̃|�C���^�������Ă���
	CWMMenu*	pSubMenu;			// �T�u���j���[�ւ̃|�C���^
	CString		sItemString;		// �A�C�e���̕�����
	CString		sItemAccelerator;	// �A�C�e���̃V���[�g�J�b�g������
//	HBITMAP		hBitmap;			// �A�C�R���p�̃r�b�g�}�b�v
	int			imageIndex;			// �摜�C���f�b�N�X
} WMMENUITEM;


/*****************************************************************************
 | CWMMenu �N���X
 *****************************************************************************
*/
class CWMMenu : public CMenu
{
public:
	CWMMenu(CImageList* pImageList);
	virtual ~CWMMenu();

////////// �A�g���r���[�g //////////
private:
	UINT		m_cxMaxStr;			// ���j���[�̕��������̍ő啝
	UINT		m_cxMaxAccelerator;	// ���j���[�̃V���[�g�J�b�g�L�[�����̍ő啝

	CImageList*	m_pImageList;		// �摜���X�g

////////// �I�y���[�V���� //////////
private:
	BOOL SetItemBitmaps( WMMENUBITMAP* pMenuBmps, int nBmps );				// ���j���[�A�C�e���Ƀr�b�g�}�b�v���֘A�t����
	void ToOwnerDraw( CMenu* pMenu, WMMENUBITMAP* pMenuBmp, int nBmps );	// ���j���[�S�Ă��I�[�i�[�`��ɂ���

	// �A�C�R��
	HBITMAP CreateGrayedBitmap( CDC* pDC, int w, int h );  // �����A�C�R����`�悷�邽�߂̃r�b�g�}�b�v�𐶐�

	// ���j���[�`��
	void DrawSelected( CDC* pDC, CRect rItem, UINT itemState );		// �I����`��
	void DrawString( CDC* pDC, LPCTSTR pszStr, LPCTSTR pszAcl, CRect rItem, UINT itemState );  // ���j���[�̕����񕔕���`��
	void DrawCheckMark( CDC* pDC, CRect rItem, UINT itemState );	// �`�F�b�N�}�[�N��`��
	void DrawSeparator( CDC* pDC, CRect rItem );					// �Z�p���[�^��`��
	void DrawBitmap( CDC* pDC, int imageIndex, CRect rItem, UINT itemState );		// �r�b�g�}�b�v��`��

public:
	// ���j���[�̃��[�h�^�j��
	BOOL PrepareOwnerDraw( WMMENUBITMAP* pMenuBmps=NULL, int nBmps=0 );				// �I�[�i�[�h���[����
	virtual BOOL DestroyMenu( CWMMenu* pMenu=NULL );								// ���j���[�j��

protected:
	// �I�[�o�[���C�h
	virtual void DrawItem( LPDRAWITEMSTRUCT pDis );
	virtual void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
};

#endif // !defined(AFX_WMMENU_H__56FB55C1_D713_4904_94E8_5B76F6F791DD__INCLUDED_)
