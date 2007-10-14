#pragma once

#include "locale.h"
#include "JRegex20050421/JRegex.h"
#include <string>
#include <vector>

/**
 * 正規表現ライブラリ JRegex のラッパー
 */
class MyRegex
{
public:
	/// 正規表現の検索結果を表すクラス
	class Result {
	public:
		std::wstring str;	///< マッチした文字列
		u_int start;		///< マッチした位置
		u_int end;			///< マッチ終了位置

		/// コンストラクタ
		Result( u_int start_, u_int end_, const std::wstring& str_ )
			: start(start_), end(end_), str(str_)
		{}
	};

private:
	// JRegex のオブジェクト
	jreg* reg;

public:
	/**
	 * exec の結果
	 */
	std::vector<Result> results;

	/// コンストラクタ
	MyRegex() : reg(NULL) {}

	/// デストラクタ
	~MyRegex() {
		if( reg != NULL ) {
			jreg_free(reg);
		}
	}

	/**
	 * 正規表現をコンパイルする
	 */
	bool compile( LPCTSTR pattern, jreg_opt opt=JREG_NORMAL )
	{
		// JRegex を使ってみる
		reg = jreg_comp( (const t_code*)pattern, opt );
		if( reg ) {
			return true;
		}else{
			return false;
		}
	}

	/**
	 * コンパイル済みかどうかを返す
	 */
	bool isCompiled() {
		return reg!=NULL;
	}

#ifdef __AFX_H__
	bool replaceAll( CString& target, const std::wstring& replace ) {
		std::wstring buf = target;
		if( replaceAll( buf, replace ) ) {
			target = buf.c_str();
			return true;
		}else{
			return false;
		}
	}
#endif

	/**
	 * 正規表現にマッチした部分文字列の置換。
	 *
	 * jreg_replace ではなく jreg_exec で独自に実装する。
	 *
	 * @param target  [in/out] 置換対象文字列。
	 * @param replace [in]     置換パターン。
	 *                         {1}～{10} を() でマッチした部分文字列に置換する。
	 */
	bool replaceAll( std::wstring& target, const std::wstring& replace ) {

		std::wstring rest = target;
		target = L"";

		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
			if( !this->exec(rest.c_str()) ) {
				// 未発見。
				// 残りの文字列を追加して終了。
				target += rest;
				break;
			}

			// 発見。

			// マッチ文字列全体の左側を出力
			target.append( rest.c_str(), this->results[0].start );

			// replace パターンで置換
			size_t len = replace.size();
			for( size_t i=0; i<len; i++ ) {
				if( replace[i] == '{' ) {
					// {N} なら置換

					// N 抽出
					std::wstring str_n;
					i++;
					for( ; i<len; i++ ) {
						int ch = replace[i];
						if( ch == '}' ) {
							// '}' を発見したので終了
							break;
						}
						str_n.push_back( ch );
					}
					if( i==len ) {
						// '}' 未発見なのでそのまま追加
						target.push_back( '{' );
						target.append( str_n );
					}else{
						// '}' 発見。

						// str_n が [0-9] なら数値変換
						size_t j=0;
						size_t str_n_len=str_n.size();
						for( ; j<str_n_len; j++ ) {
							if( !isdigit(str_n[j]) ) {
								break;
							}
						}
						if( j==str_n_len ) {
							// [0-9] なので str_n を数値変換
							size_t n = _wtoi(str_n.c_str());
							
							// n が 0 であればマッチ文字列全体に置換。
							// n がマッチ文字列数未満であれば、置換。
							if( n == 0 ) {
								// マッチ文字列全体を追加
								target.append( results[0].str );
							}else if( n < results.size() ) {
								// マッチ文字列数未満なので、置換する。
								target.append( results[n].str );
							}else{
								// マッチ文字列数以上なので、置換せずに追加
								target.push_back( '{' );
								target.append( str_n );
								target.push_back( '}' );
							}
						}else{
							// [0-9] 以外が含まれていたので置換せずに追加
							target.push_back( '{' );
							target.append( str_n );
							target.push_back( '}' );
						}
					}
				}else{
					// '{' 以外なのでそのまま追加
					target.push_back( replace[i] );
				}
			}

			// ターゲットを更新。
			rest.erase( 0, results[0].end );
		}

		return true;
	}

	/**
	 * マッチ実行。results に検索結果が格納される。
	 */
	bool exec( LPCTSTR target )
	{
		if( reg == NULL ) {
			return false;
		}

		results.clear();

		PATTERN_STR pat;
		pat.PATTERN_TCODE = (t_code*)target;
		if( jreg_exec(reg,pat,0,(u_int)wcslen(target),JREG_NORMAL) ) {
			// 成功。
			// 検索結果を results にセット。
			for( u_int i=0; i<reg->backrefcnt; i++ ) {
				u_int start = reg->backref[i].start;
				u_int width = reg->backref[i].match-1;
				if( width <= 0 ) {
					results.push_back( Result( -1, -1, std::wstring() ) );
				}else{
					results.push_back( 
						Result( start,
								start+width,
								std::wstring(target+start,width) ) );
				}
			}

			return true;
		}else{
			return false;
		}
	}
};
