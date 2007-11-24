#pragma once

#include "PostData.h"
#include "WriteEdit.h"
#include "afxwin.h"
#include "InetAccess.h"

// CWriteView �t�H�[�� �r���[

class CWriteView : public CFormView
{
	DECLARE_DYNCREATE(CWriteView)

public:
	CWriteView();           // ���I�����Ŏg�p����� protected �R���X�g���N�^
	virtual ~CWriteView();

public:
	enum { IDD = IDD_WRITEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedWriteSendButton();
	afx_msg void OnBnClickedWriteCancelButton();
	afx_msg LRESULT OnGetEnd(WPARAM, LPARAM);
	afx_msg LRESULT OnPostConfirm(WPARAM, LPARAM);
	afx_msg LRESULT OnPostEnd(WPARAM, LPARAM);
	afx_msg LRESULT OnPostAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnAccessInformation(WPARAM, LPARAM);
	afx_msg LRESULT OnFit(WPARAM, LPARAM);

	BOOL IsSendEnd() { return m_sendEnd; }
	void SetSendEnd(BOOL flag) { m_sendEnd = flag; }

	CImageList		m_menuImageList;		///< �|�b�v�A�b�v���j���[�p�摜���X�g
	CStringArray	m_menuImageListMap;		///< m_menuImageList �̃C���f�b�N�X��
											///< �ǂ̃t�@�C�����i�[����Ă��邩�������^���}�b�v


	/// ���C���r���[�iMZ3View�j����N�������������݉�ʂ��ǂ�����Ԃ�
	bool IsWriteFromMainView() {
		switch( m_writeViewType ) {
		case WRITEVIEW_TYPE_NEWMESSAGE:
		case WRITEVIEW_TYPE_NEWDIARY:
			return true;
		default:
			return false;
		}
	}


private:
	/// �؂���C�x���g
	afx_msg void OnEditCut() {
		// �t�H�[�J�X����
		CWnd* wndFocused = GetFocus();
		if (wndFocused==NULL)
			return;
		if (wndFocused->m_hWnd == m_bodyEdit.m_hWnd ) {
			m_bodyEdit.Cut();
		}
		if (wndFocused->m_hWnd == m_titleEdit.m_hWnd ) {
			m_titleEdit.Cut();
		}
	}

	/// �R�s�[�C�x���g
	afx_msg void OnEditCopy() {
		// �t�H�[�J�X����
		CWnd* wndFocused = GetFocus();
		if (wndFocused==NULL)
			return;
		if (wndFocused->m_hWnd == m_bodyEdit.m_hWnd ) {
			m_bodyEdit.Copy();
		}
		if (wndFocused->m_hWnd == m_titleEdit.m_hWnd ) {
			m_titleEdit.Copy();
		}
	}

	/// �\��t���R�}���h
	afx_msg void OnEditPaste() {
		// �t�H�[�J�X����
		CWnd* wndFocused = GetFocus();
		if (wndFocused==NULL)
			return;
		if (wndFocused->m_hWnd == m_bodyEdit.m_hWnd ) {
			m_bodyEdit.Paste();
		}
		if (wndFocused->m_hWnd == m_titleEdit.m_hWnd ) {
			m_titleEdit.Paste();
		}
	}

	/// ���ɖ߂��C�x���g
	afx_msg void OnEditUndo() {
		// �t�H�[�J�X����
		CWnd* wndFocused = GetFocus();
		if (wndFocused==NULL)
			return;
		if (wndFocused->m_hWnd == m_bodyEdit.m_hWnd ) {
			m_bodyEdit.Undo();
		}
		if (wndFocused->m_hWnd == m_titleEdit.m_hWnd ) {
			m_titleEdit.Undo();
		}
	}

	// �S�đI��
	afx_msg void OnSelectAll() {
		// �t�H�[�J�X����
		CWnd* wndFocused = GetFocus();
		if (wndFocused==NULL)
			return;
		if (wndFocused->m_hWnd == m_bodyEdit.m_hWnd ) {
			m_bodyEdit.SetSel(0, -1);
		}
		if (wndFocused->m_hWnd == m_titleEdit.m_hWnd ) {
			m_titleEdit.SetSel(0, -1);
		}
	}

	afx_msg void OnWriteSendMenu() {
		OnBnClickedWriteSendButton();
	}
	afx_msg void OnWriteCancelMenu() {
		OnBnClickedWriteCancelButton();
	}
	afx_msg void OnWriteBackMenu();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CPostData* m_postData;
	CMixiData* m_data;

	CString m_photo1_filepath;	///< �ʐ^�P�̃p�X
	CString m_photo2_filepath;	///< �ʐ^�Q�̃p�X
	CString m_photo3_filepath;	///< �ʐ^�R�̃p�X

public:
	BOOL m_sendEnd;

public:
	// UI �v�f
	CEdit m_infoEdit;			///< �ʒm�G�f�B�b�g
	CButton m_sendButton;		///< �������݃{�^��
	CButton m_cancelButton;		///< �L�����Z���{�^��
	CWriteEdit m_bodyEdit;		///< �{���G�f�B�b�g
	CEdit m_titleEdit;			///< �^�C�g���G�f�B�b�g

private:
	BOOL m_abort;
	BOOL m_access;

	WRITEVIEW_TYPE m_writeViewType;	///< ������ʁi���L or �R�����g or ���b�Z�[�W�j
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	CString MyShowDlgToConfirmPostImage( CString selectedFilepath );
	void StartWriteView(WRITEVIEW_TYPE writeViewType, CMixiData* pMixi);
	void OnImageButton();
	afx_msg void OnAttachPhoto1();
	afx_msg void OnAttachPhoto2();
	afx_msg void OnAttachPhoto3();
	afx_msg void OnCancelAttachPhoto1();
	afx_msg void OnCancelAttachPhoto2();
	afx_msg void OnCancelAttachPhoto3();
	afx_msg void OnAttachPhoto();
	bool IsEnableAttachImageMode(void);

	void StartConfirmPost( CString msg );
	void StartEntryPost();
	afx_msg void OnPreviewAttachedPhoto1();
	afx_msg void OnPreviewAttachedPhoto2();
	afx_msg void OnPreviewAttachedPhoto3();
	afx_msg void OnInsertEmoji(UINT nID);

private:
	bool DumpToTemporaryDraftFile();
public:
	void PopupWriteBodyMenu(void);
};


