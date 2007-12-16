/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/// �I�v�V�����f�[�^
namespace option {

/**
 * ���O�C���f�[�^�Ǘ��N���X
 */
class Login
{
private:
	//--- mixi �p
	CString m_loginMail;		///< ���[���A�h���X
	CString m_loginPwd;			///< �p�X���[�h

	CString m_ownerId;			///< �I�[�i�[ID�i�t�@�C���ۑ��͂��Ȃ��j

	//--- Twitter �p
	CString m_twitterId;		///< Twitter �pID
	CString m_twitterPwd;	///< Twitter �p�p�X���[�h

public:
	Login();
	virtual ~Login();

	void Read();
	void Write();

	//--- mixi �p
	void	SetEmail(LPCTSTR str)		{ m_loginMail = str;	}
	LPCTSTR GetEmail()					{ return m_loginMail;	}
	void	SetPassword(LPCTSTR str)	{ m_loginPwd = str;		}
	LPCTSTR GetPassword()				{ return m_loginPwd;	}
	void	SetOwnerID(LPCTSTR str)		{ m_ownerId = str;		}
	LPCTSTR GetOwnerID()				{ return m_ownerId;		}

	//--- Twitter �p
	void	SetTwitterId(LPCTSTR str)		{ m_twitterId = str;	}
	LPCTSTR GetTwitterId()					{ return m_twitterId;	}
	void	SetTwitterPassword(LPCTSTR str)	{ m_twitterPwd = str;	}
	LPCTSTR	GetTwitterPassword()			{ return m_twitterPwd;	}

private:
	CString Read(FILE*);
	void Write(FILE*, LPCTSTR);
};

}
