/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MyRegex.h"
#include "EmojiMap.h"

/// 表示用フィルタ
class ViewFilter {
public:
	/**
	 * 絵文字コード [m:xx] の代替文字列への変換。
	 */
	static void ReplaceEmojiCodeToText( CString& line, EmojiMapList& emojiMap )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"(\\[m:[0-9]+?\\])" )) {
			return;
		}

		// ((喫煙)) 形式に置換する
		CString target = line;
		line = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
			if( !reg.exec(target) || reg.results.size() != 2 ) {
				// 未発見。
				// 残りの文字列を代入して終了。
				line += target;
				break;
			}

			// 発見。
			std::vector<MyRegex::Result>& results = reg.results;

			// マッチ文字列全体の左側を出力
			line.Append( target, results[0].start );

			// 絵文字を探索する
			const std::wstring& emoji_number = results[1].str;
			size_t n = emojiMap.size();
			for (size_t j=0; j<n; j++) {
				if (emojiMap[j].code == emoji_number.c_str()) {
					line.AppendFormat( L"((%s))", emojiMap[j].text );
					break;
				}
			}

			// ターゲットを更新。
			target.Delete( 0, results[0].end );
		}
	}

	/**
	 * 絵文字コード [m:xx] の分離
	 */
	static void ReplaceEmojiCodeToRan2ImageTags( CString& line, CStringArray& bodyArray, EmojiMapList& emojiMap, CWnd* pWnd )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"(\\[m:[0-9]+?\\])" )) {
			return;
		}

		// ((喫煙)) 形式に置換する
		CString target = line;
		line = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
			if( !reg.exec(target) || reg.results.size() != 2 ) {
				// 未発見。
				// 残りの文字列を代入して終了。
				bodyArray.Add(target);
				break;
			}

			// 発見。
			std::vector<MyRegex::Result>& results = reg.results;

			// マッチ文字列全体の左側を出力
			bodyArray.Add( CString( target, results[0].start ) );

			// 絵文字を追加
			const std::wstring& emoji_code = results[1].str;
			size_t n = emojiMap.size();
			for (size_t j=0; j<n; j++) {
				if (emojiMap[j].code == emoji_code.c_str()) {
					CString path = util::MakeImageLogfilePathFromUrl( emojiMap[j].url );
					int imageIndex = theApp.m_imageCache.GetImageIndex(path);
					if (imageIndex == -1) {
						// 未ロードなのでロードする
						CMZ3BackgroundImage image(L"");
						if (!image.load( path )) {
							// ロードエラー
							break;
						}

						// リサイズして画像キャッシュに追加する。
						imageIndex = theApp.AddImageToImageCache(pWnd, image, path);
					}
					bodyArray.Add( util::FormatString( L"[m:%d]", imageIndex ) );
					break;
				}
			}

			// ターゲットを更新。
			target.Delete( 0, results[0].end );
		}
	}

	/**
	 * タグの変換
	 * bodyArrayをサーチしてHTMLタグをらんらんビュータグに変換する
	 * 非効率版
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
	 * タグの変換
	 * blockquoteタグの前を強制的に改行する
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
	 * 絵文字コード [m:xx] の分離
	 * および、HTMLタグをらんらんビュータグに変換
	 * 性能対策版
	 */
	static void ReplaceHTMLTagToRan2Tags( CString& line, CStringArray& bodyArray, EmojiMapList& emojiMap, CWnd* pWnd )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"(\\[m:[0-9]+?\\])" )) {
			return;
		}

		// ((喫煙)) 形式に置換する
		CString target = line;
		line = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
			int emojioffset = -1;
			int tagoffset = -1;
			CString toTag = L"";
			int taglen = 0;

			// 絵文字検索
			if( reg.exec(target) && reg.results.size() == 2 ) {
				// 発見。
				emojioffset = reg.results[0].start;
			}

			// タグ検索
			tagoffset = target.Find( L"<" );
			for( int j=0 ; j<MZ3_INFINITE_LOOP_MAX_COUNT; j++ ){	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
				if( tagoffset < 0 ){
					break;
				}
				if( emojioffset >=0 && tagoffset > emojioffset ) {
					// 絵文字より後ろなら未発見扱い
					tagoffset = -1;
					break;
				}
				// タグの判定（これでいいのか？＞私
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
				// 未発見。
				// 残りの文字列を代入して終了。
				bodyArray.Add(target);
				break;

			} else if( tagoffset < 0 || ( emojioffset >=0 && emojioffset < tagoffset ) ){
				// 絵文字を先に発見。
				std::vector<MyRegex::Result>& results = reg.results;

				// マッチ文字列全体の左側を出力
				bodyArray.Add( CString( target, results[0].start ) );

				// 絵文字を追加
				const std::wstring& emoji_code = results[1].str;
				size_t n = emojiMap.size();
				for (size_t j=0; j<n; j++) {
					if (emojiMap[j].code == emoji_code.c_str()) {
						CString path = util::MakeImageLogfilePathFromUrl( emojiMap[j].url );
						int imageIndex = theApp.m_imageCache.GetImageIndex(path);
						if (imageIndex == -1) {
							// 未ロードなのでロードする
							CMZ3BackgroundImage image(L"");
							if (!image.load( path )) {
								// ロードエラー
								break;
							}

							// リサイズして画像キャッシュに追加する。
							imageIndex = theApp.AddImageToImageCache(pWnd, image, path);
						}
						bodyArray.Add( util::FormatString( L"[m:%d]", imageIndex ) );
						break;
					}
				}
				// ターゲットを更新。
				target.Delete( 0, results[0].end );

			} else {
				// タグを先に発見
				if( tagoffset > 0 ){
					// タグの左側を文字列として追加
					bodyArray.Add( target.Left( tagoffset ) );
					target.Delete( 0 , tagoffset );
				}
				// 変換後のタグを追加
				bodyArray.Add( toTag );

				// ターゲットを更新。
				target.Delete( 0 , taglen );

			}
		}
	}

	/**
	 * タグの変換
	 * らんらんビュータグを削除する
	 */
	static void RemoveRan2ViewTag( CString& string )
	{
		// らんらんビュータグを消去
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
