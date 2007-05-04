/**
 * �e��萔
 */
#pragma once

/// �o�[�W����������
#define MZ3_VERSION_TEXT			L"MZ3.i Version 0.8.0.0 Beta7"

/// ���r�W�����ԍ�
#define MZ3_SVN_REVISION			L"$Rev$" 

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
/// �f�[�^���M�����i�m�F�p���M�����j
#define WM_MZ3_POST_CONFIRM			WM_APP + 12
/// �f�[�^���M�����i����I���j
#define	WM_MZ3_POST_END				WM_APP + 14
/// �f�[�^���M�����i���f�j
#define WM_MZ3_POST_ABORT			WM_APP + 18
/// �f�[�^��M��
#define WM_MZ3_ACCESS_LOADED		WM_APP + 19

/// ���f
#define WM_MZ3_ABORT				WM_APP + 26
/// ��ʍč\��
#define WM_MZ3_FIT					WM_APP + 40

/**
 * �A�N�Z�X�^�C�v
 */
enum ACCESS_TYPE {
	ACCESS_INVALID = -1,		///< �s���ȃA�N�Z�X��ʁi�����l=�A�N�Z�X��ʖ��ݒ�j
	ACCESS_LOGIN = 0,			///< ���O�C��
	ACCESS_MAIN,				///< ���C��(home.pl)
	ACCESS_DIARY,				///< ���L���e(view_diary.pl)
	ACCESS_BBS,					///< �R�~���j�e�B���e(view_bbs.pl)
	ACCESS_ENQUETE,				///< �A���P�[�g(view_enquete.pl)
	ACCESS_EVENT,				///< �C�x���g(view_event.pl)
	ACCESS_MYDIARY,				///< �����̓��L���e(view_diary.pl)
	ACCESS_MESSAGE,				///< ���b�Z�[�W(view_message.pl)
	ACCESS_NEWS,				///< �j���[�X���e(view_news.pl)
	ACCESS_PROFILE,				///< �l�y�[�W(show_friend.pl) �iv0.6.2 ���݁A������URL�����N�j
	ACCESS_COMMUNITY,			///< �R�~���j�e�B�y�[�W(view_community.pl)
	ACCESS_ADDDIARY,			///< ���L���e
	ACCESS_IMAGE,				///< �摜
	ACCESS_DOWNLOAD,			///< �_�E�����[�h

	//--- GROUP �n�F�O���[�v�^�u�̍��ځB
	ACCESS_GROUP_MYDIARY,		///< ���L �O���[�v
	ACCESS_GROUP_COMMUNITY,		///< �R�~���j�e�B �O���[�v
	ACCESS_GROUP_MESSAGE,		///< ���b�Z�[�W �O���[�v
	ACCESS_GROUP_NEWS,			///< �j���[�X�O���[�v
	ACCESS_GROUP_OTHERS,		///< ���̑� �O���[�v

	//--- LIST �n�F�J�e�S�����X�g�̍��ځB
	ACCESS_LIST_MYDIARY,		///< �����̓��L�ꗗ
	ACCESS_LIST_DIARY,			///< �}�C�~�N�ŐV���L�ꗗ
	ACCESS_LIST_NEW_COMMENT,	///< �ŐV�R�����g�ꗗ
	ACCESS_LIST_NEW_BBS,		///< �R�~���j�e�B�������݈ꗗ(new_bbs.pl)
	ACCESS_LIST_MESSAGE_IN,		///< ���b�Z�[�W�ꗗ�i��M���j
	ACCESS_LIST_MESSAGE_OUT,	///< ���b�Z�[�W�ꗗ�i��M���j
	ACCESS_LIST_FOOTSTEP,		///< ����
	ACCESS_LIST_COMMENT,		///< �R�����g�ꗗ
	ACCESS_LIST_NEWS,			///< �j���[�X�ꗗ
	ACCESS_LIST_BOOKMARK,		///< �u�b�N�}�[�N
	ACCESS_LIST_FAVORITE,		///< ���C�ɓ���
	ACCESS_LIST_FRIEND,			///< �}�C�~�N�ꗗ
	ACCESS_LIST_COMMUNITY,		///< �R�~���j�e�B�ꗗ
	ACCESS_LIST_INTRO,			///< �Љ(show_intro.pl)
	ACCESS_LIST_BBS,			///< �g�s�b�N�ꗗ(list_bbs.pl)

	//--- POST �n�FPOST ���̐i�����b�Z�[�W�p�BCMixiData �Ƃ��ĕێ����邱�Ƃ͂Ȃ��B
	ACCESS_POST_CONFIRM_COMMENT,		///< �R�����g���e�i�m�F��ʁj
	ACCESS_POST_ENTRY_COMMENT,			///< �R�����g���e�i�������݉�ʁj
	ACCESS_POST_CONFIRM_REPLYMESSAGE,	///< ���b�Z�[�W�ԐM�i�m�F��ʁj
	ACCESS_POST_ENTRY_REPLYMESSAGE,		///< ���b�Z�[�W�ԐM�i�������݉�ʁj
	ACCESS_POST_CONFIRM_NEWMESSAGE,		///< �V�K���b�Z�[�W�i�m�F��ʁj
	ACCESS_POST_ENTRY_NEWMESSAGE,		///< �V�K���b�Z�[�W�i�������݉�ʁj
	ACCESS_POST_CONFIRM_NEWDIARY,		///< ���L���e�i�m�F��ʁj
	ACCESS_POST_ENTRY_NEWDIARY,			///< �R�����g���e�i�������݉�ʁj

	//--- ���̑�
	ACCESS_HELP,				///< Readme.txt ��p�̓���^�C�v
};

/**
 * �y�[�W�̎擾�^�C�v
 */
enum GETPAGE_TYPE
{
	GETPAGE_ALL = 0,		///< �S���擾
	GETPAGE_LATEST10 = 1,	///< �ŐV�̂P�O���擾
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
enum CONTENT_TYPE {
	CONTENT_TYPE_INVALID = -1,			///< �����l
	CONTENT_TYPE_MULTIPART = 0,			///< Content-Type: multipart/form-data
	CONTENT_TYPE_FORM_URLENCODED = 1,	///< Content-Type: application/x-www-form-urlencoded
};
