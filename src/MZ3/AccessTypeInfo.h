/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "constants.h"
#include <map>
#include <vector>

/**
 * �A�N�Z�X��ʂɑ΂���MZ3/4�̐U�镑�����Ǘ�����N���X
 */
class AccessTypeInfo
{
public:
	/// �f�[�^���̂̎��
	enum INFO_TYPE {
		INFO_TYPE_INVALID = -1,			///< ����`
		INFO_TYPE_GROUP,				///< �O���[�v�n
		INFO_TYPE_CATEGORY,				///< �J�e�S���n
		INFO_TYPE_BODY,					///< �{�f�B���ڌn
		INFO_TYPE_POST,					///< POST�n
		INFO_TYPE_OTHER,				///< ���̑�
	};

	/// ���N�G�X�g���
	enum REQUEST_METHOD {
		REQUEST_METHOD_INVALID = -1,	///< ����`
		REQUEST_METHOD_GET = 0,			///< GET ���\�b�h
		REQUEST_METHOD_POST = 1,		///< POST ���\�b�h
	};

	/// �{�f�B�� CMixiData ���̂ǂ̍��ڂ�\�����邩�̎��ʎq
	enum BODY_INDICATE_TYPE
	{
		BODY_INDICATE_TYPE_DATE,			///< ���t��\������
		BODY_INDICATE_TYPE_TITLE,			///< �^�C�g����\������
		BODY_INDICATE_TYPE_NAME,			///< ���O��\������
		BODY_INDICATE_TYPE_BODY,			///< �{����\������
		BODY_INDICATE_TYPE_NONE,			///< �����\�����Ȃ�
	};

	/// �e�A�N�Z�X��ʂ̐U�镑�����`����f�[�^�\��
	class Data
	{
	public:
		INFO_TYPE			infoType;			///< �f�[�^���̂̎�ʁi���ɗ��p�����A�N�Z�X��ʂ��H�j
		std::string			serviceType;		///< �ΏۂƂ���T�[�r�X�̎�ʁi"mixi", "Twitter"�Ȃǁj
		std::wstring		shortText;			///< ����������
		REQUEST_METHOD		requestType;		///< ���N�G�X�g���

		//--- �O���[�v�n�A�J�e�S���n�݂̂�������
		std::string			serializeKey;		///< Mz3GroupData �� ini �t�@�C���ɃV���A���C�Y����ۂ̃L�[

		//--- �J�e�S���n�݂̂�������
		bool				bCruiseTarget;		///< ����ΏۂƂ��邩�H
		std::wstring		defaultCategoryURL;	///< �J�e�S����URL
		std::wstring		bodyHeaderCol1Name;	///< �{�f�B���X�g�̃w�b�_�[1�̃J������
		std::wstring		bodyHeaderCol2NameA;///< �{�f�B���X�g�̃w�b�_�[2�̃J������A
		std::wstring		bodyHeaderCol2NameB;///< �{�f�B���X�g�̃w�b�_�[2�̃J������B
		BODY_INDICATE_TYPE	bodyHeaderCol1Type;	///< �{�f�B���X�g�̃w�b�_�[1�̃J�������
		BODY_INDICATE_TYPE	bodyHeaderCol2TypeA;///< �{�f�B���X�g�̃w�b�_�[2�̃J�������A�iB�ƃg�O���j
		BODY_INDICATE_TYPE	bodyHeaderCol2TypeB;///< �{�f�B���X�g�̃w�b�_�[2�̃J�������B�iA�ƃg�O���j

		Data(INFO_TYPE a_infoType, const char* a_serviceType, const wchar_t* a_shortText, REQUEST_METHOD a_requestType)
			: infoType(a_infoType)
			, serviceType(a_serviceType)
			, shortText(a_shortText)
			, requestType(a_requestType)
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyHeaderCol1Name(L"")
			, bodyHeaderCol2NameA(L"")
			, bodyHeaderCol2NameB(L"")
			, bodyHeaderCol1Type(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeA(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeB(BODY_INDICATE_TYPE_NONE)
		{}
		Data()
			: infoType(INFO_TYPE_INVALID)
			, serviceType("")
			, shortText(L"<unknown>")
			, requestType(REQUEST_METHOD_INVALID)
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyHeaderCol1Name(L"")
			, bodyHeaderCol2NameA(L"")
			, bodyHeaderCol2NameB(L"")
			, bodyHeaderCol1Type(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeA(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeB(BODY_INDICATE_TYPE_NONE)
		{}
	};

	typedef std::map<ACCESS_TYPE, Data> MYMAP;
	MYMAP m_map;

public:
	bool init();

	/// �J�e�S���n��ʈꗗ�̐���
	std::vector<ACCESS_TYPE> getCategoryTypeList() {
		std::vector<ACCESS_TYPE> types;
		for (MYMAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
			if (it->second.infoType==INFO_TYPE_CATEGORY) {
				types.push_back(it->first);
			}
		}
		return types;
	}

	/// �f�[�^���
	INFO_TYPE getInfoType( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return INFO_TYPE_INVALID;
		}
		return it->second.infoType;
	}

	/// �T�[�r�X��ʂ̎擾
	const char* getServiceType( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serviceType.c_str();
	}

	/// ���N�G�X�g��ʂ̎擾
	REQUEST_METHOD getRequestMethod( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return REQUEST_METHOD_INVALID;
		}
		return it->second.requestType;
	}

	/// ����������̎擾
	const wchar_t* getShortText( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"<unknown>";
		}
		return it->second.shortText.c_str();
	}

	/// �V���A���C�Y�L�[�̎擾
	const char* getSerializeKey( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serializeKey.c_str();
	}

	/// ����ΏۂƂ��邩�H�i����\��\���H�j
	const bool isCruiseTarget( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return false;
		}
		return it->second.bCruiseTarget;
	}

	/// �{�f�B���X�g�̃w�b�_�[1�̃J������
	const wchar_t* getBodyHeaderCol1Name( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol1Name.c_str();
	}

	/// �{�f�B���X�g�̃w�b�_�[2�̃J������A
	const wchar_t* getBodyHeaderCol2NameA( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2NameA.c_str();
	}

	/// �{�f�B���X�g�̃w�b�_�[2�̃J������B
	const wchar_t* getBodyHeaderCol2NameB( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2NameB.c_str();
	}

	/// �{�f�B���X�g�̃w�b�_�[1�̃J�������
	BODY_INDICATE_TYPE getBodyHeaderCol1Type( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol1Type;
	}

	/// �{�f�B���X�g�̃w�b�_�[2�̃J�������A
	BODY_INDICATE_TYPE getBodyHeaderCol2TypeA( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2TypeA;
	}

	/// �{�f�B���X�g�̃w�b�_�[2�̃J�������B
	BODY_INDICATE_TYPE getBodyHeaderCol2TypeB( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2TypeB;
	}

	/// �f�t�H���gURL
	const wchar_t* getDefaultCategoryURL( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.defaultCategoryURL.c_str();
	}

};
