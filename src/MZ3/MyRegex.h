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

