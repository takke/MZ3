/**
 * �e��萔
 */
#pragma once

/// �A�v���P�[�V������
#ifdef WINCE
#define	MZ3_APP_NAME				L"MZ3"
#else
#define	MZ3_APP_NAME				L"MZ4"
#endif

// CHM �w���v�t�@�C���p�X
#ifndef WINCE
# define MZ4_MANUAL_URL				L"http://mz3.jp/manual_mz4/"
#endif

// Window ���b�Z�[�W

/// �f�[�^�擾�����i����I���j
#define WM_MZ3_GET_END				WM_APP + 2 
/// �f�[�^�擾�����i�o�C�i���f�[�^�擾�����j
#define WM_MZ3_GET_END_BINARY		WM_APP + 20
/// �f�[�^�擾�����i�G���[�����j
#define WM_MZ3_GET_ERROR			WM_APP + 4
/// �f�[�^�擾�����i���f�j
#define WM_MZ3_GET_ABORT			WM_APP + 6
/// 
#define WM_MZ3_ACCESS_INFORMATION	WM_APP + 8
/// �r���[�ύX
#define WM_MZ3_CHANGE_VIEW			WM_APP + 10
/// �ēǍ��i���e��Ȃǁj
#define WM_MZ3_RELOAD				WM_APP + 11
/// �f�[�^���M�����i���͉�ʎ擾�p���M�����j
#define WM_MZ3_POST_ENTRY_END		WM_APP + 12
/// �f�[�^���M�����i�m�F�p���M�����j
#define WM_MZ3_POST_CONFIRM_END		WM_APP + 13
/// �f�[�^���M�����i����I���j
#define	WM_MZ3_POST_END				WM_APP + 14
/// �f�[�^���M�����i���f�j
#define WM_MZ3_POST_ABORT			WM_APP + 18
/// �f�[�^��M��
#define WM_MZ3_ACCESS_LOADED		WM_APP + 19

/// �R�����g�ړ�
#define WM_MZ3_MOVE_DOWN_LIST		WM_APP + 21
#define WM_MZ3_MOVE_UP_LIST			WM_APP + 22

/// �R�����g���X�g�̃A�C�e�������擾
#define WM_MZ3_GET_LIST_ITEM_COUNT	WM_APP + 23

/// ���f
#define WM_MZ3_ABORT				WM_APP + 26
/// ��ʍč\��
#define WM_MZ3_FIT					WM_APP + 40
/// �r���[��\��
#define WM_MZ3_HIDE_VIEW			WM_APP + 41

/**
 * �A�N�Z�X�^�C�v
 */
enum ACCESS_TYPE {
	ACCESS_INVALID = -1,		///< �s���ȃA�N�Z�X��ʁi�����l=�A�N�Z�X��ʖ��ݒ�j

	//----------------------------------------------
	// mixi �n
	//----------------------------------------------
	ACCESS_LOGIN=0,				///< ���O�C��
	ACCESS_MAIN,				///< ���C��(home.pl)
	ACCESS_DIARY,				///< ���L���e(view_diary.pl)
	ACCESS_NEIGHBORDIARY,		///< ���L���e(neighbor_diary.pl)	//MOriyama�ǉ� ���̓��L�A�O�̓��L
	ACCESS_BBS,					///< �R�~���j�e�B���e(view_bbs.pl)
	ACCESS_ENQUETE,				///< �A���P�[�g(view_enquete.pl)
	ACCESS_EVENT,				///< �C�x���g(view_event.pl)
	ACCESS_EVENT_JOIN,			///< �Q���C�x���g(view_event.pl)  //MOriyama�ǉ�
	ACCESS_EVENT_MEMBER,		///< �C�x���g�Q���҈ꗗ(list_event_member.pl)
	ACCESS_MYDIARY,				///< �����̓��L���e(view_diary.pl)
	ACCESS_MESSAGE,				///< ���b�Z�[�W(view_message.pl)
	ACCESS_NEWS,				///< �j���[�X���e(view_news.pl)
	ACCESS_PROFILE,				///< �l�y�[�W(show_friend.pl)
	ACCESS_BIRTHDAY,			///< �a�����l�y�[�W(show_friend.pl)  //MOriyama�ǉ�
	ACCESS_COMMUNITY,			///< �R�~���j�e�B�y�[�W(view_community.pl)
	ACCESS_IMAGE,				///< �摜
	ACCESS_MOVIE,				///< ����
	ACCESS_SCHEDULE,			///< �ǉ������\��  //MOriyama�ǉ�
	ACCESS_MIXI_ECHO_USER,		///< �G�R�[�̃I�u�W�F�N�g
	ACCESS_DOWNLOAD,			///< �ėp�_�E�����[�h
	ACCESS_PLAIN,				///< �ėp�e�L�X�g�f�[�^

