/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "PostData.h"
#include "MixiData.h"

namespace mixi {

/// PostData �𐶐�����N���X�Q�̊��N���X
class PostDataGeneratorBase {
public:

	/**
	 * �摜�f�[�^�𖄂ߍ���
	 */
	static bool embedPhotoData( CPostData& post, const char* form_name, LPCTSTR photo_filepath ) {

		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );

		if( photo_filepath != NULL && wcslen(photo_filepath) > 0 ) {
			// �t�@�C������擾���Ė��ߍ���

			// Content-Disposition
			post.AppendPostBody( "Content-Disposition: form-data; name=\"" );
			post.AppendPostBody( form_name );
			post.AppendPostBody( "\"; " );
			post.AppendPostBody( "filename=\"photo.jpg\"" );
			post.AppendPostBodyWithCRLF( "" );

			// Content-Type
			post.AppendPostBodyWithCRLF( "Content-Type: image/jpeg" );

			// ��s
			post.AppendPostBodyWithCRLF( "" );

			// �t�@�C����ǂݍ��݁A���ߍ���
			if( !appendFile( post, photo_filepath ) ) {
				return false;
			}

			// ��s
			post.AppendPostBodyWithCRLF( "" );
		}else{
			// �t�@�C�����w��Ȃ̂Ŗ��ߍ��܂Ȃ�
			// ��̓d���𖄂ߍ���

			// Content-Disposition
			post.AppendPostBody( "Content-Disposition: form-data; name=\"" );
			post.AppendPostBody( form_name );
			post.AppendPostBody( "\"; " );
			post.AppendPostBody( "filename=\"\"" );
			post.AppendPostBodyWithCRLF( "" );

			// Content-Type
			post.AppendPostBodyWithCRLF( "Content-Type: application/octet-stream" );

			// ��s
			post.AppendPostBodyWithCRLF( "" );
		}

		return true;
	}

	/**
	 * �t�@�C����ǂݍ��݁A���ߍ���
	 */
	static bool appendFile( CPostData& post, LPCTSTR photo_filepath ) {

		// �t�@�C����ǂݍ��݁A���̂܂ܖ��ߍ���
		FILE* fp = _wfopen( photo_filepath, L"rb" );
		if( fp == NULL ) {
			return false;
		}

#define	BUF_SIZE 1024
		char buf[ BUF_SIZE ];
		while( !feof( fp ) ) {
			int len = fread( buf, 1, BUF_SIZE, fp );
			if( ferror( fp ) ) {
				// �ǂݍ��݃G���[
				fclose( fp );
				return false;
			}

			post.AppendPostBody( buf, len );
		}

		fclose( fp );
		return true;
	}
};

/**
 * �R�����g���e�̊m�F��ʗp POST �d�������N���X�i�������݉�ʁˊm�F��ʃ{�^���j
 * 
 * add_bbs_comment.pl
 */
class CommentConfirmGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param mixi				CMixiData �I�u�W�F�N�g
	 * @param msg				���[�U�����͂������b�Z�[�W
	 * @param photo1_filepath	photo1 �̃t�@�C���p�X
	 * @param photo2_filepath	photo2 �̃t�@�C���p�X
	 * @param photo3_filepath	photo3 �̃t�@�C���p�X
	 */
	static bool generate( CPostData& post, const CMixiData& mixi, 
						  LPCTSTR msg,
						  LPCTSTR photo1_filepath = NULL, 
						  LPCTSTR photo2_filepath = NULL, 
						  LPCTSTR photo3_filepath = NULL )
	{
		// ����ʂ� hidden �^�O�Ƃ��Ė��ߍ��ރ��b�Z�[�W��ۑ����Ă���
		post.SetComment(msg);

		// POST �d���̐���
		post.ClearPostBody();

		CString strContentType = mixi.GetContentType();
		if (strContentType == CONTENT_TYPE_MULTIPART) {
			// �}���`�p�[�g�̎��͉��L�d���𑗐M����
			post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
			post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"comment\"" );
			post.AppendPostBodyWithCRLF( "" );
			post.AppendPostBodyWithCRLF( "msg" );

			if( !embedPhotoData( post, "photo1", photo1_filepath ) ) return false;
			if( !embedPhotoData( post, "photo2", photo2_filepath ) ) return false;
			if( !embedPhotoData( post, "photo3", photo3_filepath ) ) return false;

			post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e--" );
		}
		else {
			// urlencode�̏ꍇ�́A�G���R�[�h���ꂽ���b�Z�[�W�����̂܂ܐݒ�
			CString body;
			switch (mixi.GetAccessType()) {
			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
			case ACCESS_NEIGHBORDIARY:
				body.Format(L"owner_id=%d&comment_body=%s", mixi.GetOwnerID(), msg);
				break;
			case ACCESS_ENQUETE:
				body.Format(L"submit=main&comment=%s", msg);
				break;
			}

			post.AppendPostBodyWithCRLF( body );
		}

		// Content-Type ��ݒ肷��
		post.SetContentType( strContentType );

		return true;
	}
};

