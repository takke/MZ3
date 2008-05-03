/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "constants.h"
#include <map>

/**
 * �A�N�Z�X��ʂɑ΂���MZ3/4�̐U�镑�����Ǘ�����N���X
 */
class AccessTypeInfo
{
public:
	/// ���N�G�X�g���
	enum REQUEST_METHOD {
		REQUEST_METHOD_INVALID = -1,	///< ����`
		REQUEST_METHOD_GET = 0,			///< GET ���\�b�h
		REQUEST_METHOD_POST = 1,		///< POST ���\�b�h
	};

	/// �e�A�N�Z�X��ʂ̐U�镑�����`����f�[�^�\��
	class Data
	{
	public:
		std::string		serviceType;	///< �ΏۂƂ���T�[�r�X�̎�ʁi"mixi", "Twitter"�Ȃǁj
		std::wstring	shortText;		///< ����������
		REQUEST_METHOD	requestType;	///< ���N�G�X�g���
		bool			bCruiseTarget;	///< ����ΏۂƂ��邩�H

		Data(const char* a_serviceType, const wchar_t* a_shortText, REQUEST_METHOD a_requestType, bool a_bCruiseTarget)
			: serviceType(a_serviceType)
			, shortText(a_shortText)
			, requestType(a_requestType)
			, bCruiseTarget(a_bCruiseTarget)
		{}
		Data()
			: serviceType("")
			, shortText(L"<unknown>")
			, requestType(REQUEST_METHOD_INVALID)
			, bCruiseTarget(false)
		{}
	};

	typedef std::map<ACCESS_TYPE, Data> MYMAP;
	MYMAP m_map;

public:
	bool init();

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

	/// ����ΏۂƂ��邩�H�i����\��\���H�j
	const bool isCruiseTarget( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return false;
		}
		return it->second.bCruiseTarget;
	}
};
