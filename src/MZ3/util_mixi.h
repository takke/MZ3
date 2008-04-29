/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/**
 * mixi�֘A���[�e�B���e�B
 */
#include "util_base.h"
#include "MixiUrlParser.h"

/// MZ3 �p���[�e�B���e�B
namespace util
{

/**
 * URL ����A�N�Z�X��ʂ𐄒肷��
 */
inline ACCESS_TYPE EstimateAccessTypeByUrl( const CString& url ) 
{
	// �Ƃ肠���� view �n�̂�
	if( url.Find( L"home.pl" ) != -1 ) 			{ return ACCESS_MAIN;      } // ���C��
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_DIARY;     } // ���L���e
	if( url.Find( L"view_bbs.pl" ) != -1 ) 		{ return ACCESS_BBS;       } // �R�~���j�e�B���e
	if( url.Find( L"view_enquete.pl" ) != -1 ) 	{ return ACCESS_ENQUETE;   } // �A���P�[�g
	if( url.Find( L"view_event.pl" ) != -1 ) 	{ return ACCESS_EVENT;     } // �C�x���g
	if( url.Find( L"list_event_member.pl" ) != -1 ) { return ACCESS_EVENT_MEMBER; } // �C�x���g�Q���҈ꗗ
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_MYDIARY;   } // �����̓��L���e
	if( url.Find( L"view_message.pl" ) != -1 ) 	{ return ACCESS_MESSAGE;   } // ���b�Z�[�W
	if( url.Find( L"view_news.pl" ) != -1 ) 	{ return ACCESS_NEWS;      } // �j���[�X���e
	if( url.Find( L"show_friend.pl" ) != -1 ) 	{ return ACCESS_PROFILE;   } // �l�y�[�W
	if( url.Find( L"view_community.pl" ) != -1 ){ return ACCESS_COMMUNITY; } // �R�~���j�e�B�y�[�W

	// �s���Ȃ̂� INVALID �Ƃ���
	return ACCESS_INVALID;
}

/**
 * ���[�UID�� AuthorID, OwnerID ����擾����
 */
inline int GetUserIdFromAuthorOrOwnerID( const CMixiData& mixi )
{
	int userId = mixi.GetAuthorID();
	if( userId < 0 ) {
		userId = mixi.GetOwnerID();
	}
	return userId;
}

/**
 * url �쐬�ihttp://mixi.jp/ �̕⊮�j
 */
inline CString CreateMixiUrl(LPCTSTR str)
{
	if( wcsstr( str, L"mixi.jp" ) == NULL &&
		wcsstr( str, L"http://" ) == NULL)
	{
		CString uri;
		uri.Format(_T("http://mixi.jp/%s"), str);
		return uri;
	}else{
		return str;
	}
}

/**
 * ���ǁE���Ǐ�ԊǗ����O�t�@�C���p��ID�𐶐�����B
 */
inline CString GetLogIdString( const CMixiData& mixi )
{
	CString logId;

	switch (mixi.GetAccessType()) {
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
		logId.Format(_T("d%d"), mixi::MixiUrlParser::GetID( mixi.GetURL() ) );
		break;
	case ACCESS_BBS:
		logId.Format(_T("b%d"), mixi.GetID());
		break;
	case ACCESS_EVENT:
		logId.Format(_T("v%d"), mixi.GetID());
		break;
	case ACCESS_ENQUETE:
		logId.Format(_T("e%d"), mixi.GetID());
		break;
	case ACCESS_PROFILE:
		logId.Format(_T("p%d"), mixi.GetID());
		break;

	default:
		logId = L"";
		break;
	}

	return logId;
}

