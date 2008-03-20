/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MyRegex.h"
#include "EmojiMap.h"

/// �\���p�t�B���^
class ViewFilter {
public:
	/**
	 * �G�����R�[�h [m:xx] �̑�֕�����ւ̕ϊ��B
	 */
	static void ReplaceEmojiCodeToText( CString& line, EmojiMapList& emojiMap )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"(\\[m:[0-9]+?\\])" )) {
			return;
		}

		// ((�i��)) �`���ɒu������
		CString target = line;
		line = L"";
		for( int i=0; i<1000; i++ ) {	// 1000 �͖������[�v�h�~
			if( !reg.exec(target) || reg.results.size() != 2 ) {
				// �������B
				// �c��̕�����������ďI���B
				line += target;
				break;
			}

			// �����B
			std::vector<MyRegex::Result>& results = reg.results;

			// �}�b�`������S�̂̍������o��
			line.Append( target, results[0].start );

			// �G������T������
			const std::wstring& emoji_number = results[1].str;
			size_t n = emojiMap.size();
			for (size_t j=0; j<n; j++) {
				if (emojiMap[j].code == emoji_number.c_str()) {
					line.AppendFormat( L"((%s))", emojiMap[j].text );
					break;
				}
			}

			// �^�[�Q�b�g���X�V�B
			target.Delete( 0, results[0].end );
		}
	}

	/**
	 * �G�����R�[�h [m:xx] �̕���
	 */
	static void ReplaceEmojiCodeToRan2ImageTags( CString& line, CStringArray& bodyArray, EmojiMapList& emojiMap, CWnd* pWnd )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"(\\[m:[0-9]+?\\])" )) {
			return;
		}

		// ((�i��)) �`���ɒu������
		CString target = line;
		line = L"";
		for( int i=0; i<1000; i++ ) {	// 1000 �͖������[�v�h�~
			if( !reg.exec(target) || reg.results.size() != 2 ) {
				// �������B
				// �c��̕�����������ďI���B
				bodyArray.Add(target);
				break;
			}

			// �����B
			std::vector<MyRegex::Result>& results = reg.results;

			// �}�b�`������S�̂̍������o��
			bodyArray.Add( CString( target, results[0].start ) );

			// �G������ǉ�
			const std::wstring& emoji_code = results[1].str;
			size_t n = emojiMap.size();
			for (size_t j=0; j<n; j++) {
				if (emojiMap[j].code == emoji_code.c_str()) {
					CString path = util::MakeImageLogfilePathFromUrl( emojiMap[j].url );
					int imageIndex = theApp.m_imageCache.GetImageIndex(path);
					if (imageIndex == -1) {
						// �����[�h�Ȃ̂Ń��[�h����
						CMZ3BackgroundImage image(L"");
						if (!image.load( path )) {
							// ���[�h�G���[
							break;
						}

						// 16x16 �Ƀ��T�C�Y����B
						CMZ3BackgroundImage resizedImage(L"");
						util::MakeResizedImage( pWnd, resizedImage, image );

						// �r�b�g�}�b�v�̒ǉ�
						CBitmap bm;
						bm.Attach( resizedImage.getHandle() );
						imageIndex = theApp.m_imageCache.Add( &bm, (CBitmap*)NULL, path );
					}
					bodyArray.Add( util::FormatString( L"[m:%d]", imageIndex ) );
					break;
				}
			}

			// �^�[�Q�b�g���X�V�B
			target.Delete( 0, results[0].end );
		}
	}

	/**
	 * �^�O�̕ϊ�
	 * bodyArray���T�[�`����HTML�^�O�������r���[�^�O�ɕϊ�����
	 */
	static void ReplaceHTMLTagToRan2ImageTags( const CString fromTag, const CString toTag, CStringArray* bodyArray )
	{

		size_t taglen = fromTag.GetLength();

		for( int i=0 ; i<= bodyArray->GetUpperBound() ; i++ ){
			CString line;
			line = bodyArray->GetAt( i );

			int idx = 0;
			do {
				idx = line.Find( fromTag , idx );
				if( idx < 0 ) {
					break;
				} else {
					if( idx > 0 ){
						bodyArray->InsertAt( i++ , line.Left( idx ) );
						line.Delete( 0 , idx );
					}
					bodyArray->InsertAt( i++ , toTag );
					line.Delete( 0 , taglen );
					bodyArray->SetAt( i , line );
					idx += taglen;
				}
			} while( idx>= 0 && idx <= line.GetLength() );
		}
	}

	/**
	 * �^�O�̕ϊ�
	 * blockquote�^�O�̑O�������I�ɉ��s����
	 */
	static void InsertBRTagToBeforeblockquoteTag( CStringArray* bodyArray )
	{

		CString lastline = TEXT("");

		for( int i=0 ; i<= bodyArray->GetUpperBound() ; i++ ){
			CString line;
			line = bodyArray->GetAt( i );

			if( line == TEXT("[blockquote]") ||
				line == TEXT("[/blockquote]")) {
				if( lastline != TEXT("[br]") ){
					bodyArray->InsertAt( i++ , TEXT("[br]") );
					lastline = TEXT("[br]");;
					line = bodyArray->GetAt( i );
				}
			}

			lastline = line;
			
		}
	}
};
