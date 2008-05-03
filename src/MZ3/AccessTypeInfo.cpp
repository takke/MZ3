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
	// �O���[�v�n
	m_map[ACCESS_GROUP_COMMUNITY] = AccessTypeInfo::Data(
		""
		, L"�R�~���j�e�BG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_MESSAGE] = AccessTypeInfo::Data(
		""
		, L"���b�Z�[�WG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_MYDIARY] = AccessTypeInfo::Data(
		""
		, L"���LG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_NEWS] = AccessTypeInfo::Data(
		""
		, L"�j���[�XG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_OTHERS] = AccessTypeInfo::Data(
		""
		, L"���̑�G"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_TWITTER] = AccessTypeInfo::Data(
		""
		, L"TwitterG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	m_map[ACCESS_LOGIN] = AccessTypeInfo::Data(
		"mixi"
		, L"���O�C��"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"���C��"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_DIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L���e"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_NEWS] = AccessTypeInfo::Data(
		"mixi"
		, L"�j���[�X"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~������"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_ENQUETE] = AccessTypeInfo::Data(
		"mixi"
		, L"�A���P�[�g"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_EVENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�C�x���g"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_EVENT_MEMBER] = AccessTypeInfo::Data(
		"mixi"
		, L"�C�x���g�Q���҈ꗗ"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MYDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_ADDDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L���e"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_IMAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"�摜"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MOVIE] = AccessTypeInfo::Data(
		"mixi"
		, L"����"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_DOWNLOAD] = AccessTypeInfo::Data(
		"mixi"
		, L"�_�E�����[�h"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_PROFILE] = AccessTypeInfo::Data(
		"mixi"
		, L"�v���t�B�[��"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_COMMUNITY] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~���j�e�B"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_PLAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"�ėpURL"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_INTRO] = AccessTypeInfo::Data(
		"mixi"
		, L"�Љ"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_DIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEW_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�V���R�����g�ꗗ"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g�ꗗ"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEWS] = AccessTypeInfo::Data(
		"mixi"
		, L"�j���[�X�ꗗ"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_FAVORITE] = AccessTypeInfo::Data(
		"mixi"
		, L"���C�ɓ���"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_FRIEND] = AccessTypeInfo::Data(
		"mixi"
		, L"�}�C�~�N�ꗗ"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_COMMUNITY] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~���j�e�B�ꗗ"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEW_BBS_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~���R�����g�L������"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEW_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�~�������ꗗ"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"�g�s�b�N�ꗗ"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_MYDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L�ꗗ"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_FOOTSTEP] = AccessTypeInfo::Data(
		"mixi"
		, L"������"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_MESSAGE_IN] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W(��M��)"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_MESSAGE_OUT] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W(���M��)"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_BOOKMARK] = AccessTypeInfo::Data(
		"mixi"
		, L"�u�b�N�}�[�N"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_CALENDAR] = AccessTypeInfo::Data(
		"mixi"
		, L"�J�����_�["
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	// POST �n
	m_map[ACCESS_POST_CONFIRM_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g���e�i�m�F�j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_CONFIRM_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W�ԐM�i�m�F�j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"���b�Z�[�W�ԐM�i�����j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_CONFIRM_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"�V�K���b�Z�[�W�i�m�F�j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"�V�K���b�Z�[�W�i�����j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_CONFIRM_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"���L���e�i�m�F�j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"�R�����g���e�i�����j"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	// Twitter �n
	m_map[ACCESS_TWITTER_FRIENDS_TIMELINE] = AccessTypeInfo::Data(
		"Twitter"
		, L"�^�C�����C��"
		, REQUEST_METHOD_POST// �^�C�����C���擾��POST�ɂ��Ă݂�
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_FAVORITES] = AccessTypeInfo::Data(
		"Twitter"
		, L"���C�ɓ���"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_DIRECT_MESSAGES] = AccessTypeInfo::Data(
		"Twitter"
		, L"���b�Z�[�W"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_UPDATE] = AccessTypeInfo::Data(
		"Twitter"
		, L"�X�V"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	m_map[ACCESS_TWITTER_USER] = AccessTypeInfo::Data(
		"Twitter"
		, L"Twitter����"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_NEW_DM] = AccessTypeInfo::Data(
		"Twitter"
		, L"���b�Z�[�W���M"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	m_map[ACCESS_TWITTER_FAVOURINGS_CREATE] = AccessTypeInfo::Data(
		"Twitter"
		, L"���C�ɓ���o�^"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_FAVOURINGS_DESTROY] = AccessTypeInfo::Data(
		"Twitter"
		, L"���C�ɓ���폜"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);

	m_map[ACCESS_TWITTER_FRIENDSHIPS_CREATE] = AccessTypeInfo::Data(
		"Twitter"
		, L"�t�H���[�o�^"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_FRIENDSHIPS_DESTROY] = AccessTypeInfo::Data(
		"Twitter"
		, L"�t�H���[����"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);

	m_map[ACCESS_INVALID] = AccessTypeInfo::Data(
		""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	return true;
}