/// �A�N�Z�X��ʂ𕶎���ɕϊ�����
inline LPCTSTR AccessType2Message( ACCESS_TYPE type )
{
	LPCTSTR text = L"";
	switch( type ) {
	case ACCESS_LOGIN:				text = L"���O�C��";				break;
	case ACCESS_MAIN:				text = L"���C��";				break;
	case ACCESS_DIARY:				text = L"���L���e";				break;
	case ACCESS_NEWS:				text = L"�j���[�X";				break;
	case ACCESS_BBS:				text = L"�R�~������";			break;
	case ACCESS_ENQUETE:			text = L"�A���P�[�g";			break;
	case ACCESS_EVENT:				text = L"�C�x���g";				break;
	case ACCESS_EVENT_MEMBER:		text = L"�C�x���g�Q���҈ꗗ";	break;
	case ACCESS_MYDIARY:			text = L"���L";					break;
	case ACCESS_ADDDIARY:			text = L"���L���e";				break;
	case ACCESS_MESSAGE:			text = L"���b�Z�[�W";			break;
	case ACCESS_IMAGE:				text = L"�摜";					break;
	case ACCESS_MOVIE:				text = L"����";					break;
	case ACCESS_DOWNLOAD:			text = L"�_�E�����[�h";			break;
	case ACCESS_PROFILE:			text = L"�v���t�B�[��";			break;
	case ACCESS_COMMUNITY:			text = L"�R�~���j�e�B";			break;
	case ACCESS_PLAIN:				text = L"�ėpURL";				break;

	case ACCESS_LIST_INTRO:			text = L"�Љ";				break;

	case ACCESS_LIST_DIARY:			text = L"���L�ꗗ";				break;
	case ACCESS_LIST_NEW_COMMENT:	text = L"�V���R�����g�ꗗ";		break;
	case ACCESS_LIST_COMMENT:		text = L"�R�����g�ꗗ";			break;
	case ACCESS_LIST_NEWS:			text = L"�j���[�X�ꗗ";			break;
	case ACCESS_LIST_FAVORITE:		text = L"���C�ɓ���";			break;
	case ACCESS_LIST_FRIEND:		text = L"�}�C�~�N�ꗗ";			break;
	case ACCESS_LIST_COMMUNITY:		text = L"�R�~���j�e�B�ꗗ";		break;
	case ACCESS_LIST_NEW_BBS_COMMENT:text = L"�R�~���R�����g�L������";		break;
	case ACCESS_LIST_NEW_BBS:		text = L"�R�~�������ꗗ";		break;
	case ACCESS_LIST_BBS:			text = L"�g�s�b�N�ꗗ";			break;
	case ACCESS_LIST_MYDIARY:		text = L"���L�ꗗ";				break;
	case ACCESS_LIST_FOOTSTEP:		text = L"������";				break;
	case ACCESS_LIST_MESSAGE_IN:	text = L"���b�Z�[�W(��M��)";	break;
	case ACCESS_LIST_MESSAGE_OUT:	text = L"���b�Z�[�W(���M��)";	break;
	case ACCESS_LIST_BOOKMARK:		text = L"�u�b�N�}�[�N";			break;
	case ACCESS_LIST_CALENDAR:		text = L"�J�����_�[";			break;  //icchu�ǉ�

	case ACCESS_GROUP_COMMUNITY:	text = L"�R�~���j�e�BG";		break;
	case ACCESS_GROUP_MESSAGE:		text = L"���b�Z�[�WG";			break;
	case ACCESS_GROUP_MYDIARY:		text = L"���LG";				break;
	case ACCESS_GROUP_NEWS:			text = L"�j���[�XG";			break;
	case ACCESS_GROUP_OTHERS:		text = L"���̑�G";				break;
	case ACCESS_GROUP_TWITTER:		text = L"TwitterG";				break;

	// POST �n
	case ACCESS_POST_CONFIRM_COMMENT:		text = L"�R�����g���e�i�m�F�j";		break;
	case ACCESS_POST_ENTRY_COMMENT:			text = L"�R�����g���e�i�����j";		break;
	case ACCESS_POST_CONFIRM_REPLYMESSAGE:	text = L"���b�Z�[�W�ԐM�i�m�F�j";	break;
	case ACCESS_POST_ENTRY_REPLYMESSAGE:	text = L"���b�Z�[�W�ԐM�i�����j";	break;
	case ACCESS_POST_CONFIRM_NEWMESSAGE:	text = L"�V�K���b�Z�[�W�i�m�F�j";	break;
	case ACCESS_POST_ENTRY_NEWMESSAGE:		text = L"�V�K���b�Z�[�W�i�����j";	break;
	case ACCESS_POST_CONFIRM_NEWDIARY:		text = L"���L���e�i�m�F�j";			break;
	case ACCESS_POST_ENTRY_NEWDIARY:		text = L"�R�����g���e�i�����j";		break;

	// Twitter �n
	case ACCESS_TWITTER_FRIENDS_TIMELINE:	text = L"�^�C�����C��";	break;
	case ACCESS_TWITTER_FAVORITES:			text = L"���C�ɓ���";	break;
	case ACCESS_TWITTER_DIRECT_MESSAGES:	text = L"���b�Z�[�W";	break;
	case ACCESS_TWITTER_UPDATE:				text = L"�X�V";			break;

	case ACCESS_TWITTER_USER:				text = L"Twitter����";	break;
	case ACCESS_TWITTER_NEW_DM:				text = L"���b�Z�[�W���M";	break;

	case ACCESS_TWITTER_FAVOURINGS_CREATE:	text = L"���C�ɓ���o�^";	break;
	case ACCESS_TWITTER_FAVOURINGS_DESTROY:	text = L"���C�ɓ���폜";	break;

	case ACCESS_TWITTER_FRIENDSHIPS_CREATE:	text = L"�t�H���[�o�^";	break;
	case ACCESS_TWITTER_FRIENDSHIPS_DESTROY:text = L"�t�H���[����";	break;


	case ACCESS_INVALID:			text = L"<invalid>";			break;
	default:						text = L"<unknown>";			break;
	}
	return text;
}

}