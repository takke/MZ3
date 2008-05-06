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
	ACCESS_TYPE type;

	//------------------------------------------------------------------
	//--- �O���[�v�n
	type = ACCESS_GROUP_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"�R�~���j�e�BG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "COMMUNITY";

	type = ACCESS_GROUP_MESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"���b�Z�[�WG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MESSAGE";

	type = ACCESS_GROUP_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"���LG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MYDIARY";

	type = ACCESS_GROUP_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"�j���[�XG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "NEWS";

	type = ACCESS_GROUP_OTHERS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"���̑�G"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "OTHERS";

	type = ACCESS_GROUP_TWITTER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"TwitterG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "TWITTER";

	//------------------------------------------------------------------
	//--- mixi,���X�g�n
	type = ACCESS_LIST_INTRO;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�Љ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "INTRO";
	m_map[type].defaultCategoryURL = L"show_intro.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"�Љ");

	type = ACCESS_LIST_DIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "DIARY";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_friend_diary.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_NEW_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�V���R�����g�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "NEW_COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�����g�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�j���[�X�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "NEWS";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"���o��");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"�z�M����>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�z����>>");

	type = ACCESS_LIST_FAVORITE_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���C�ɓ��胆�[�U"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "FAVORITE";
	m_map[type].defaultCategoryURL = L"list_bookmark.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�ŏI���O�C��>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"���ȏЉ�>>");

	type = ACCESS_LIST_FAVORITE_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���C�ɓ���R�~��"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "FAVORITE_COMMUNITY";
	m_map[type].defaultCategoryURL = L"list_bookmark.pl?kind=community";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"�R�~���j�e�B");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�l��>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����>>");

	type = ACCESS_LIST_FRIEND;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�}�C�~�N�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "FRIEND";
	m_map[type].defaultCategoryURL = L"list_friend.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���O�C������");

	type = ACCESS_LIST_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�~���j�e�B�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "COMMUNITY";
	m_map[type].defaultCategoryURL = L"list_community.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"�R�~���j�e�B");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�l��");

	type = ACCESS_LIST_NEW_BBS_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�~���R�����g�L������"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "NEW_BBS_COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_NEW_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�~�������ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "BBS";	// �{���� "NEW_BBS" ����
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�g�s�b�N�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "TOPIC";	// �{���� "BBS" ����
	m_map[type].bCruiseTarget = true;
	// ���I�ɐ�������邽�߃f�t�H���gURL�͕s�v
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t");

	type = ACCESS_LIST_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MYDIARY";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_diary.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����");

	type = ACCESS_LIST_FOOTSTEP;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"������"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// mixi API => UTF-8
	m_map[type].serializeKey = "FOOTSTEP";
	m_map[type].defaultCategoryURL = L"http://mixi.jp/atom/tracks/r=2/member_id={owner_id}";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"����");

	type = ACCESS_LIST_MESSAGE_IN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���b�Z�[�W(��M��)"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MESSAGE_IN";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"����");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���o�l>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t>>");

	type = ACCESS_LIST_MESSAGE_OUT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���b�Z�[�W(���M��)"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MESSAGE_OUT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl?box=outbox";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"����");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"����>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t>>");

	type = ACCESS_LIST_BOOKMARK;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�u�b�N�}�[�N"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].serializeKey = "BOOKMARK";
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B");

	type = ACCESS_LIST_CALENDAR;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�J�����_�["
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "CALENDAR";
	m_map[type].defaultCategoryURL = L"show_calendar.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t");

	//------------------------------------------------------------------
	//--- mixi,���
	m_map[ACCESS_LOGIN] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"���O�C��"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MAIN] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"���C��"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_DIARY] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"���L���e"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_NEWS] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�j���[�X"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_BBS] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�R�~������"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_ENQUETE] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�A���P�[�g"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_EVENT] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�C�x���g"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_EVENT_MEMBER] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// v0.9.1 ���_�ł�ReportView��[�y�[�W]����̑J�ڂ̂݃T�|�[�g
		, "mixi"
		, L"�C�x���g�Q���҈ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MYDIARY] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"���L"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MESSAGE] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_IMAGE] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"�摜"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MOVIE] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"����"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_DOWNLOAD] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"�_�E�����[�h"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_PROFILE] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�v���t�B�[��"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_COMMUNITY] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// �{���̓R�~���j�e�B�g�b�v�y�[�W�Bv0.9.1 ���_�ł�CMZ3View���Ńg�s�b�N�ꗗ�ɐ؂�ւ��Ă���
		, "mixi"
		, L"�R�~���j�e�B"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_PLAIN] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�ėpURL"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- mixi,POST �n
	m_map[ACCESS_POST_CONFIRM_COMMENT] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_COMMENT] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_REPLYMESSAGE] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���b�Z�[�W�ԐM�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_REPLYMESSAGE] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���b�Z�[�W�ԐM�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_NEWMESSAGE] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�V�K���b�Z�[�W�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_NEWMESSAGE] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�V�K���b�Z�[�W�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_NEWDIARY] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���L���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_NEWDIARY] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- Twitter �n
	type = ACCESS_TWITTER_FRIENDS_TIMELINE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"�^�C�����C��"
		, REQUEST_METHOD_POST// �^�C�����C���擾��POST�ɂ��Ă݂�
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_FRIENDS_TIMELINE";
	m_map[type].defaultCategoryURL = L"http://twitter.com/statuses/friends_timeline.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");

	type = ACCESS_TWITTER_FAVORITES;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"���C�ɓ���"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_FAVORITES";
	m_map[type].defaultCategoryURL = L"http://twitter.com/favorites.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");

	type = ACCESS_TWITTER_DIRECT_MESSAGES;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_DIRECT_MESSAGES";
	m_map[type].defaultCategoryURL = L"http://twitter.com/direct_messages.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"���b�Z�[�W");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");

	type = ACCESS_TWITTER_UPDATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"�X�V"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "Twitter"
		, L"Twitter����"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_NEW_DM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"���b�Z�[�W���M"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FAVOURINGS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"���C�ɓ���o�^"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FAVOURINGS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"���C�ɓ���폜"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FRIENDSHIPS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"�t�H���[�o�^"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FRIENDSHIPS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"�t�H���[����"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	//------------------------------------------------------------------
	//--- RSS Reader
	type = ACCESS_RSS_READER_FEED;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "RSS"
		, L"RSS Feed"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;
	m_map[type].serializeKey = "RSS_FEED";
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2A = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY,  L"���e>>");
	m_map[type].bodyHeaderCol2B = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t>>");

	type = ACCESS_RSS_READER_ITEM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "RSS"
		, L"RSS Item"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].requestEncoding = ENCODING_UTF8;

	//------------------------------------------------------------------
	//--- ���̑�
	m_map[ACCESS_INVALID] = AccessTypeInfo::Data(
		INFO_TYPE_INVALID
		, ""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		);

	// TODO �K�{���ڂ̃e�X�g

	return true;
}