	//--- GROUP �n�F�O���[�v�^�u�̍��ځB
	ACCESS_GROUP_GENERAL,		///< �O���[�v ���

	//--- LIST �n�F�J�e�S�����X�g�̍��ځB
	ACCESS_LIST_MYDIARY,			///< �����̓��L�ꗗ
	ACCESS_LIST_DIARY,				///< �}�C�~�N�ŐV���L�ꗗ
	ACCESS_LIST_NEW_COMMENT,		///< �ŐV�R�����g�ꗗ
	ACCESS_LIST_NEW_BBS,			///< �R�~���j�e�B�������݈ꗗ(new_bbs.pl)
//	ACCESS_LIST_MESSAGE_IN,			///< ���b�Z�[�W�ꗗ�i��M���j
//	ACCESS_LIST_MESSAGE_OUT,		///< ���b�Z�[�W�ꗗ�i��M���j
	ACCESS_LIST_FOOTSTEP,			///< ����
	ACCESS_LIST_COMMENT,			///< �R�����g�ꗗ
	ACCESS_LIST_NEWS,				///< �j���[�X�ꗗ
	ACCESS_LIST_BOOKMARK,			///< �u�b�N�}�[�N
	ACCESS_LIST_FAVORITE_USER,		///< ���C�ɓ��胆�[�U
	ACCESS_LIST_FAVORITE_COMMUNITY,	///< ���C�ɓ���R�~��
	ACCESS_LIST_FRIEND,				///< �}�C�~�N�ꗗ
	ACCESS_LIST_COMMUNITY,			///< �R�~���j�e�B�ꗗ
	ACCESS_LIST_INTRO,				///< �Љ(show_intro.pl)
	ACCESS_LIST_BBS,				///< �g�s�b�N�ꗗ(list_bbs.pl)
	ACCESS_LIST_NEW_BBS_COMMENT,	///< �R�~���j�e�B�R�����g�L������
	ACCESS_LIST_CALENDAR,			///< �J�����_�[  //icchu�ǉ�

	//--- mixi echo �֘A
	ACCESS_MIXI_RECENT_ECHO,			///< �݂�Ȃ̃G�R�[�Frecent_echo.pl
	ACCESS_MIXI_ADD_ECHO,				///< �G�R�[�������ށFadd_echo.pl
	ACCESS_MIXI_ADD_ECHO_REPLY,			///< �G�R�[�ԐM�Fadd_echo.pl

	//--- POST �n�FPOST ���̐i�����b�Z�[�W�p�BCMixiData �Ƃ��ĕێ����邱�Ƃ͂Ȃ��B
	ACCESS_POST_COMMENT_CONFIRM,		///< �R�����g���e�i�m�F��ʁj
	ACCESS_POST_COMMENT_REGIST,			///< �R�����g���e�i�������݉�ʁj
	ACCESS_POST_REPLYMESSAGE_ENTRY,		///< ���b�Z�[�W�ԐM�i���͉�ʁj
	ACCESS_POST_REPLYMESSAGE_CONFIRM,	///< ���b�Z�[�W�ԐM�i�m�F��ʁj
	ACCESS_POST_REPLYMESSAGE_REGIST,	///< ���b�Z�[�W�ԐM�i�������݉�ʁj
	ACCESS_POST_NEWMESSAGE_ENTRY,		///< �V�K���b�Z�[�W�i���͉�ʁj
	ACCESS_POST_NEWMESSAGE_CONFIRM,		///< �V�K���b�Z�[�W�i�m�F��ʁj
	ACCESS_POST_NEWMESSAGE_REGIST,		///< �V�K���b�Z�[�W�i�������݉�ʁj
	ACCESS_POST_NEWDIARY_CONFIRM,		///< ���L���e�i�m�F��ʁj
	ACCESS_POST_NEWDIARY_REGIST,		///< ���L���e�i�������݉�ʁj

