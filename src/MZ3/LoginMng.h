/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
	CString m_mixiOwnerId;			///< オーナーID（ファイル保存はしない）

	//--- 汎用ID管理

	/// あるサービスのID管理名
	struct Data {
		CString strServiceName;		///< サービス名("mixi", "gooホーム", "GMail", etc...)

		CString strId;				///< ID
		CString strPassword;		///< パスワード
		CString strDummy1;			///< ダミー1
		CString strDummy2;			///< ダミー2

		Data(LPCTSTR service_name=L"", LPCTSTR id=L"", LPCTSTR pw=L"")
			: strServiceName(service_name), strId(id), strPassword(pw)
		{}
	};
	std::vector<Data> m_idList;		///< ID管理


public:
	Login();
	virtual ~Login();

	void Read();
	void Write();

	bool IsAllEmpty()
	{
		if (m_idList.empty()) {
			return true;
		}
		return false;
	}

	//--- 汎用

	void SetId(LPCTSTR szServiceName, LPCTSTR id);			///< ID 設定
	void SetPassword(LPCTSTR szServiceName, LPCTSTR pw);	///< Password 設定
	LPCTSTR GetId(LPCTSTR szServiceName);					///< ID 取得 (未登録時は NULLを返す)
	LPCTSTR GetPassword(LPCTSTR szServiceName);				///< Password 取得 (未登録時は NULLを返す)

	//--- 以下、旧バージョン互換用I/F

	//--- mixi 用
	void	SetMixiEmail(LPCTSTR str)		{ SetId(L"mixi", str);				}
	LPCTSTR GetMixiEmail()					{ return GetId(L"mixi");			}
	void	SetMixiPassword(LPCTSTR str)	{ SetPassword(L"mixi", str);		}
	LPCTSTR GetMixiPassword()				{ return GetPassword(L"mixi");		}

	void	SetMixiOwnerID(LPCTSTR str)		{ m_mixiOwnerId = str;				}
	LPCTSTR GetMixiOwnerID()				{ return m_mixiOwnerId;				}

	//--- Twitter 用
	void	SetTwitterId(LPCTSTR str)		{ SetId(L"Twitter", str);			}
	LPCTSTR GetTwitterId()					{ return GetId(L"Twitter");			}
	void	SetTwitterPassword(LPCTSTR str)	{ SetPassword(L"Twitter", str);		}
	LPCTSTR	GetTwitterPassword()			{ return GetPassword(L"Twitter");	}

	//--- Wassr 用
	void	SetWassrId(LPCTSTR str)			{ SetId(L"Wassr", str);			}
	LPCTSTR GetWassrId()					{ return GetId(L"Wassr");			}
	void	SetWassrPassword(LPCTSTR str)	{ SetPassword(L"Wassr", str);		}
	LPCTSTR	GetWassrPassword()				{ return GetPassword(L"Wassr");	}

	//--- gooホーム 用
	void	SetGooId(LPCTSTR str)					{ SetId(L"gooホーム", str);			}
	LPCTSTR GetGooId()								{ return GetId(L"gooホーム");		}
	void	SetGoohomeQuoteMailAddress(LPCTSTR str)	{ SetPassword(L"gooホーム", str);	}
	LPCTSTR	GetGoohomeQuoteMailAddress()			{ return GetPassword(L"gooホーム");	}

private:
	CString ReadItem(FILE*, bool bSupportOver255=false);
	void	WriteItem(FILE*, LPCTSTR, bool bSupportOver255=false);

	CString SerializeId();
	void UnserializeId(const CString& serializedId);
};

}
