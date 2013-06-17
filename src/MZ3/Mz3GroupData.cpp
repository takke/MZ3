/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#include "stdafx.h"
#include "MZ3.h"
#include "Mz3GroupData.h"
#include "util_base.h"
#include "util_mz3.h"
#include "inifile.h"

/**
 * �g�b�v�y�[�W�p�̏�����
 *
 * �O���[�v���X�g�Ƃ��̔z���̃��X�g�̐����B
 */
bool Mz3GroupData::initForTopPage(AccessTypeInfo& accessTypeInfo, const InitializeType initType)
{
	this->groups.clear();
	this->services = initType.strSelectedServices;

	// �T�[�r�X����̏ꍇ�͑S�I���Ƃ݂Ȃ��S�T�[�r�X��ǉ�����
	if (this->services.empty()) {
		for (size_t idx=0; idx<theApp.m_luaServices.size(); idx++) {
			this->services += " ";
			this->services += theApp.m_luaServices[idx].name;
		}
	}

	// �C�x���g�n���h���̌Ăяo��
	util::CallMZ3ScriptHookFunctions("", "creating_default_group", this);

	return true;
}

/**
 * �J�e�S��URL�̈ڍs����
 *
 * �J�e�S�������ύX�\(��:"�݂�Ȃ�echo" �� "�݂�Ȃ̃{�C�X")
 */
inline CString migrateCategoryUrl(LPCTSTR default_category_url,
										CStringA category_url,
										ACCESS_TYPE category_type,
										std::string& category_name)
{

#ifdef BT_MZ3
	// ���`��ini�t�@�C���̈ڍs����
/*
	if (category_type == ACCESS_LIST_FOOTSTEP && category_url == "show_log.pl") {
		// ����������URL��API�pURL�ɒu��(�f�t�H���gURL���̗p����)
		return default_category_url;
	}
*/
	/* ���L�́u�}�C�~�N�ꗗ�v�� "list_friend.pl" ���� API �Ɉڍs�����Ƃ��̏���
	if (category_type == ACCESS_LIST_FRIEND && category_url == "list_friend.pl") {
		// ���}�C�~�N�ꗗURL��API�pURL�ɒu��(�f�t�H���gURL���̗p����)
		return default_category_url;
	}
	*/

	if (category_type == ACCESS_LIST_FRIEND) {
		// �}�C�~�N�ꗗURL�̓f�t�H���gURL��������("list_friend.pl" �� API �� URL �͔p�~�Ƃ���)
		return default_category_url;
	}

	if (category_type == ACCESS_MIXI_RECENT_VOICE &&
		category_url.Find("recent_echo.pl")>=0)
	{
		// �ڍs�����Frecent_echo.pl �� recent_voice.pl �ɏ���������
		CString strCategoryUrl(category_url);
		strCategoryUrl.Replace(L"recent_echo.pl",
							   L"recent_voice.pl");
		category_name = "�݂�Ȃ̃{�C�X";
		return strCategoryUrl;
	}
	
	if (category_type == ACCESS_MIXI_RECENT_VOICE &&
		category_url.Find("res_echo.pl")>=0 &&
		strstr(category_name.c_str(), "�ԐM")!=NULL)
	{
		// �ڍs�����Fres_voice.pl �� 2010/04/14 ��mixi�d�l�ύX�ŏ������̂ō��ڂƂ��Ă��폜
		return L"";
	}
#endif

	// MZ3 API : �J�e�S��URL��migrate
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(""));	// URL
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(category_type));
	if (util::CallMZ3ScriptHookFunctions2("migrate_category_url", &rvals, util::MyLuaData(serializeKey), util::MyLuaData(category_url))) {
		if (rvals.size() == 1) {
			return CString(rvals[0].m_strText);
		}
	}

	return CString(category_url);
}


/**
 * group �ɃJ�e�S����ǉ�����B
 *
 * category_url �� NULL �܂��͋�̏ꍇ�́A�J�e�S����ʂɉ������f�t�H���gURL���w�肷��B
 * �J�e�S����ʂɉ������J�����^�C�v���w�肷��B
 */
