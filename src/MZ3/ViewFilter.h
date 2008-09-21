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
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
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
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
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

						// ���T�C�Y���ĉ摜�L���b�V���ɒǉ�����B
						imageIndex = theApp.AddImageToImageCache(pWnd, image, path);
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
	 * �������
	 */
	static void ReplaceHTMLTagToRan2ImageTags( const CString fromTag, const CString toTag, CStringArray* bodyArray )
	{

		size_t taglen = fromTag.GetLength();
		//TRACE( L"ReplaceHTMLTagToRan2ImageTags: %s %s %d\r\n" , fromTag , toTag , taglen );

		for( int i=0 ; i<= bodyArray->GetUpperBound() ; i++ ){
			CString line;
			//CString text;
			line = bodyArray->GetAt( i );

			int idx = 0;
			while( idx>=0 ) {
				idx = line.Find( fromTag , 0 );
				if( idx < 0 ) {
					break;
				} else {
					if( idx > 0 ){
						bodyArray->InsertAt( i++ , line.Left( idx ) );
						line.Delete( 0 , idx );
						//text = bodyArray->GetAt( i - 1 );
						//TRACE( L"InsertAt(i++,line.Left(idx));: %s\r\n" , text );
					}
					bodyArray->InsertAt( i++ , toTag );
					line.Delete( 0 , taglen );
					//TRACE( L"line.Delete(0,taglen);: %s\r\n" , line );
					//text = bodyArray->GetAt( i - 1 );
					//TRACE( L"InsertAt(i++,toTag);: %s\r\n" , text );
					bodyArray->SetAt( i , line );
					//text = bodyArray->GetAt( i );
					//TRACE( L"SetAt(i,line);: %s\r\n" , text );
				}
			} 
		}
		//for( int i=0 ; i<= bodyArray->GetUpperBound() ; i++ ){
		//	CString line;
		//	line = bodyArray->GetAt( i );
		//	TRACE( L"%5d: %s\r\n" , i , line );
		//}

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

			if( line == TEXT("<blockquote>") ||
				line == TEXT("</blockquote>")) {
				if( lastline != TEXT("<br>") ){
					bodyArray->InsertAt( i++ , TEXT("<br>") );
					lastline = TEXT("<br>");;
					line = bodyArray->GetAt( i );
				}
			}
			if( lastline == TEXT("</blockquote>") ) {
				if( line != TEXT("<br>") ) {
					bodyArray->InsertAt( i++ , TEXT("<br>") );
					lastline = TEXT("<br>");;
					line = bodyArray->GetAt( i );
				}
			}

			lastline = line;
			
		}
	}

	/**
	 * �G�����R�[�h [m:xx] �̕���
	 * ����сAHTML�^�O�������r���[�^�O�ɕϊ�
	 * ���\�΍���
	 */
	static void ReplaceHTMLTagToRan2Tags( CString& line, CStringArray& bodyArray, EmojiMapList& emojiMap, CWnd* pWnd )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"(\\[m:[0-9]+?\\])" )) {
			return;
		}

		// ((�i��)) �`���ɒu������
		CString target = line;
		line = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
			int emojioffset = -1;
			int tagoffset = -1;
			CString toTag = L"";
			int taglen = 0;

			// �G��������
			if( reg.exec(target) && reg.results.size() == 2 ) {
				// �����B
				emojioffset = reg.results[0].start;
			}

			// �^�O����
			tagoffset = target.Find( L"<" );
			for( int j=0 ; j<MZ3_INFINITE_LOOP_MAX_COUNT; j++ ){	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
				if( tagoffset < 0 ){
					break;
				}
				if( emojioffset >=0 && tagoffset > emojioffset ) {
					// �G���������Ȃ疢��������
					tagoffset = -1;
					break;
				}
				// �^�O�̔���i����ł����̂��H����
				if( target.Mid( tagoffset , 8 ) == L"<strong>" ){
					toTag = L"<b>";
					taglen = 8;
					break;
				} else if( target.Mid( tagoffset , 9 ) == L"</strong>" ){
					toTag = L"</b>";
					taglen = 9;
					break;
				} else if( target.Mid( tagoffset , 12 ) == L"<blockquote>" ){
					toTag = L"<blockquote>";
					taglen = 12;
					break;
				} else if( target.Mid( tagoffset ,13 ) == L"</blockquote>" ){
					toTag = L"</blockquote>";
					taglen = 13;
					break;
				} else if( target.Mid( tagoffset , 4 ) == L"<_a>" ){
					toTag = L"<a>";
					taglen = 4;
					break;
				} else if( target.Mid( tagoffset , 5) == L"</_a>" ){
					toTag = L"</a>";
					taglen = 5;
					break;
				} else if( target.Mid( tagoffset , 6) == L"<_img>" ){
					toTag = L"<img>";
					taglen = 6;
					break;
				} else if( target.Mid( tagoffset , 7 ) == L"</_img>" ){
					toTag = L"</img>";
					taglen = 7;
					break;
				} else if( target.Mid( tagoffset , 6 ) == L"<_mov>" ){
					toTag = L"<mov>";
					taglen = 6;
					break;
				} else if( target.Mid( tagoffset , 7 ) == L"</_mov>" ){
					toTag = L"</mov>";
					taglen = 7;
					break;
				} else {
					tagoffset = target.Find( L"<" , tagoffset + 1 );
				}
			}

			if( emojioffset < 0 && tagoffset < 0 ){
				// �������B
				// �c��̕�����������ďI���B
				bodyArray.Add(target);
				break;

			} else if( tagoffset < 0 || ( emojioffset >=0 && emojioffset < tagoffset ) ){
				// �G�������ɔ����B
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

							// ���T�C�Y���ĉ摜�L���b�V���ɒǉ�����B
							imageIndex = theApp.AddImageToImageCache(pWnd, image, path);
						}
						bodyArray.Add( util::FormatString( L"[m:%d]", imageIndex ) );
						break;
					}
				}
				// �^�[�Q�b�g���X�V�B
				target.Delete( 0, results[0].end );

			} else {
				// �^�O���ɔ���
				if( tagoffset > 0 ){
					// �^�O�̍����𕶎���Ƃ��Ēǉ�
					bodyArray.Add( target.Left( tagoffset ) );
					target.Delete( 0 , tagoffset );
				}
				// �ϊ���̃^�O��ǉ�
				bodyArray.Add( toTag );

				// �^�[�Q�b�g���X�V�B
				target.Delete( 0 , taglen );

			}
		}
	}

	/**
	 * �^�O�̕ϊ�
	 * �����r���[�^�O���폜����
	 */
	static void RemoveRan2ViewTag( CString& string )
	{
		// �����r���[�^�O������
		while( string.Replace( L"<_a>" , L"" ) );
		while( string.Replace( L"</_a>" , L"" ) );
		while( string.Replace( L"<_img>" , L"" ) );
		while( string.Replace( L"</_img>" , L"" ) );
		while( string.Replace( L"<_mov>" , L"" ) );
		while( string.Replace( L"</_mov>" , L"" ) );
		while( string.Replace( L"<blockquote>" , L"" ) );
		while( string.Replace( L"</blockquote>" , L"" ) );
		while( string.Replace( L"<strong>" , L"" ) );
		while( string.Replace( L"</strong>" , L"" ) );
	}

};
