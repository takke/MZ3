/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
	CString m_mixiOwnerId;			///< �I�[�i�[ID�i�t�@�C���ۑ��͂��Ȃ��j

	//--- �ėpID�Ǘ�

	/// ����T�[�r�X��ID�Ǘ���
	struct Data {
		CString strServiceName;		///< �T�[�r�X��("mixi", "goo�z�[��", "GMail", etc...)

		CString strId;				///< ID
		CString strPassword;		///< �p�X���[�h
		CString strDummy1;			///< �_�~�[1
		CString strDummy2;			///< �_�~�[2

		Data(LPCTSTR service_name=L"", LPCTSTR id=L"", LPCTSTR pw=L"")
			: strServiceName(service_name), strId(id), strPassword(pw)
		{}
	};
	std::vector<Data> m_idList;		///< ID�Ǘ�


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

	//--- �ėp

	void SetId(LPCTSTR szServiceName, LPCTSTR id);			///< ID �ݒ�
	void SetPassword(LPCTSTR szServiceName, LPCTSTR pw);	///< Password �ݒ�
	LPCTSTR GetId(LPCTSTR szServiceName);					///< ID �擾 (���o�^���� NULL��Ԃ�)
	LPCTSTR GetPassword(LPCTSTR szServiceName);				///< Password �擾 (���o�^���� NULL��Ԃ�)

	//--- �ȉ��A���o�[�W�����݊��pI/F

	//--- mixi �p
	void	SetMixiEmail(LPCTSTR str)		{ SetId(L"mixi", str);				}
	LPCTSTR GetMixiEmail()					{ return GetId(L"mixi");			}
	void	SetMixiPassword(LPCTSTR str)	{ SetPassword(L"mixi", str);		}
	LPCTSTR GetMixiPassword()				{ return GetPassword(L"mixi");		}

	void	SetMixiOwnerID(LPCTSTR str)		{ m_mixiOwnerId = str;				}
	LPCTSTR GetMixiOwnerID()				{ return m_mixiOwnerId;				}

	//--- Twitter �p
	void	SetTwitterId(LPCTSTR str)		{ SetId(L"Twitter", str);			}
	LPCTSTR GetTwitterId()					{ return GetId(L"Twitter");			}
	void	SetTwitterPassword(LPCTSTR str)	{ SetPassword(L"Twitter", str);		}
	LPCTSTR	GetTwitterPassword()			{ return GetPassword(L"Twitter");	}

	//--- Wassr �p
	void	SetWassrId(LPCTSTR str)			{ SetId(L"Wassr", str);			}
	LPCTSTR GetWassrId()					{ return GetId(L"Wassr");			}
	void	SetWassrPassword(LPCTSTR str)	{ SetPassword(L"Wassr", str);		}
	LPCTSTR	GetWassrPassword()				{ return GetPassword(L"Wassr");	}

	//--- goo�z�[�� �p
	void	SetGooId(LPCTSTR str)					{ SetId(L"goo�z�[��", str);			}
	LPCTSTR GetGooId()								{ return GetId(L"goo�z�[��");		}
	void	SetGoohomeQuoteMailAddress(LPCTSTR str)	{ SetPassword(L"goo�z�[��", str);	}
	LPCTSTR	GetGoohomeQuoteMailAddress()			{ return GetPassword(L"goo�z�[��");	}

private:
	CString ReadItem(FILE*, bool bSupportOver255=false);
	void	WriteItem(FILE*, LPCTSTR, bool bSupportOver255=false);

	CString SerializeId();
	void UnserializeId(const CString& serializedId);
};

}
