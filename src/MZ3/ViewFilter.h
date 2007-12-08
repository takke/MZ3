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
	 * �G�����R�[�h [m:xx] �� URL �ւ̕ϊ��B
	 */
	static void ReplaceEmojiCodeToInlineImageTags( CString& line, EmojiMapList& emojiMap )
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
					line.AppendFormat( L"<img src=\"%s\" alt=\"%s\"/>", emojiMap[j].url, emojiMap[j].text );
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
	static void ReplaceEmojiCodeToRan2ImageTags( CString& line, CStringArray& bodyArray, EmojiMapList& emojiMap )
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

						// �r�b�g�}�b�v�̒ǉ�
						CBitmap bm;
						bm.Attach( image.getHandle() );
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
};
