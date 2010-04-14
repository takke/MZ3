/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#include "stdafx.h"
#include "locale.h"
#include "AccessTypeInfo.h"

/// ������
bool AccessTypeInfo::init()
{
	ACCESS_TYPE type;

	m_map.clear();

	//------------------------------------------------------------------
	//--- �O���[�v�n
	//------------------------------------------------------------------
	type = ACCESS_GROUP_GENERAL;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"���̑�G"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "OTHERS";

	//------------------------------------------------------------------
	//--- mixi,���X�g�n
	//------------------------------------------------------------------
#ifdef BT_MZ3
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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"�Љ");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����");
	m_map[type].bodyIntegratedLinePattern1 = L"%2";
	m_map[type].bodyIntegratedLinePattern2 = L"%1";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2\t(%3)";
	m_map[type].bodyIntegratedLinePattern2 = L"%1";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2\t(%3)";
	m_map[type].bodyIntegratedLinePattern2 = L"%1";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"���o��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"�z�M����>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�z����>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2 (%3)";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�ŏI���O�C��>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"�ǉ���>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%1\t(%2)";
	m_map[type].bodyIntegratedLinePattern2 = L"%3";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"�R�~���j�e�B");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�l��>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%1\t(%2)";
	m_map[type].bodyIntegratedLinePattern2 = L"%3";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���O�C������");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";
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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"�R�~���j�e�B");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"�l��");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2\t(%3)";
	m_map[type].bodyIntegratedLinePattern2 = L"%1";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"����>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2\t(%3)";
	m_map[type].bodyIntegratedLinePattern2 = L"%1";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"����");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";

	type = ACCESS_LIST_BOOKMARK;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�u�b�N�}�[�N"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].serializeKey = "BOOKMARK";
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�g�s�b�N");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"�R�~���j�e�B");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";
#endif

	//------------------------------------------------------------------
	//--- mixi,���
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_LOGIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"���O�C��"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"login.html";
	m_map[type].serializeKey = "MIXI_LOGIN";

	// �g�b�v�y�[�W
	type = ACCESS_MAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"���C��"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"home.html";
	m_map[type].serializeKey = "MIXI_HOME";

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
	m_map[type].serializeKey = "MIXI_DIARY";

	type = ACCESS_NEIGHBORDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"���L���e"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/neighbor_diary.pl?id=xxx&owner_id=yyy&direction=prev&new_id=zzz
	// url(2) : http://mixi.jp/neighbor_diary.pl?id=xxx&owner_id=yyy&direction=next&new_id=zzz
	m_map[type].cacheFilePattern = L"diary\\{urlparam:owner_id}\\{urlparam:new_id}.html";
	m_map[type].serializeKey = "MIXI_NEIGHBORDIARY";

	type = ACCESS_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�j���[�X"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://news.mixi.jp/view_news.pl?id=132704&media_id=4
	m_map[type].cacheFilePattern = L"news\\{urlparam:media_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_NEWS";

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
	m_map[type].serializeKey = "MIXI_BBS";

	type = ACCESS_ENQUETE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�A���P�[�g"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_enquete.pl?id=13632077&comment_count=1&comm_id=1198460
	m_map[type].cacheFilePattern = L"enquete\\{urlparam:comm_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_ENQUETE";

	type = ACCESS_EVENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�C�x���g"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_EVENT";

	type = ACCESS_EVENT_JOIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�C�x���g"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_EVENT_JOIN";

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
	m_map[type].serializeKey = "MIXI_MYDIARY";

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
	m_map[type].serializeKey = "MIXI_MESSAGE";

	type = ACCESS_MOVIE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"����"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_MOVIE";

	type = ACCESS_PROFILE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�v���t�B�[��"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_PROFILE";

	type = ACCESS_BIRTHDAY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�a����"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_BIRTHDAY";

	type = ACCESS_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// �{���̓R�~���j�e�B�g�b�v�y�[�W�Bv0.9.1 ���_�ł�CMZ3View���Ńg�s�b�N�ꗗ�ɐ؂�ւ��Ă���
		, "mixi"
		, L"�R�~���j�e�B"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_COMMUNITY";

	type = ACCESS_SCHEDULE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�ǉ������\��"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_SCHEDULE";
#endif

	type = ACCESS_IMAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"�摜"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_IMAGE";

	type = ACCESS_DOWNLOAD;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"�_�E�����[�h"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "DOWNLOAD";


	//------------------------------------------------------------------
	//--- mixi,POST �n
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_POST_COMMENT_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_COMMENT_CONFIRM";

	type = ACCESS_POST_COMMENT_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_COMMENT_REGIST";

	type = ACCESS_POST_REPLYMESSAGE_ENTRY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���b�Z�[�W�ԐM�i���́j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_REPLYMESSAGE_ENTRY";

	type = ACCESS_POST_REPLYMESSAGE_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���b�Z�[�W�ԐM�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_REPLYMESSAGE_CONFIRM";

	type = ACCESS_POST_REPLYMESSAGE_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���b�Z�[�W�ԐM�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_REPLYMESSAGE_REGIST";

	type = ACCESS_POST_NEWMESSAGE_ENTRY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�V�K���b�Z�[�W�i���́j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWMESSAGE_ENTRY";

	type = ACCESS_POST_NEWMESSAGE_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�V�K���b�Z�[�W�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWMESSAGE_CONFIRM";

	type = ACCESS_POST_NEWMESSAGE_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�V�K���b�Z�[�W�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWMESSAGE_REGIST";

	type = ACCESS_POST_NEWDIARY_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"���L���e�i�m�F�j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWDIARY_CONFIRM";

	type = ACCESS_POST_NEWDIARY_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWDIARY_REGIST";
#endif

	//------------------------------------------------------------------
	//--- Twitter �n
	//------------------------------------------------------------------
	type = ACCESS_TWITTER_FRIENDS_TIMELINE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"�^�C�����C��"
//		, REQUEST_METHOD_POST// �^�C�����C���擾��POST�ɂ��Ă݂�
		, REQUEST_METHOD_GET
		);
	// http://twitter.com/statuses/friends_timeline.xml
	// http://twitter.com/statuses/friends_timeline/takke.xml
	// http://twitter.com/statuses/replies.xml
	// http://twitter.com/statuses/user_timeline.xml
	// http://twitter.com/statuses/user_timeline/takke.xml
	// => twitter/user_timeline_takke.xml
	m_map[type].cacheFilePattern = L"twitter\\{urlafter:/:friends_timeline.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_FRIENDS_TIMELINE";
	m_map[type].defaultCategoryURL = L"http://twitter.com/statuses/friends_timeline.xml";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2 \t(%3)";	// "���O  (���t)"
	m_map[type].bodyIntegratedLinePattern2 = L"%1";			// "����"

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2 \t(%3)";	// "���O  (���t)"
	m_map[type].bodyIntegratedLinePattern2 = L"%1";			// "����"

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"���b�Z�[�W");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2 \t(%3)";	// "���O  (���t)"
	m_map[type].bodyIntegratedLinePattern2 = L"%1";			// "���b�Z�[�W"

	type = ACCESS_TWITTER_UPDATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"�X�V"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_UPDATE";

	type = ACCESS_TWITTER_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "Twitter"
		, L"Twitter����"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_USER";

	type = ACCESS_TWITTER_NEW_DM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"���b�Z�[�W���M"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_NEW_DM";

	//------------------------------------------------------------------
	//--- mixi echo �֘A
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_MIXI_RECENT_VOICE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"�݂�Ȃ̃{�C�X"
		, REQUEST_METHOD_GET
		);
	// http://mixi.jp/recent_voice.pl
	// => recent_voice.html
	m_map[type].cacheFilePattern = L"mixi_voice\\{urlafter:mixi.jp/:recent_voice}.html";
	m_map[type].serializeKey = "MIXI_RECENT_VOICE";
	m_map[type].defaultCategoryURL = L"recent_voice.pl";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2 \t%3";	// "���O  (���t)"
	m_map[type].bodyIntegratedLinePattern2 = L"%1";		// "����"

	// add_voice.pl
	type = ACCESS_MIXI_ADD_VOICE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"��������"
		, REQUEST_METHOD_POST
		);
	m_map[type].serializeKey = "MIXI_ADD_VOICE";

	// add_voice.pl(�ԐM)
	type = ACCESS_MIXI_ADD_VOICE_REPLY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"��������(�ԐM)"
		, REQUEST_METHOD_POST
		);
	m_map[type].serializeKey = "MIXI_ADD_VOICE_REPLY";

	// echo ����
	type = ACCESS_MIXI_VOICE_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"mixi�{�C�X����"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_RECENT_VOICE_ITEM";
#endif

	//------------------------------------------------------------------
	//--- Wassr �n
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_WASSR_FRIENDS_TIMELINE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Wassr"
		, L"�^�C�����C��"
		, REQUEST_METHOD_GET
		);
	// http://api.wassr.jp/statuses/friends_timeline.xml
	// http://api.wassr.jp/statuses/replies.xml
	// => wassr/user_timeline_takke.xml
	m_map[type].cacheFilePattern = L"wassr\\{urlafter:statuses/:friends_timeline.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Wassr API => UTF-8
	m_map[type].serializeKey = "WASSR_FRIENDS_TIMELINE";
	m_map[type].defaultCategoryURL = L"http://api.wassr.jp/statuses/friends_timeline.xml";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2 \t(%3)";	// "���O  (���t)"
	m_map[type].bodyIntegratedLinePattern2 = L"%1";			// "����"

	type = ACCESS_WASSR_UPDATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Wassr"
		, L"Wassr�X�V"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Wassr API => UTF-8
	m_map[type].serializeKey = "WASSR_UPDATE";

	type = ACCESS_WASSR_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "Wassr"
		, L"Wassr����"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "WASSR_USER";
#endif

	//------------------------------------------------------------------
	//--- goo�z�[�� �n
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_GOOHOME_QUOTE_QUOTES_FRIENDS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "gooHome"
		, L"�F�B�E���ڂ̐l"
		, REQUEST_METHOD_GET
		);
	// http://home.goo.ne.jp/api/quote/quotes/friends/json
	// http://home.goo.ne.jp/api/quote/quotes/myself/json
	// => goohome/quote/quotes_friends.json
	m_map[type].cacheFilePattern = L"goohome\\quote\\quotes_{urlafter:quotes/:friends.json}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// goohome API => UTF-8
	m_map[type].serializeKey = "GOOHOME_QUOTE_QUOTES_FRIENDS";
	m_map[type].defaultCategoryURL = L"http://home.goo.ne.jp/api/quote/quotes/friends/json";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"����");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"���O>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"���t>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2 \t(%3)";	// "���O  (���t)"
	m_map[type].bodyIntegratedLinePattern2 = L"%1";			// "����"

	type = ACCESS_GOOHOME_QUOTE_UPDATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "gooHome"
		, L"�ЂƂ��ƍX�V"
		, REQUEST_METHOD_POST
		);
	m_map[type].serializeKey = "GOOHOME_QUOTE_UPDATE";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Wassr API => UTF-8

	type = ACCESS_GOOHOME_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "gooHome"
		, L"�ЂƂ��Ɣ���"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "GOOHOME_USER";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
#endif

	//------------------------------------------------------------------
	//--- RSS Reader
	//------------------------------------------------------------------
#ifdef BT_MZ3
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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"�^�C�g��");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY,  L"���e>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"���t>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%1 \t(%3)";	// "�^�C�g��  (���t)"
	m_map[type].bodyIntegratedLinePattern2 = L"%2";			// "���e"

	type = ACCESS_RSS_READER_ITEM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "RSS"
		, L"RSS Item"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].requestEncoding = ENCODING_UTF8;
	m_map[type].serializeKey = "RSS_ITEM";

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
#endif

	//------------------------------------------------------------------
	//--- ���̑�
	//------------------------------------------------------------------
	type = ACCESS_INVALID;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_INVALID
		, ""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "INVALID";

	// �o�[�W�����`�F�b�N
	type = ACCESS_SOFTWARE_UPDATE_CHECK;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "general"
		, L"�o�[�W�����`�F�b�N"
		, REQUEST_METHOD_GET
		);
	// http://mz3.jp/latest_version.xml
	m_map[type].cacheFilePattern = L"mz3.jp\\latest_version.xml";
	m_map[type].defaultCategoryURL = L"http://mz3.jp/latest_version.xml";
	m_map[type].serializeKey = "MZ3_VERSION_CHECK";

	type = ACCESS_PLAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"�ėpURL"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "PLAIN";

	//------------------------------------------------------------------
	// �V���A���C�Y�L�[ �� �A�N�Z�X��ʃ}�b�v �̍\�z
	//------------------------------------------------------------------
	m_serializeKeyToAccessKeyMap.clear();
	for (ACCESS_TYPE_TO_DATA_MAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
		ACCESS_TYPE accessType = it->first;
		std::string serializeKey = it->second.serializeKey;
		if (!serializeKey.empty()) {
			m_serializeKeyToAccessKeyMap[ serializeKey ] = accessType;
		}
	}

	//------------------------------------------------------------------
	// TODO �K�{���ڂ̃e�X�g
	//------------------------------------------------------------------

	//------------------------------------------------------------------
	// Debug ���[�h�̂Ƃ� CSV �_���v����
	//------------------------------------------------------------------