bool Mz3GroupData::appendCategoryByIniData( 
	AccessTypeInfo& accessTypeInfo, 
	CGroupItem& group, std::string category_name, ACCESS_TYPE category_type, const char* category_url, bool bCruise )
{
	// �f�t�H���gURL�icategory_url �����w��̏ꍇ�ɗp����URL�j
	LPCTSTR default_category_url = accessTypeInfo.getDefaultCategoryURL(category_type);

	AccessTypeInfo::BODY_INDICATE_TYPE colType1 = accessTypeInfo.getBodyHeaderCol1Type(category_type);
	AccessTypeInfo::BODY_INDICATE_TYPE colType2 = accessTypeInfo.getBodyHeaderCol2Type(category_type);
	AccessTypeInfo::BODY_INDICATE_TYPE colType3 = accessTypeInfo.getBodyHeaderCol3Type(category_type);

	if (colType1==AccessTypeInfo::BODY_INDICATE_TYPE_NONE || 
		colType2==AccessTypeInfo::BODY_INDICATE_TYPE_NONE) 
	{
		// �T�|�[�g�O�̂��ߒǉ������I��
//		MZ3LOGGER_ERROR(L"�T�|�[�g�O�̂��ߒǉ����܂���B");
		return false;
	}

	// �A�N�Z�X��ʕ����ɔ����ڍs����
#ifdef BT_MZ3
	switch( category_type ) {
	case ACCESS_LIST_FAVORITE_USER:
		// ���C�ɓ��胆�[�U
		if (category_url != NULL && strstr(category_url, "kind=community")!=NULL) {
			// �u���C�ɓ���R�~���v�Ƃ��ď�������
			category_type = ACCESS_LIST_FAVORITE_COMMUNITY;
		}
		break;
	}
#endif

	// URL ���w�肳��Ă���΂��� URL ��p����B
	// �A���A�������̌Â�URL�ɂ��Ă͈ڍs�������s���B
	CString url = default_category_url;
	if (category_url!=NULL && strlen(category_url) > 0) {

		// �J�e�S��URL�̈ڍs����
		url = migrateCategoryUrl(default_category_url, category_url, category_type, category_name);
		if (url.IsEmpty()) {
			return true;
		}
	}

	static CCategoryItem item;
	item.init( 
		util::my_mbstowcs(category_name).c_str(), 
		url, 
		category_type, 
		group.categories.size(), 
		colType1, colType2, colType3 );
	item.m_bCruise = bCruise;
	group.categories.push_back( item );

	return true;
}

/**
 * ���L�̃}�b�v�𐶐�����B
 *
 * �J�e�S����ʕ����� ���� �J�e�S�����
 */
void Mz3GroupDataInifileHelper::InitMap(AccessTypeInfo& accessTypeInfo) {
	category_string2type.RemoveAll();
	category_string2type.InitHashTable( 20 );

	// AccessTypeInfo �̃V���A���C�Y�L�[����e��n�b�V���e�[�u�����\�z����
	AccessTypeInfo::ACCESS_TYPE_TO_DATA_MAP::iterator it;
	for (it=accessTypeInfo.m_map.begin(); it!=accessTypeInfo.m_map.end(); it++) {
		ACCESS_TYPE accessType = it->first;
		AccessTypeInfo::Data& data = it->second;

		switch (data.infoType) {
		case AccessTypeInfo::INFO_TYPE_CATEGORY:
			// category_string2type �̍\�z
			if (!data.serializeKey.empty()) {
				category_string2type[ util::my_mbstowcs(data.serializeKey.c_str()).c_str() ] = accessType;
			}
			break;

		default:
			// nothing to do.
			break;
		}
	}
}

/**
 * ini �t�@�C���i�O���[�v��`�t�@�C���j���� Mz3GroupData �𐶐�����B
 */
