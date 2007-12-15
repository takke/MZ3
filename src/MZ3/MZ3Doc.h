/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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


