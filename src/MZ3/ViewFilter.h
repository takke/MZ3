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
	 * 絵文字コード [m:xx] の URL への変換。
	 */
	static void ReplaceEmojiCodeToInlineImageTags( CString& line, EmojiMapList& emojiMap )
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
					line.AppendFormat( L"<img src=\"%s\" alt=\"%s\"/>", emojiMap[j].url, emojiMap[j].text );
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
	static void ReplaceEmojiCodeToRan2ImageTags( CString& line, CStringArray& bodyArray, EmojiMapList& emojiMap )
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

						// ビットマップの追加
						CBitmap bm;
						bm.Attach( image.getHandle() );
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
};