bool Mz3GroupDataReader::load( AccessTypeInfo& accessTypeInfo, Mz3GroupData& target, const CString& inifilename )
{
	target.groups.clear();

	inifile::IniFile inifile;
	Mz3GroupDataInifileHelper helper(accessTypeInfo);

	// �O���[�v��`�t�@�C���̃��[�h
	if(! inifile.Load( inifilename ) ) {
		return false;
	}

	// �\�[�g�ς݃Z�N�V�������X�g���擾����
	std::vector<std::string> group_section_names;
	{
		// �Z�N�V�����ꗗ���擾����B
		group_section_names = inifile.GetSectionNames();

		// �Z�N�V�����ꗗ�̂����uGroup�v�Ŏn�܂���̂𒊏o����B
		for( size_t i=0; i<group_section_names.size(); ) {
			if( strncmp( group_section_names[i].c_str(), "Group", 5 ) != 0 ) {
				// �uGroup�v�Ŏn�܂�Ȃ��̂ŏ�������B
				group_section_names.erase( group_section_names.begin()+i );
			}else{
				i++;
			}
		}

		// �\�[�g
		sort( group_section_names.begin(), group_section_names.end() );
	}

	// �e�Z�N�V�����ɑ΂��Ēǉ����������s
	for( size_t i=0; i<group_section_names.size(); i++ ) {
		const char* section_name = group_section_names[i].c_str();

		// "Name" �̒l���擾���A�O���[�v���Ƃ���B
		std::string group_name = inifile.GetValue( "Name", section_name );
		if( group_name.empty() ) {
			continue;
		}

		// "Type" �̒l���擾���A�O���[�v��ʂƂ���B
		// ���O���[�v�̎�ʂ͔p�~�B�S�� GROUP_GENERAL �Ƃ���B
		ACCESS_TYPE group_type = ACCESS_GROUP_GENERAL;

		// "Url" �̒l���擾���AURL�Ƃ���B
		std::string url = inifile.GetValue( "Url", section_name );

		// �O���[�v�쐬
		CGroupItem group;
		group.init( util::my_mbstowcs(group_name).c_str(), util::my_mbstowcs(url).c_str(), group_type );

		// "CategoryXX" �̒l���擾���A��������BXX �� [01,99] �Ƃ���B
		for( int j=0; j<99; j++ ) {
			CStringA key;
			key.Format( "Category%02d", j+1 );
			
			// "CategoryXX" �̒l�����݂��Ȃ���ΏI������B
			if(! inifile.RecordExists( (const char*)key, section_name ) ) {
				break;
			}

			// "CategoryXX" �̒l���擾����B
			std::string value = inifile.GetValue( (const char*)key, section_name );

			// �J���}��؂�ŕ����񃊃X�g������B
			std::vector<std::string> values;
			util::split_by_comma( values, value );
			if( values.size() < 2 ) {
				continue;
			}

			// �����񃊃X�g�̑�1�v�f���J�e�S�����Ƃ���B
			std::string& category_name = values[0];

			// �����񃊃X�g�̑�2�v�f���J�e�S����ʂƂ���B
			// �J�e�S����ʖ����J�e�S����ʕϊ����s���B
			ACCESS_TYPE category_type = helper.CategoryString2Type( util::my_mbstowcs(values[1]).c_str() );
			if( category_type == ACCESS_INVALID ) {
				continue;
			}

			// �����񃊃X�g�̑�3�v�f��URL�Ƃ���B
			// ���݂��Ȃ��ꍇ�̓J�e�S����ʂ̃f�t�H���g�l��p����B
			const char* category_url = NULL;
			if( values.size() >= 3 ) {
				category_url = values[2].c_str();
			}

			// �����񃊃X�g�̑�4�v�f������t���O�Ƃ���B
			// ���݂��Ȃ��ꍇ�͏���Ȃ��Ƃ���B
			bool bCruise = false;
			if( values.size() >= 4 ) {
				bCruise = atoi(values[3].c_str()) != 0 ? true : false;
			}

			// �J�e�S���ǉ��B
			target.appendCategoryByIniData( accessTypeInfo, group, category_name, category_type, category_url, bCruise );
		}

		// �J�e�S�����P�ȏ�Ȃ�Agroups �ɒǉ��B
		if( group.categories.size() >= 1 ) {
			target.groups.push_back( group );
		}
	}

	// �O���[�v�v�f���[���Ȃ�I���B
	if( target.groups.empty() ) {
		return false;
	}

	return true;
}

/**
 * Mz3GroupData �I�u�W�F�N�g�� ini �t�@�C���i�O���[�v��`�t�@�C���j������B
 */
bool Mz3GroupDataWriter::save( AccessTypeInfo& accessTypeInfo, const Mz3GroupData& target, const CString& inifilename )
{
	inifile::IniFile inifile;

	const int n = target.groups.size();
	for( int i=0; i<n; i++ ) {
		const CGroupItem& group = target.groups[i];

		if (!group.bSaveToGroupFile) {
			continue;
		}

		// �Z�N�V����������
		CStringA strSectionName;
		strSectionName.Format( "Group%02d", i+1 );

		// Name �o��
		inifile.SetValue( L"Name", group.name, strSectionName );

		// Type �o��(��� GROUP_GENERAL �Ƃ���)
		inifile.SetValue( "Type", accessTypeInfo.getSerializeKey(ACCESS_GROUP_GENERAL), (LPCSTR)strSectionName );

		// Url �o��
		inifile.SetValue( L"Url", group.mixi.GetURL(), strSectionName );

		// CategoryXX �o��
		int nc = group.categories.size();
		int categoryNumber = 1;
		for( int j=0; j<nc; j++ ) {
			// �L�[����
			CString key;
			key.Format( L"Category%02d", categoryNumber );

			// �E�Ӓl����
			// �E�Ӓl�͏��ɁA�u�J�e�S�����́v�A�u�J�e�S����ʕ�����v�A�uURL�v
			const CCategoryItem& item = group.categories[j];
			CString categoryString = util::my_mbstowcs(accessTypeInfo.getSerializeKey(item.m_mixi.GetAccessType())).c_str();
			if (categoryString.IsEmpty() || item.bSaveToGroupFile==false) 
				continue;

			CString value;
			value.Format( L"%s,%s,%s,%d", 
				item.m_name, 
				categoryString,
				item.m_mixi.GetURL(),
				item.m_bCruise ? 1 : 0
				);

			// �o��
			inifile.SetValue( key, value, strSectionName );
			categoryNumber ++;
		}
	}

	// �ۑ�
	return inifile.Save( inifilename, false );
}
