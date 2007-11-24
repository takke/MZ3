#pragma once

#include "PostData.h"
#include "WriteEdit.h"
#include "afxwin.h"
#include "InetAccess.h"

// CWriteView フォーム ビュー

class CWriteView : public CFormView
{
	DECLARE_DYNCREATE(CWriteView)

public:
	CWriteView();           // 動的生成で使用される protected コンストラクタ
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

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

	CImageList		m_menuImageList;		///< ポップアップメニュー用画像リスト
	CStringArray	m_menuImageListMap;		///< m_menuImageList のインデックスに
											///< どのファイルが格納されているかを示す疑似マップ


	/// メインビュー（MZ3View）から起動した書き込み画面かどうかを返す
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
	/// 切り取りイベント
	afx_msg void OnEditCut() {
		// フォーカス判定
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

	/// コピーイベント
	afx_msg void OnEditCopy() {
		// フォーカス判定
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

	/// 貼り付けコマンド
	afx_msg void OnEditPaste() {
		// フォーカス判定
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

	/// 元に戻すイベント
	afx_msg void OnEditUndo() {
		// フォーカス判定
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

	// 全て選択
	afx_msg void OnSelectAll() {
		// フォーカス判定
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

	CString m_photo1_filepath;	///< 写真１のパス
	CString m_photo2_filepath;	///< 写真２のパス
	CString m_photo3_filepath;	///< 写真３のパス

public:
	BOOL m_sendEnd;

public:
	// UI 要素
	CEdit m_infoEdit;			///< 通知エディット
	CButton m_sendButton;		///< 書き込みボタン
	CButton m_cancelButton;		///< キャンセルボタン
	CWriteEdit m_bodyEdit;		///< 本文エディット
	CEdit m_titleEdit;			///< タイトルエディット

private:
	BOOL m_abort;
	BOOL m_access;

	WRITEVIEW_TYPE m_writeViewType;	///< 処理種別（日記 or コメント or メッセージ）
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


