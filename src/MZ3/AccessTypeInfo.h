/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "constants.h"
#include <map>
#include <vector>
#include <string>

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
	LPCTSTR info_type_to_text(INFO_TYPE e) {
		switch (e) {
		case INFO_TYPE_INVALID:		return L"����`";
		case INFO_TYPE_GROUP:		return L"�O���[�v�n";
		case INFO_TYPE_CATEGORY:	return L"�J�e�S���n";
		case INFO_TYPE_BODY:		return L"�{�f�B���ڌn";
		case INFO_TYPE_POST:		return L"POST�n";
		case INFO_TYPE_OTHER:
		default:					return L"���̑�";
		}
	}
	INFO_TYPE text_to_info_type(const CString& s) {
		if (s==L"����`")		return INFO_TYPE_INVALID;
		if (s==L"�O���[�v�n")	return INFO_TYPE_GROUP;
		if (s==L"�J�e�S���n")	return INFO_TYPE_CATEGORY;
		if (s==L"�{�f�B���ڌn") return INFO_TYPE_BODY;		
		if (s==L"POST�n")		return INFO_TYPE_POST;
		return INFO_TYPE_OTHER;
	}

	/// ���N�G�X�g���
	enum REQUEST_METHOD {
		REQUEST_METHOD_INVALID = -1,	///< ����`
		REQUEST_METHOD_GET = 0,			///< GET ���\�b�h
		REQUEST_METHOD_POST = 1,		///< POST ���\�b�h
	};
	LPCTSTR request_method_to_text(REQUEST_METHOD e) {
		switch (e) {
		case REQUEST_METHOD_GET:		return L"GET";
		case REQUEST_METHOD_POST:		return L"POST";
		case REQUEST_METHOD_INVALID:
		default:						return L"����`";
		}
	}
	REQUEST_METHOD text_to_request_method(const CString& s) {
		if (s==L"GET")	return REQUEST_METHOD_GET;
		if (s==L"POST")	return REQUEST_METHOD_POST;
		return REQUEST_METHOD_INVALID;
	}

	/// �G���R�[�f�B���O
	enum ENCODING {
		ENCODING_SJIS = 0,
		ENCODING_EUC  = 1,
		ENCODING_UTF8 = 2,
		ENCODING_NOCONVERSION = 3,
	};
	LPCTSTR encoding_to_text(ENCODING e) {
		switch (e) {
		case ENCODING_SJIS:			return L"SJIS";
		case ENCODING_EUC:			return L"EUC";
		case ENCODING_UTF8:			return L"UTF8";
		case ENCODING_NOCONVERSION:
		default:					return L"NOCONVERSION";
		}
	}
	ENCODING text_to_encoding(const CString& s) {
		if (s==L"SJIS")	return ENCODING_SJIS;
		if (s==L"EUC")	return ENCODING_EUC;
		if (s==L"UTF8")	return ENCODING_UTF8;
		return ENCODING_NOCONVERSION;
	}

	/// �{�f�B�� CMixiData ���̂ǂ̍��ڂ�\�����邩�̎��ʎq
	enum BODY_INDICATE_TYPE
	{
		BODY_INDICATE_TYPE_DATE,		///< ���t��\������
		BODY_INDICATE_TYPE_TITLE,		///< �^�C�g����\������
		BODY_INDICATE_TYPE_NAME,		///< ���O��\������
		BODY_INDICATE_TYPE_BODY,		///< �{����\������
		BODY_INDICATE_TYPE_NONE,		///< �����\�����Ȃ�
	};
	LPCTSTR body_indicate_type_to_text(BODY_INDICATE_TYPE e) {
		switch (e) {
		case BODY_INDICATE_TYPE_DATE:	return L"���t";
		case BODY_INDICATE_TYPE_TITLE:	return L"�^�C�g��";
		case BODY_INDICATE_TYPE_NAME:	return L"���O";
		case BODY_INDICATE_TYPE_BODY:	return L"�{��";
		case BODY_INDICATE_TYPE_NONE:
		default:						return L"";
		}
	}

	/// �{�f�B�̃J�����p�f�[�^
	class BodyHeaderColumn {
	public:
		BODY_INDICATE_TYPE type;		///< �ǂ̍��ڂ�\�����邩
		std::wstring	   title;		///< �J�����ɕ\�����镶����

		/// �R���X�g���N�^
		BodyHeaderColumn() : type(BODY_INDICATE_TYPE_NONE), title(L"") {}
		/// �R���X�g���N�^
		BodyHeaderColumn(BODY_INDICATE_TYPE a_type, const wchar_t* a_title) : type(a_type), title(a_title) {}
	};

	/// �e�A�N�Z�X��ʂ̐U�镑�����`����f�[�^�\��
	class Data
	{
	public:
		INFO_TYPE			infoType;			///< �f�[�^���̂̎�ʁi���ɗ��p�����A�N�Z�X��ʂ��H�j
		std::string			serviceType;		///< �ΏۂƂ���T�[�r�X�̎�ʁi"mixi", "Twitter"�Ȃǁj
		std::wstring		shortText;			///< ����������
		REQUEST_METHOD		requestMethod;		///< ���N�G�X�g���

		ENCODING			requestEncoding;	///< �擾���̃G���R�[�f�B���O�^�C�v

		std::wstring		cacheFilePattern;	///< �L���b�V���t�@�C�����̃p�^�[���B
												///< MakeLogfilePath �ɂ��A���L�̃p�^�[�����g�p�\�B
												///< "{urlparam:�p�����[�^��}"
												///< "{urlafter:TargetURL[:default_path]}"

		//--- �O���[�v�n�A�J�e�S���n�݂̂�������
		std::string			serializeKey;		///< Mz3GroupData �� ini �t�@�C���ɃV���A���C�Y����ۂ̃L�[

		//--- �J�e�S���n�݂̂�������
		bool				bCruiseTarget;		///< ����ΏۂƂ��邩�H
		std::wstring		defaultCategoryURL;	///< �J�e�S����URL
		BodyHeaderColumn	bodyHeaderCol1;		///< �{�f�B���X�g�̃w�b�_�[1�̃J����
		BodyHeaderColumn	bodyHeaderCol2;		///< �{�f�B���X�g�̃w�b�_�[2�̃J����
		BodyHeaderColumn	bodyHeaderCol3;		///< �{�f�B���X�g�̃w�b�_�[3�̃J����
		std::wstring		bodyIntegratedLinePattern1;		///< �����J�������[�h�A1�s�ڂ̃p�^�[��(%n �ŃJ�����v�f���w�肷��)
		std::wstring		bodyIntegratedLinePattern2;		///< �����J�������[�h�A2�s�ڂ̃p�^�[��(%n �ŃJ�����v�f���w�肷��)

//		std::wstring		refererUrlPattern;	///< ���t�@��URL�̃p�^�[���B

		Data(INFO_TYPE a_infoType, const char* a_serviceType, const wchar_t* a_shortText, REQUEST_METHOD a_requestMethod)
			: infoType(a_infoType)
			, serviceType(a_serviceType)
			, shortText(a_shortText)
			, requestMethod(a_requestMethod)
			, requestEncoding(ENCODING_EUC)
			, cacheFilePattern(L"")
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyIntegratedLinePattern1(L"%1")
			, bodyIntegratedLinePattern2(L"%2 | %3")
//			, refererUrlPattern(L"")
		{}
		Data()
			: infoType(INFO_TYPE_INVALID)
			, serviceType("")
			, shortText(L"<unknown>")
			, requestMethod(REQUEST_METHOD_INVALID)
			, requestEncoding(ENCODING_EUC)
			, cacheFilePattern(L"")
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyIntegratedLinePattern1(L"%1")
			, bodyIntegratedLinePattern2(L"%2 | %3")
//			, refererUrlPattern(L"")
		{}
	};

	// �A�N�Z�X��� �� �f�[�^�}�b�v
	typedef std::map<ACCESS_TYPE, Data> ACCESS_TYPE_TO_DATA_MAP;
	ACCESS_TYPE_TO_DATA_MAP m_map;

	// �V���A���C�Y�L�[ �� �A�N�Z�X��ʃ}�b�v
	typedef std::map<std::string, ACCESS_TYPE> SERIALIZE_KEY_TO_ACCESS_TYPE_MAP;
	SERIALIZE_KEY_TO_ACCESS_TYPE_MAP m_serializeKeyToAccessKeyMap;

