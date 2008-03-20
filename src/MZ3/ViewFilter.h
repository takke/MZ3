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
		for( int i=0; i<1000; i++ ) {	// 1000 は無限ループ防止
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
		for( int i=0; i<1000; i++ ) {	// 1000 は無限ループ防止
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

						// 16x16 にリサイズする。
						CMZ3BackgroundImage resizedImage(L"");
						util::MakeResizedImage( pWnd, resizedImage, image );

						// ビットマップの追加
						CBitmap bm;
						bm.Attach( resizedImage.getHandle() );
						imageIndex = theApp.m_imageCache.Add( &bm, (CBitmap*)NULL, path );
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
	 * タグの変換
	 * blockquoteタグの前を強制的に改行する
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