	//----------------------------------------------
	// Twitter
	//----------------------------------------------
	//--- Twitter Timeline �n
	ACCESS_TWITTER_FRIENDS_TIMELINE,	///< [GET/POST] friends_timeline.xml
	ACCESS_TWITTER_UPDATE,				///< [POST] �X�e�[�^�X�X�V
	ACCESS_TWITTER_USER,				///< �e�����v�f

	//--- Twitter DM �n
	ACCESS_TWITTER_DIRECT_MESSAGES,		///< [GET/POST] direct_messages.xml
	ACCESS_TWITTER_NEW_DM,				///< [POST] DM���M

	//--- Twitter Favorites �n
	ACCESS_TWITTER_FAVORITES,			///< [GET/POST] ���C�ɓ���擾

	//----------------------------------------------
	// Wassr
	//----------------------------------------------
	//--- Wassr Timeline �n
	ACCESS_WASSR_FRIENDS_TIMELINE,		///< [GET/POST] Wassr, friends_timeline.xml
	ACCESS_WASSR_UPDATE,				///< [POST] Wassr, �X�e�[�^�X�X�V
	ACCESS_WASSR_USER,					///< Wassr, �e�����v�f

	//----------------------------------------------
	// goo�z�[��
	//----------------------------------------------
	//--- goo�z�[�� �ЂƂ��Ɗ֘A
	ACCESS_GOOHOME_QUOTE_QUOTES_FRIENDS,	///< [GET] quotes/friends/json
	ACCESS_GOOHOME_USER,					///< gooHome, �e�����v�f
	ACCESS_GOOHOME_QUOTE_UPDATE,			///< [POST] �ЂƂ��Ɣ���

	//----------------------------------------------
	// RSS
	//----------------------------------------------
	//--- RSS Reader
	ACCESS_RSS_READER_FEED,				///< RSS Reader Feed
	ACCESS_RSS_READER_ITEM,				///< RSS Reader Item
	ACCESS_RSS_READER_AUTO_DISCOVERY,	///< RSS Reader; RSS AutoDiscovery �pItem

	//--- ���̑�
	ACCESS_HELP,						///< Readme.txt ��p�̓���^�C�v
	ACCESS_ERRORLOG,					///< mz3log.txt ��p�̓���^�C�v
	ACCESS_SOFTWARE_UPDATE_CHECK,		///< �o�[�W�����`�F�b�N

	//--- API�o�^
	ACCESS_TYPE_MZ3_SCRIPT_BASE = 1000,	///< ����ȍ~�̔ԍ��͑S�� MZ3 Script API �o�^�ɂ��A�N�Z�X��ʁB
										///< ���e�͎��s���Ɍ��肷��B
};

/**
 * �y�[�W�̎擾�^�C�v
 */
enum GETPAGE_TYPE
{
	GETPAGE_ALL = 0,		///< �S���擾
	GETPAGE_LATEST20 = 1,	///< �ŐV�̂Q�O���擾
};

/**
 * WriteView �̏������
 */
enum WRITEVIEW_TYPE {
	WRITEVIEW_TYPE_INVALID  = -1,		///< �����l
	WRITEVIEW_TYPE_COMMENT  =  0,		///< �R�����g���e�����i���L�A�R�~���j�e�B���p�H�j
	WRITEVIEW_TYPE_NEWDIARY =  1,		///< ���L���e����
	WRITEVIEW_TYPE_REPLYMESSAGE  =  2,	///< ���b�Z�[�W�ԐM����
	WRITEVIEW_TYPE_NEWMESSAGE  =  3,	///< �V�K���b�Z�[�W�̑��M����
};

/// ���M���� Content-Type �̎��
typedef LPCTSTR CONTENT_TYPE;

///< �����l
#define CONTENT_TYPE_INVALID L""

/// Content-Type: multipart/form-data
#define CONTENT_TYPE_MULTIPART L"multipart/form-data; boundary=---------------------------7d62ee108071e"
/// Content-Type: application/x-www-form-urlencoded
#define CONTENT_TYPE_FORM_URLENCODED L"application/x-www-form-urlencoded"

/// �R���p�C�����s���b�Z�[�W
#define FAILED_TO_COMPILE_REGEX_MSG		L"�R���p�C�����s"

/// Smartphone/Standard Edition �̏ꍇ�̃c�[���o�[������
#define MZ3_TOOLBAR_HEIGHT	26

/// �������[�v�h�~�p���[�v�ő�l
#define MZ3_INFINITE_LOOP_MAX_COUNT	100000