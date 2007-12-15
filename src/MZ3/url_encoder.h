/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
/**
 * URL エンコード用ユーティリティ
 */
#pragma once
#include "kfm.h"

/// URL エンコーダ
class URLEncoder
{
private:
	static unsigned long UrlEncode( unsigned char *csource,
							 unsigned char *cbuffer,
							 unsigned long lbuffersize )
	{
		unsigned long   llength;		// csource のサイズを格納
		unsigned long   lcount = 0;		// csource の読み込み位置カウンタ
		unsigned char   cbyte;			// 抜き出された 1 バイト分のデータを格納
		unsigned char   ctemp[4];		// 変換結果(1 文字分)一時格納バッファ
		unsigned long   lresultcount = 0; // cbuffer の書き込み位置カウンタ

		llength = (unsigned long)strlen((char*)csource); // csource の文字サイズを得る
		if (!llength) {
			return lresultcount;
		} // csource が 0 文字の場合、関数を抜ける
		if (lbuffersize < (llength * 3 + 1)) {
			return lresultcount;
		} // バッファサイズが足りない場合、関数を抜ける

		while (1) {
			cbyte = *(csource + lcount); // 1 バイトを抜き出す

			if (cbyte == 0x20) {	// 1 バイト半角スペース(" ")だった場合
				strncpy((char*)(cbuffer + lresultcount), "+", 2);	// "+" を cbuffer にコピー
				lcount++;											// 読み込みカウンタをインクリメント
				lresultcount++;										// 書き込みカウンタをインクリメント
			} else if (((cbyte >= 0x40) && (cbyte <= 0x5A)) ||		// @A-Z
				((cbyte >= 0x61) && (cbyte <= 0x7A)) ||				// a-z 
				((cbyte >= 0x30) && (cbyte <= 0x39)) ||				// 0-9 
				(cbyte == 0x2A) ||									// "*" 
				(cbyte == 0x2D) ||									// "-" 
				(cbyte == 0x2E) ||									// "." 
				(cbyte == 0x5F) )									// "_"
			{ 
				// 無変換文字だった場合
				strncpy((char*)(cbuffer + lresultcount), (char*)(csource + lcount), 2); // そのまま cbuffer にコピー
				lcount++;											// 読み込みカウンタをインクリメント
				lresultcount++;										// 書き込みカウンタをインクリメント
			} else {
				// その他の文字の場合
				sprintf((char*)ctemp, "%%%02X", cbyte);				// URL エンコード
				strncpy((char*)(cbuffer + lresultcount), (char*)ctemp, 4); // cbuffer にコピー
				lcount++;											// 読み込みカウンタをインクリメント
				lresultcount += 3;									// 書き込みカウンタを 3 増やす
			}
			if (lcount == llength) {
				break;
			} // 文字列の終端に達した場合、ループを抜ける
		}
		return lresultcount;	// cbuffer に書き込んだ文字列のサイズを返す
	}

	/**
	 * URL エンコード(EUC_JP)を行う。
	 *
	 * wchar_t : 文字セットは UNICODE (UCS-2)
	 * を
	 * wchar_t : 文字セットは全てASCII
	 * に変換する。
	 *
	 * 具体的には、
	 * wchar_t -> sjis -> euc -> url-encoded euc -> wchar_t (文字セットは全てASCII) を行う。
	 */
	static void encode_euc( LPCTSTR msg, TCHAR* url_encoded_text, int len )
	{
	#define MY_BUFFER_MAX	10000
		// msg (UNICODE) を sjis に変換
		kfm::kf_buf_type buf_sjis;
		buf_sjis.resize( MY_BUFFER_MAX+1 );
		wcstombs( (char*)&buf_sjis[0], msg, MY_BUFFER_MAX );

		// sjis を euc に変換
		kfm::kf_buf_type buf_euc;
		kfm::kfm filter( buf_sjis, buf_euc );
		filter.set_default_input_code( kfm::kfm::SJIS );
		filter.toeuc();

		// URLエンコード
		// euc 文字列 を URL エンコード
		char buf_euc_url[ MY_BUFFER_MAX+1 ];
		{
			memset(buf_euc_url, 0x00, sizeof(char) * (MY_BUFFER_MAX+1));
			UrlEncode((unsigned char*)&buf_euc[0], (unsigned char*)buf_euc_url, MY_BUFFER_MAX);
		}

		// URLエンコード済み文字列をワイド文字列へ変換
		mbstowcs(url_encoded_text, buf_euc_url, len);
	}

public:
	/**
	 * URL エンコード(EUC_JP)を行う。
	 */
	static CString encode_euc( LPCTSTR msg )
	{
		TCHAR url_encoded_text[MY_BUFFER_MAX+1];

		encode_euc( msg, url_encoded_text, MY_BUFFER_MAX );

		return url_encoded_text;
	}

};