public:
	bool init();

	/// �V���A���C�Y�L�[����A�N�Z�X��ʂ��擾����
	ACCESS_TYPE getAccessTypeBySerializeKey(const std::string& key) {
		SERIALIZE_KEY_TO_ACCESS_TYPE_MAP::iterator it = m_serializeKeyToAccessKeyMap.find(key);
		if (it==m_serializeKeyToAccessKeyMap.end()) {
			return ACCESS_INVALID;
		}
		return it->second;
	}

	/// �J�e�S���n��ʈꗗ�̐���
	std::vector<ACCESS_TYPE> getCategoryTypeList() {
		std::vector<ACCESS_TYPE> types;
		for (ACCESS_TYPE_TO_DATA_MAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
			if (it->second.infoType==INFO_TYPE_CATEGORY) {
				types.push_back(it->first);
			}
		}
		return types;
	}

	/// �f�[�^���
	INFO_TYPE getInfoType( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return INFO_TYPE_INVALID;
		}
		return it->second.infoType;
	}

	/// �T�[�r�X��ʂ̎擾
	std::string getServiceType( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serviceType.c_str();
	}

	/// ���N�G�X�g��ʂ̎擾
	REQUEST_METHOD getRequestMethod( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return REQUEST_METHOD_INVALID;
		}
		return it->second.requestMethod;
	}

	/// ����������̎擾
	const wchar_t* getShortText( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"<unknown>";
		}
		return it->second.shortText.c_str();
	}

	/// �V���A���C�Y�L�[�̎擾
	const char* getSerializeKey( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serializeKey.c_str();
	}

	/// ����ΏۂƂ��邩�H�i����\��\���H�j
	const bool isCruiseTarget( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return false;
		}
		return it->second.bCruiseTarget;
	}

	/// �{�f�B���X�g�̃w�b�_�[1�̃J������
	const wchar_t* getBodyHeaderCol1Name( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol1.title.c_str();
	}

	/// �{�f�B���X�g�̃w�b�_�[2�̃J������
	const wchar_t* getBodyHeaderCol2Name( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2.title.c_str();
	}

	/// �{�f�B���X�g�̃w�b�_�[3�̃J������
	const wchar_t* getBodyHeaderCol3Name( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol3.title.c_str();
	}

	/// �{�f�B���X�g�̃w�b�_�[1�̃J�������
	BODY_INDICATE_TYPE getBodyHeaderCol1Type( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol1.type;
	}

	/// �{�f�B���X�g�̃w�b�_�[2�̃J�������
	BODY_INDICATE_TYPE getBodyHeaderCol2Type( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2.type;
	}

	/// �{�f�B���X�g�̃w�b�_�[3�̃J�������
	BODY_INDICATE_TYPE getBodyHeaderCol3Type( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol3.type;
	}

	/// �����J�������[�h�A1�s�ڂ̃p�^�[��
	const wchar_t* getBodyIntegratedLinePattern1( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyIntegratedLinePattern1.c_str();
	}

	/// �����J�������[�h�A2�s�ڂ̃p�^�[��
	const wchar_t* getBodyIntegratedLinePattern2( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyIntegratedLinePattern2.c_str();
	}

	/// �f�t�H���gURL
	const wchar_t* getDefaultCategoryURL( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.defaultCategoryURL.c_str();
	}

	/// �G���R�[�f�B���O
	ENCODING getRequestEncoding( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return ENCODING_EUC;
		}
		return it->second.requestEncoding;
	}

	/// �L���b�V���t�@�C�����̃p�^�[��
	const wchar_t* getCacheFilePattern( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.cacheFilePattern.c_str();
	}

	/// referer�̎擾
/*	const wchar_t* getRefererUrlPattern( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return NULL;
		}
		return it->second.refererUrl.c_str();
	}
*/
};
