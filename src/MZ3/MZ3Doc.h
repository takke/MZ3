/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// MZ3Doc.h : CMZ3Doc クラスのインターフェイス
//


#pragma once

class CMZ3Doc : public CDocument
{
protected: // シリアル化からのみ作成します。
	CMZ3Doc();
	DECLARE_DYNCREATE(CMZ3Doc)

// 属性
public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL OnNewDocument();
#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
	virtual void Serialize(CArchive& ar);
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT

// 実装
public:
	virtual ~CMZ3Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
};


