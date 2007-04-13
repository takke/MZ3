#pragma once

/// オプションデータ
namespace option {

/**
 * ログインデータ管理クラス
 */
class Login
{
private:
	CString m_loginMail;
	CString m_loginPwd;
	CString m_ownerId;

public:
	Login();
	virtual ~Login();

	void Read();
	void Write();

	void SetEmail(LPCTSTR str) {m_loginMail = str;};
	LPCTSTR GetEmail() { return m_loginMail; }
	void SetPassword(LPCTSTR str) {m_loginPwd = str;};
	LPCTSTR GetPassword() { return m_loginPwd; };
	void SetOwnerID(LPCTSTR str) {m_ownerId = str;};
	LPCTSTR GetOwnerID() { return m_ownerId; };

private:
	LPCTSTR Read(FILE*);
	void Write(FILE*, LPCTSTR);
};

}
