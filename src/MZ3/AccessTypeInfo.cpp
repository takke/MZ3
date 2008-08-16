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
	// url(1) : http://mixi.jp/show_intro.pl
	// url(2) : http://mixi.jp/show_intro.pl?page=2
	m_map[type].cacheFilePattern = L"show_intro{urlparam:page}.html";
	m_map[type].serializeKey = "INTRO";
	m_map[type].defaultCategoryURL = L"show_intro.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"�Љ");

	type = ACCESS_LIST_DIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"new_friend_diary.html";
	m_map[type].serializeKey = "DIARY";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_friend_diary.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_NEW_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�V���R�����g�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"new_comment.html";
	m_map[type].serializeKey = "NEW_COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�����g�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_comment.html";
	m_map[type].serializeKey = "COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�j���[�X�ꗗ"
		, REQUEST_METHOD_GET
		);
	// �J�e�S���A�y�[�W�ԍ����ɕ���
	// url : http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
	// url : http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1
	// url : http://news.mixi.jp/list_news_category.pl?page=2&id=pickup&type=bn
	// url : http://news.mixi.jp/list_news_category.pl?page=2&sort=1&id=1&type=bn
	m_map[type].cacheFilePattern = L"list_news_category_{urlparam:id}_{urlparam:page}.html";
	m_map[type].serializeKey = "NEWS";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"���o��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"�z�M����>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�z����>>");

	type = ACCESS_LIST_FAVORITE_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���C�ɓ��胆�[�U"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/list_bookmark.pl
	// url(2) : http://mixi.jp/list_bookmark.pl?page=2&kind=friend
	// url(3) : http://mixi.jp/list_bookmark.pl?kind=community
	// url(4) : http://mixi.jp/list_bookmark.pl?page=2&kind=community
	m_map[type].cacheFilePattern = L"list_bookmark_{urlparam:kind}_{urlparam:page}.html";
	m_map[type].serializeKey = "FAVORITE";
	m_map[type].defaultCategoryURL = L"list_bookmark.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�ŏI���O�C��>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"���ȏЉ�>>");

	type = ACCESS_LIST_FAVORITE_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���C�ɓ���R�~��"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/list_bookmark.pl
	// url(2) : http://mixi.jp/list_bookmark.pl?page=2&kind=friend
	// url(3) : http://mixi.jp/list_bookmark.pl?kind=community
	// url(4) : http://mixi.jp/list_bookmark.pl?page=2&kind=community
	m_map[type].cacheFilePattern = L"list_bookmark_{urlparam:kind}_{urlparam:page}.html";
	m_map[type].serializeKey = "FAVORITE_COMMUNITY";
	m_map[type].defaultCategoryURL = L"list_bookmark.pl?kind=community";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"�R�~���j�e�B");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�l��>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����>>");

	type = ACCESS_LIST_FRIEND;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�}�C�~�N�ꗗ"
		, REQUEST_METHOD_POST
		);
	// url(1) : http://mixi.jp/ajax_friend_setting.pl?type=thumbnail&mode=get_friends&page=1&sort=nickname
	m_map[type].cacheFilePattern = L"ajax_friend_setting{urlparam:page}.html";
	m_map[type].serializeKey = "FRIEND";
	m_map[type].defaultCategoryURL = L"ajax_friend_setting.pl?type=thumbnail&mode=get_friends&page=1&sort=nickname";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���O�C������");
	m_map[type].requestEncoding = ENCODING_UTF8;	// mixi API => UTF-8

	type = ACCESS_LIST_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�~���j�e�B�ꗗ"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/list_community.pl?id=xxx
	// url(2) : http://mixi.jp/list_community.pl?id=xxx&page=2
	m_map[type].cacheFilePattern = L"list_community{urlparam:page}.html";
	m_map[type].serializeKey = "COMMUNITY";
	m_map[type].defaultCategoryURL = L"list_community.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"�R�~���j�e�B");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�l��");

	type = ACCESS_LIST_NEW_BBS_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�~���R�����g�L������"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/new_bbs_comment.pl?id=xxx
	// url(2) : http://mixi.jp/new_bbs_comment.pl?id=xxx&page=2
	m_map[type].cacheFilePattern = L"new_bbs_comment{urlparam:page}.html";
	m_map[type].serializeKey = "NEW_BBS_COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_NEW_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�R�~�������ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"new_bbs_{urlparam:page}.html";
	m_map[type].serializeKey = "BBS";	// �{���� "NEW_BBS" ����
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");

	type = ACCESS_LIST_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�g�s�b�N�ꗗ"
		, REQUEST_METHOD_GET
		);
	// url : list_bbs.pl?id=xxx
	m_map[type].cacheFilePattern = L"list_bbs_{urlparam:id}.html";
	m_map[type].serializeKey = "TOPIC";	// �{���� "BBS" ����
	m_map[type].bCruiseTarget = true;
	// ���I�ɐ�������邽�߃f�t�H���gURL�͕s�v
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t");

	type = ACCESS_LIST_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_diary.html";
	m_map[type].serializeKey = "MYDIARY";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_diary.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����");

	type = ACCESS_LIST_FOOTSTEP;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"������"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"tracks.xml";
	m_map[type].requestEncoding = ENCODING_UTF8;	// mixi API => UTF-8
	m_map[type].serializeKey = "FOOTSTEP";
	m_map[type].defaultCategoryURL = L"http://mixi.jp/atom/tracks/r=2/member_id={owner_id}";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"����");

	type = ACCESS_LIST_MESSAGE_IN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���b�Z�[�W(��M��)"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_message_in.html";
	m_map[type].serializeKey = "MESSAGE_IN";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���o�l>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t>>");

	type = ACCESS_LIST_MESSAGE_OUT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"���b�Z�[�W(���M��)"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_message_out.html";
	m_map[type].serializeKey = "MESSAGE_OUT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl?box=outbox";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"����>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t>>");

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
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B");

	type = ACCESS_LIST_CALENDAR;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�J�����_�["
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"show_calendar.html";
	m_map[type].serializeKey = "CALENDAR";
	m_map[type].defaultCategoryURL = L"show_calendar.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t");

	//------------------------------------------------------------------
	//--- mixi,���
	type = ACCESS_LOGIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"���O�C��"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"login.html";

	// �g�b�v�y�[�W
	type = ACCESS_MAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"���C��"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"home.html";

	type = ACCESS_DIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"���L���e"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy
	// url(2) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy&comment_count=8
	m_map[type].cacheFilePattern = L"diary\\{urlparam:owner_id}\\{urlparam:id}.html";

	type = ACCESS_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�j���[�X"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://news.mixi.jp/view_news.pl?id=132704&media_id=4
	m_map[type].cacheFilePattern = L"news\\{urlparam:media_id}\\{urlparam:id}.html";

	type = ACCESS_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�R�~������"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_bbs.pl?id=9360823&comm_id=1198460
	// url(2) : http://mixi.jp/view_bbs.pl?page=4&comm_id=1198460&id=9360823
	m_map[type].cacheFilePattern = L"bbs\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_ENQUETE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�A���P�[�g"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_enquete.pl?id=13632077&comment_count=1&comm_id=1198460
	m_map[type].cacheFilePattern = L"enquete\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_EVENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�C�x���g"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_EVENT_JOIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�C�x���g"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_EVENT_MEMBER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// v0.9.1 ���_�ł�ReportView��[�y�[�W]����̑J�ڂ̂݃T�|�[�g
		, "mixi"
		, L"�C�x���g�Q���҈ꗗ"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"���L"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy
	// url(2) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy&comment_count=8
	m_map[type].cacheFilePattern = L"diary\\{urlparam:owner_id}\\{urlparam:id}.html";

	type = ACCESS_MESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_message.pl?id=xxx&box=inbox
	// url(2) : http://mixi.jp/view_message.pl?id=xxx&box=outbox
	m_map[type].cacheFilePattern = L"message\\{urlparam:box}\\{urlparam:id}.html";

	type = ACCESS_IMAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"�摜"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_MOVIE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"����"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_DOWNLOAD;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"�_�E�����[�h"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_PROFILE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�v���t�B�[��"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";

	type = ACCESS_BIRTHDAY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�v���t�B�[��"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";

	type = ACCESS_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// �{���̓R�~���j�e�B�g�b�v�y�[�W�Bv0.9.1 ���_�ł�CMZ3View���Ńg�s�b�N�ꗗ�ɐ؂�ւ��Ă���
		, "mixi"
		, L"�R�~���j�e�B"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_SCHEDULE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�ǉ������\��"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_PLAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�ėpURL"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- mixi,POST �n
	type = ACCESS_POST_CONFIRM_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_CONFIRM_REPLYMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���b�Z�[�W�ԐM�i�m�F�j"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_REPLYMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���b�Z�[�W�ԐM�i�����j"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_CONFIRM_NEWMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�V�K���b�Z�[�W�i�m�F�j"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_NEWMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�V�K���b�Z�[�W�i�����j"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_CONFIRM_NEWDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���L���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_NEWDIARY;
	m_map[type] = AccessTypeInfo::Data(
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
	// http://twitter.com/statuses/friends_timeline.xml
	// http://twitter.com/statuses/friends_timeline/takke.xml
	// http://twitter.com/statuses/replies.xml
	// http://twitter.com/statuses/user_timeline.xml
	// http://twitter.com/statuses/user_timeline/takke.xml
	// => twitter/user_timeline_takke.xml
	m_map[type].cacheFilePattern = L"twitter\\{urlafter:statuses/:friends_timeline.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_FRIENDS_TIMELINE";
	m_map[type].defaultCategoryURL = L"http://twitter.com/statuses/friends_timeline.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");

	type = ACCESS_TWITTER_FAVORITES;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"���C�ɓ���"
		, REQUEST_METHOD_GET
		);
	// http://twitter.com/favorites.xml
	// http://twitter.com/favorites/takke.xml
	// => twitter/favorites_takke.xml
	m_map[type].cacheFilePattern = L"twitter\\favorites{urlafter:favorites/:.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_FAVORITES";
	m_map[type].defaultCategoryURL = L"http://twitter.com/favorites.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");

	type = ACCESS_TWITTER_DIRECT_MESSAGES;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		);
	// http://twitter.com/direct_messages.xml
	// => twitter/recv.xml
	// http://twitter.com/direct_messages/sent.xml
	// => twitter/sent.xml
	m_map[type].cacheFilePattern = L"twitter\\{urlafter:direct_messages/:recv.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_DIRECT_MESSAGES";
	m_map[type].defaultCategoryURL = L"http://twitter.com/direct_messages.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"���b�Z�[�W");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");

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
	//--- mixi echo �֘A
	type = ACCESS_MIXI_RECENT_ECHO;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�݂�Ȃ̃G�R�["
		, REQUEST_METHOD_GET
		);
	// http://mixi.jp/recent_echo.pl
	// => recent_echo.html
	m_map[type].cacheFilePattern = L"recent_echo.html";
	m_map[type].serializeKey = "MIXI_RECENT_ECHO";
	m_map[type].defaultCategoryURL = L"recent_echo.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");

	// add_echo.pl
	type = ACCESS_MIXI_ADD_ECHO;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"��������"
		, REQUEST_METHOD_GET
		);


	//------------------------------------------------------------------
	//--- RSS Reader
	type = ACCESS_RSS_READER_FEED;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "RSS"
		, L"RSS Feed"
		, REQUEST_METHOD_GET
		);
	// http://hogehoge.mz3.jp/hogefuga/
	// => hogehoge.mz3.jp_hogefuga_.rss
	m_map[type].cacheFilePattern = L"rss\\{urlafter://}.rss";
	m_map[type].requestEncoding = ENCODING_UTF8;
	m_map[type].serializeKey = "RSS_FEED";
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY,  L"���e>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t>>");

	type = ACCESS_RSS_READER_ITEM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "RSS"
		, L"RSS Item"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].requestEncoding = ENCODING_UTF8;

	type = ACCESS_RSS_READER_AUTO_DISCOVERY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "RSS"
		, L"RSS AutoDiscovery"
		, REQUEST_METHOD_GET
		);
	// no-cache
//	m_map[type].cacheFilePattern = L"rss\\{urlafter://}.rss";
	m_map[type].requestEncoding = ENCODING_NOCONVERSION;
	m_map[type].serializeKey = "RSS_AUTO_DISCOVERY";

	//------------------------------------------------------------------
	//--- ���̑�
	type = ACCESS_INVALID;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_INVALID
		, ""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		);

	// TODO �K�{���ڂ̃e�X�g

	return true;
}