/**
 * �R�����g���e�̏������݉�ʗp POST �d�������N���X�i�m�F��ʁb�������݃{�^�������j
 * 
 * add_bbs_comment.pl
 */
class CommentRegistGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param mixi				CMixiData �I�u�W�F�N�g
	 */
	static bool generate( CPostData& post, const CMixiData& mixi )
	{
		// POST �d���̐���
		post.ClearPostBody();

		switch( mixi.GetAccessType() ) {
		case ACCESS_BBS:
		case ACCESS_EVENT:
		case ACCESS_EVENT_JOIN:
			post.AppendPostBody( "submit=confirm&packed=" );
			post.AppendPostBody( post.GetPacked() );
			post.AppendPostBody( "&comment=" );
			post.AppendPostBody( post.GetComment() );
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post.GetPostKey() );
			break;

		case ACCESS_ENQUETE:
			post.AppendPostBody( "submit=confirm&comment=" );
			post.AppendPostBody( post.GetComment() );
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post.GetPostKey() );
			break;

		case ACCESS_DIARY:
		case ACCESS_MYDIARY:
		case ACCESS_NEIGHBORDIARY:
			post.AppendPostBody( "submit=confirm&comment_body=" );
			post.AppendPostBody( post.GetComment() );

      //2007/06/22 �������イ mixi�d�l�ύX�Ή��@owner_id���v��Ȃ��Ȃ������ۂ�
			//post.AppendPostBody( "&owner_id=" );
			//post.AppendPostBody( util::int2str_a( mixi.GetOwnerID() ) );
      
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post.GetPostKey() );
			break;
		}

		// Content-Type ��ݒ肷��
		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * ���L���e�̊m�F��ʗp POST �d�������N���X�i�������݉�ʁˊm�F��ʃ{�^���j
 * 
 * add_diary.pl
 */
class DiaryConfirmGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param ownerId			���[�UID
	 * @param msg				���[�U�����͂������b�Z�[�W
	 * @param photo1_filepath	photo1 �̃t�@�C���p�X
	 * @param photo2_filepath	photo2 �̃t�@�C���p�X
	 * @param photo3_filepath	photo3 �̃t�@�C���p�X
	 */
	static bool generate( CPostData& post, LPCTSTR ownerId, 
						  LPCTSTR msg,
						  LPCTSTR photo1_filepath = NULL, 
						  LPCTSTR photo2_filepath = NULL, 
						  LPCTSTR photo3_filepath = NULL )
	{
		// ����ʂ� hidden �^�O�Ƃ��Ė��ߍ��ރ��b�Z�[�W��ۑ����Ă���
		post.SetComment(msg);

		// POST �d���̐���
		post.ClearPostBody();
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"news_id\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"id\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( ownerId ); // ���[�U�[�h�c
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"news_title\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"news_url\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"movie_id\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"movie_title\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"movie_url\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"submit\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "main" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"diary_title\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "title" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"diary_body\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "msg" );

		if( !embedPhotoData( post, "photo1", photo1_filepath ) ) return false;
		if( !embedPhotoData( post, "photo2", photo2_filepath ) ) return false;
		if( !embedPhotoData( post, "photo3", photo3_filepath ) ) return false;

		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e--" );

		// Content-Type ��ݒ肷��
		// Content-Type: multipart/form-data
		post.SetContentType( CONTENT_TYPE_MULTIPART );

		return true;
	}
};

/**
 * ���L���e�̏������݉�ʗp POST �d�������N���X�i�m�F��ʁb�������݃{�^�������j
 * 
 * add_diary.pl
 */
class DiaryRegistGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param title				�^�C�g��������(wchar_t)
	 */
	static bool generate( CPostData& post, LPCTSTR title , LPCTSTR viewlimit)
	{
		// POST �d���̐���
		post.ClearPostBody();
		post.AppendPostBody( "submit=confirm&packed=" );
		post.AppendPostBody( post.GetPacked() );
		post.AppendPostBody( "&post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&news_id=&id=" );
		post.AppendPostBody( theApp.m_loginMng.GetMixiOwnerID() );
		post.AppendPostBody( "&diary_title=" );

		// �^�C�g���� EUC-JP URL Encoded String �ɕϊ�����
		post.AppendPostBody( URLEncoder::encode_euc(title) );
		post.AppendPostBody( "&diary_body=" );
		post.AppendPostBody( post.GetComment() );
		post.AppendPostBody( "&news_title=&news_url=&movie_id=&movie_title=&movie_url=" );
		post.AppendPostBody( "&tag_id=" );
		post.AppendPostBody( viewlimit );

		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * ���b�Z�[�W�ԐM�̊m�F��ʗp POST �d�������N���X�i�������݉�ʁˊm�F��ʃ{�^���j
 * 
 * reply_message.pl
 */
class ReplyMessageConfirmGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param msg				���[�U�����͂������b�Z�[�W
	 */
	static bool generate( CPostData& post,
						  LPCTSTR title, LPCTSTR msg, int friend_id, LPCTSTR reply_message_id )
	{
		// ����ʂ� hidden �^�O�Ƃ��Ė��ߍ��ރ��b�Z�[�W��ۑ����Ă���
		post.SetComment(msg);

		// POST �d���̐���
		post.ClearPostBody();
		post.AppendPostBody( "mode=confirm_or_save&" );
		post.AppendPostBody( "from_show_friend=&" );
		
		// �^�C�g���� EUC-JP URL Encoded String �ɕϊ����Ė��ߍ���
		post.AppendPostBody( "subject=" );
		post.AppendPostBody( URLEncoder::encode_euc( title ) );
		post.AppendPostBody( "&" );
		
		post.AppendPostBody( "body=" );
		post.AppendPostBody( msg );
		post.AppendPostBody( "&" );
		
		post.AppendPostBody( "post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&" );
		
		post.AppendPostBody( "original_message_id=&" );
		
		post.AppendPostBody( "reply_message_id=" );
		post.AppendPostBody( reply_message_id );
		post.AppendPostBody( "&" );
		
		post.AppendPostBody( "id=" );
		post.AppendPostBody( util::int2str(friend_id) );

		// Content-Type ��ݒ肷��
		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * ���b�Z�[�W�ԐM�̏������݉�ʗp POST �d�������N���X�i�m�F��ʁb�������݃{�^�������j
 * 
 * reply_message.pl
 */
class ReplyMessageRegistGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param title				�^�C�g��������(wchar_t)
	 */
	static bool generate( CPostData& post, LPCTSTR title, int friend_id, LPCTSTR reply_message_id )
	{
		// POST �d���̐���
		post.ClearPostBody();

		post.AppendPostBody( "from_show_friend=&mode=commit_or_edit&" );

		// �^�C�g���� EUC-JP URL Encoded String �ɕϊ����Ė��ߍ���
		post.AppendPostBody( "subject=" );
		post.AppendPostBody( URLEncoder::encode_euc( title ) );
		post.AppendPostBody( "&" );

		post.AppendPostBody( "body=" );
		post.AppendPostBody( post.GetComment() );
		post.AppendPostBody( "&" );

		post.AppendPostBody( "post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&" );

		post.AppendPostBody( "original_message_id=&" );
		
		post.AppendPostBody( "reply_message_id=" );
		post.AppendPostBody( reply_message_id );
		post.AppendPostBody( "&" );

		post.AppendPostBody( "id=" );
		post.AppendPostBody( util::int2str(friend_id) );

		post.AppendPostBody( "&yes=%A1%A1%C1%F7%A1%A1%BF%AE%A1%A1" );

		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * �V�K���b�Z�[�W�̊m�F��ʗp POST �d�������N���X�i�������݉�ʁˊm�F��ʃ{�^���j
 * 
 * send_message.pl
 */
class NewMessageConfirmGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param msg				���[�U�����͂������b�Z�[�W
	 */
	static bool generate( CPostData& post,
						  LPCTSTR title, 
						  LPCTSTR msg, 
						  int friend_id )
	{
		// ����ʂ� hidden �^�O�Ƃ��Ė��ߍ��ރ��b�Z�[�W��ۑ����Ă���
		post.SetComment(msg);

		// POST �d���̐���
		post.ClearPostBody();
		post.AppendPostBody( "mode=confirm_or_save&" );
		post.AppendPostBody( "from_show_friend=&" );
		
		post.AppendPostBody( "subject=" );
		post.AppendPostBody( URLEncoder::encode_euc(title) );
		post.AppendPostBody( "&" );
		
		post.AppendPostBody( "body=" );
		post.AppendPostBody( msg );
		post.AppendPostBody( "&" );
		
		post.AppendPostBody( "post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&" );

		post.AppendPostBody( "original_message_id=&" );
		post.AppendPostBody( "reply_message_id=&" );
		post.AppendPostBody( "id=" );
		post.AppendPostBody( util::int2str(friend_id) );

		// Content-Type ��ݒ肷��
		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}

/*	static CStringA generateNewPostKey() {
		CStringA postKey;

		const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
		srand( (int)GetTickCount() );
		for( int i=0; i<22; i++ ) {
			int idx = rand() % 36;

			postKey.AppendChar( chars[idx] );
		}

		return postKey;
	}
*/
};

/**
 * �V�K���b�Z�[�W�̏������݉�ʗp POST �d�������N���X�i�m�F��ʁb�������݃{�^�������j
 * 
 * send_message.pl
 */
class NewMessageRegistGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData �̐���
	 *
	 * @param post				CPostData �I�u�W�F�N�g
	 * @param title				�^�C�g��������(wchar_t)
	 */
	static bool generate( CPostData& post, LPCTSTR title, int friend_id )
	{
		post.ClearPostBody();

		post.AppendPostBody( "from_show_friend=&" );
		post.AppendPostBody( "mode=commit_or_edit&" );

		// �^�C�g���� EUC-JP URL Encoded String �ɕϊ����Ė��ߍ���
		post.AppendPostBody( "subject=" );
		post.AppendPostBody( URLEncoder::encode_euc( title ) );
		post.AppendPostBody( "&" );

		// �{���𖄂ߍ���
		post.AppendPostBody( "body=" );
		post.AppendPostBody( post.GetComment() );
		post.AppendPostBody( "&" );

		post.AppendPostBody( "post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&" );

		post.AppendPostBody( "id=" );
		post.AppendPostBody( util::int2str(friend_id) );
		post.AppendPostBody( "&" );
		post.AppendPostBody( "original_message_id=&" );
		post.AppendPostBody( "reply_message_id=&" );

		post.AppendPostBody( "yes=%C1%F7%BF%AE%A4%B9%A4%EB" );

		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );
		return true;
	}
};


}
