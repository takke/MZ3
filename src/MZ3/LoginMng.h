/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/// オプションデータ
namespace option {

/**
 * ログインデータ管理クラス
 */
class Login
{
private:
	//--- mixi 用
	CString m_loginMail;		///< メールアドレス
	CString m_loginPwd;			///< パスワード

	CString m_ownerId;			///< オーナーID（ファイル保存はしない）

	//--- Twitter 用
	CString m_twitterId;		///< Twitter 用ID
	CString m_twitterPwd;	///< Twitter 用パスワード

public:
	Login();
	virtual ~Login();

	void Read();
	void Write();

	//--- mixi 用
	void	SetEmail(LPCTSTR str)		{ m_loginMail = str;	}
	LPCTSTR GetEmail()					{ return m_loginMail;	}
	void	SetPassword(LPCTSTR str)	{ m_loginPwd = str;		}
	LPCTSTR GetPassword()				{ return m_loginPwd;	}
	void	SetOwnerID(LPCTSTR str)		{ m_ownerId = str;		}
	LPCTSTR GetOwnerID()				{ return m_ownerId;		}

	//--- Twitter 用
	void	SetTwitterId(LPCTSTR str)		{ m_twitterId = str;	}
	LPCTSTR GetTwitterId()					{ return m_twitterId;	}
	void	SetTwitterPassword(LPCTSTR str)	{ m_twitterPwd = str;	}
	LPCTSTR	GetTwitterPassword()			{ return m_twitterPwd;	}

private:
	CString Read(FILE*);
	void Write(FILE*, LPCTSTR);
};

}