#if defined(_DEBUG) && !defined(WINCE)
	CString tsv_dump_filepath;
	{
		TCHAR path[256];
		memset(path, 0x00, sizeof(TCHAR) * 256);
		GetModuleFileName(NULL, path, 256);
		tsv_dump_filepath = path;	// "...\\MZ3.exe"
		tsv_dump_filepath = tsv_dump_filepath.Left(tsv_dump_filepath.ReverseFind('\\'));
		tsv_dump_filepath += L"\\_access_type_list.csv";
	}

	FILE* fp_tsv = _wfopen(tsv_dump_filepath, L"wt");
	if (fp_tsv!=NULL) {
		fwprintf(fp_tsv, L"ACCESS_TYPE,infoType,serviceType,shortText,requestType,requestEncoding,cacheFilePattern,"
						 L"serializeKey,bCruiseTarget,defaultCategoryURL,bodyHeaderCol1,,bodyHeaderCol2,,bodyHeaderCol3,,"
						 L"bodyIntegratedLinePattern1,bodyIntegratedLinePattern2\n");
		for (ACCESS_TYPE_TO_DATA_MAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
			const ACCESS_TYPE& accessType = it->first;
			const Data& data = it->second;

			fwprintf(fp_tsv, L"\"%d\",", accessType);
			fwprintf(fp_tsv, L"\"%s\",", info_type_to_text(data.infoType));
			fwprintf(fp_tsv, L"\"%s\",", CString(data.serviceType.c_str()));
			fwprintf(fp_tsv, L"\"%s\",", data.shortText.c_str());
			fwprintf(fp_tsv, L"\"%s\",", request_method_to_text(data.requestMethod));
			fwprintf(fp_tsv, L"\"%s\",", encoding_to_text(data.requestEncoding));
			fwprintf(fp_tsv, L"\"%s\",", data.cacheFilePattern.c_str());

			//--- �O���[�v�n�A�J�e�S���n�݂̂�������
			fwprintf(fp_tsv, L"\"%s\",", CString(data.serializeKey.c_str()));

			//--- �J�e�S���n�݂̂�������
			fwprintf(fp_tsv, L"\"%d\",", data.bCruiseTarget ? 1 : 0);
			fwprintf(fp_tsv, L"\"%s\",", data.defaultCategoryURL.c_str());

			fwprintf(fp_tsv, L"\"%s\",", data.bodyHeaderCol1.title.c_str());
			fwprintf(fp_tsv, L"\"%s\",", body_indicate_type_to_text(data.bodyHeaderCol1.type));
			fwprintf(fp_tsv, L"\"%s\",", data.bodyHeaderCol2.title.c_str());
			fwprintf(fp_tsv, L"\"%s\",", body_indicate_type_to_text(data.bodyHeaderCol2.type));
			fwprintf(fp_tsv, L"\"%s\",", data.bodyHeaderCol3.title.c_str());
			fwprintf(fp_tsv, L"\"%s\",", body_indicate_type_to_text(data.bodyHeaderCol3.type));

			// Excel hack.
			fwprintf(fp_tsv, L"=\"%s\",", data.bodyIntegratedLinePattern1.c_str());
			fwprintf(fp_tsv, L"=\"%s\",", data.bodyIntegratedLinePattern2.c_str());

			fwprintf(fp_tsv, L"\n");
		}
		fclose(fp_tsv);
	}
#endif

	return true;
}