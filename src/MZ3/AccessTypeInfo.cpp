/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "AccessTypeInfo.h"

/// ������
bool AccessTypeInfo::init()
{
	//------------------------------------------------------------------
	//--- �O���[�v�n
	m_map[ACCESS_GROUP_COMMUNITY] = AccessTypeInfo::Data(
		""
		, L"�R�~���j�e�BG"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_MESSAGE] = AccessTypeInfo::Data(
		""
		, L"���b�Z�[�WG"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_MYDIARY] = AccessTypeInfo::Data(
		""
		, L"���LG"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_NEWS] = AccessTypeInfo::Data(
		""
		, L"�j���[�XG"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_OTHERS] = AccessTypeInfo::Data(
		""
		, L"���̑�G"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_TWITTER] = AccessTypeInfo::Data(
		""
		, L"TwitterG"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- mixi,���X�g�n
	ACCESS_TYPE type;
	
	type = ACCESS_LIST_INTRO;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�Љ"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"show_intro.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol1Name  = L"���O";
	m_map[type].bodyHeaderCol2NameA = L"�Љ";

	type = ACCESS_LIST_DIARY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_friend_diary.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�^�C�g��";
	m_map[type].bodyHeaderCol2NameA = L"���O>>";
	m_map[type].bodyHeaderCol2NameB = L"����>>";

	type = ACCESS_LIST_NEW_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�V���R�����g�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_comment.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�^�C�g��";
	m_map[type].bodyHeaderCol2NameA = L"���O>>";
	m_map[type].bodyHeaderCol2NameB = L"����>>";

	type = ACCESS_LIST_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_comment.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�^�C�g��";
	m_map[type].bodyHeaderCol2NameA = L"���O>>";
	m_map[type].bodyHeaderCol2NameB = L"����>>";

	type = ACCESS_LIST_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�j���[�X�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol1Name  = L"���o��";
	m_map[type].bodyHeaderCol2NameA = L"�z�M����>>";
	m_map[type].bodyHeaderCol2NameB = L"�z����>>";

	type = ACCESS_LIST_FAVORITE_USER;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"���C�ɓ��胆�[�U"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_bookmark.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol1Name  = L"���O";
	m_map[type].bodyHeaderCol2NameA = L"�ŏI���O�C��>>";
	m_map[type].bodyHeaderCol2NameB = L"���ȏЉ�>>";

	type = ACCESS_LIST_FAVORITE_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"���C�ɓ���R�~��"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_bookmark.pl?kind=community";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol1Name  = L"�R�~���j�e�B";
	m_map[type].bodyHeaderCol2NameA = L"�l��>>";
	m_map[type].bodyHeaderCol2NameB = L"����>>";

	type = ACCESS_LIST_FRIEND;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�}�C�~�N�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_friend.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"���O";
	m_map[type].bodyHeaderCol2NameA = L"���O�C������";

	type = ACCESS_LIST_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~���j�e�B�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_community.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�R�~���j�e�B";
	m_map[type].bodyHeaderCol2NameA = L"�l��";

	type = ACCESS_LIST_NEW_BBS_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~���R�����g�L������"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs_comment.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�g�s�b�N";
	m_map[type].bodyHeaderCol2NameA = L"�R�~���j�e�B>>";
	m_map[type].bodyHeaderCol2NameB = L"����>>";

	type = ACCESS_LIST_NEW_BBS;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~�������ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�g�s�b�N";
	m_map[type].bodyHeaderCol2NameA = L"�R�~���j�e�B>>";
	m_map[type].bodyHeaderCol2NameB = L"����>>";

	type = ACCESS_LIST_BBS;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�g�s�b�N�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	// ���I�ɐ�������邽�߃f�t�H���gURL�͕s�v
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�g�s�b�N";
	m_map[type].bodyHeaderCol2NameA = L"���t";

	type = ACCESS_LIST_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_diary.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�^�C�g��";
	m_map[type].bodyHeaderCol2NameA = L"����";

	type = ACCESS_LIST_FOOTSTEP;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"������"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"http://mixi.jp/atom/tracks/r=2/member_id={owner_id}";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"���O";
	m_map[type].bodyHeaderCol2NameA = L"����";

	type = ACCESS_LIST_MESSAGE_IN;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W(��M��)"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"����";
	m_map[type].bodyHeaderCol2NameA = L"���o�l>>";
	m_map[type].bodyHeaderCol2NameB = L"���t>>";

	type = ACCESS_LIST_MESSAGE_OUT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W(���M��)"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl?box=outbox";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"����";
	m_map[type].bodyHeaderCol2NameA = L"����>>";
	m_map[type].bodyHeaderCol2NameB = L"���t>>";

	type = ACCESS_LIST_BOOKMARK;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�u�b�N�}�[�N"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol1Name  = L"�g�s�b�N";
	m_map[type].bodyHeaderCol2NameA = L"�R�~���j�e�B";

	type = ACCESS_LIST_CALENDAR;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"�J�����_�["
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"show_calendar.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"�^�C�g��";
	m_map[type].bodyHeaderCol2NameA = L"���t";

	//------------------------------------------------------------------
	//--- mixi,���
	m_map[ACCESS_LOGIN] = AccessTypeInfo::Data(
		"mixi"
		, L"���O�C��"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"���C��"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_DIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L���e"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_NEWS] = AccessTypeInfo::Data(
		"mixi"
		, L"�j���[�X"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~������"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_ENQUETE] = AccessTypeInfo::Data(
		"mixi"
		, L"�A���P�[�g"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_EVENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�C�x���g"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_EVENT_MEMBER] = AccessTypeInfo::Data(
		"mixi"
		, L"�C�x���g�Q���҈ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MYDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_ADDDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L���e"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_IMAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"�摜"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MOVIE] = AccessTypeInfo::Data(
		"mixi"
		, L"����"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_DOWNLOAD] = AccessTypeInfo::Data(
		"mixi"
		, L"�_�E�����[�h"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_PROFILE] = AccessTypeInfo::Data(
		"mixi"
		, L"�v���t�B�[��"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_COMMUNITY] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~���j�e�B"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_PLAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"�ėpURL"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- mixi,POST �n
	m_map[ACCESS_POST_CONFIRM_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W�ԐM�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W�ԐM�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"�V�K���b�Z�[�W�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"�V�K���b�Z�[�W�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- Twitter �n
	type = ACCESS_TWITTER_FRIENDS_TIMELINE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"�^�C�����C��"
		, REQUEST_METHOD_POST// �^�C�����C���擾��POST�ɂ��Ă݂�
		);
	m_map[type].defaultCategoryURL = L"http://twitter.com/statuses/friends_timeline.xml";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"����";
	m_map[type].bodyHeaderCol2NameA = L"���O>>";
	m_map[type].bodyHeaderCol2NameB = L"���t>>";

	type = ACCESS_TWITTER_FAVORITES;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"���C�ɓ���"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"http://twitter.com/favorites.xml";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"����";
	m_map[type].bodyHeaderCol2NameA = L"���O>>";
	m_map[type].bodyHeaderCol2NameB = L"���t>>";

	type = ACCESS_TWITTER_DIRECT_MESSAGES;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"http://twitter.com/direct_messages.xml";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"���b�Z�[�W";
	m_map[type].bodyHeaderCol2NameA = L"���O>>";
	m_map[type].bodyHeaderCol2NameB = L"���t>>";

	type = ACCESS_TWITTER_UPDATE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"�X�V"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_TWITTER_USER;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"Twitter����"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_TWITTER_NEW_DM;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"���b�Z�[�W���M"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_TWITTER_FAVOURINGS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"���C�ɓ���o�^"
		, REQUEST_METHOD_POST
		);

	type = ACCESS_TWITTER_FAVOURINGS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"���C�ɓ���폜"
		, REQUEST_METHOD_POST
		);

	type = ACCESS_TWITTER_FRIENDSHIPS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"�t�H���[�o�^"
		, REQUEST_METHOD_POST
		);

	type = ACCESS_TWITTER_FRIENDSHIPS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"�t�H���[����"
		, REQUEST_METHOD_POST
		);

	//------------------------------------------------------------------
	//--- ���̑�
	m_map[ACCESS_INVALID] = AccessTypeInfo::Data(
		""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		);

	return true;
}