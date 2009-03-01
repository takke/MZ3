#ifndef __INC_JREGEX__
#define __INC_JREGEX__
#include "jectlib.h"
#include "JChar.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	//- ノード型
typedef struct _JNODE  JNODE;
typedef struct _JLOOP  JLOOP;
typedef struct _JBM    JBM;

	//- 比較文字列型定義
typedef union
{
	t_code (*PATTERN_FUNC)(u_int,void*); // 文字取得関数
	t_code *PATTERN_TCODE;               // 文字列ポインタ
}PATTERN_STR;

	//- 後方参照型定義
typedef struct _BACK_REF
{
	u_int start; // マッチ開始位置
	u_int match; // マッチ幅
}BACK_REF;

	//- 正規表現実行オプション
typedef enum _JREG_OPT
{
	JREG_NONE       = 0x0000, // オプション指定なし

	JREG_COMPOPT    = 0x00FF, // コンパイルオプションマスク
	JREG_STARTLINE  = 0x0001, // 改行直後を先頭として扱う
	JREG_ENDLINE    = 0x0002, // 改行直前を終端として扱う
	JREG_BINARYMODE = 0x0004, // バイナリモードで実行
	JREG_DFA        = 0x0008, // DFAコンパイルを行う
	JREG_MULTILINE  = JREG_STARTLINE | JREG_ENDLINE, // 複数行を扱う

	JREG_EXECOPT    = 0xFF00, // 実行オプションマスク
	JREG_ICASE      = 0x0100, // 大文字小文字無視
	JREG_SINGLESTEP = 0x0200, // 複数位置での検索/置き換えをしない
	JREG_STRINGFUNC = 0x0400, // 文字列をコールバック関数から取得
	JREG_USELASTRET = 0x0800, // 前回の検索結果を使用

	JREG_NORMAL     = JREG_ICASE, // デフォルトの設定で動作
}jreg_opt;

	//- 正規表現型定義
typedef struct _JREG
{
	t_code      *regstr;    // 正規表現の保存用コピー
	JNODE       *node;      // NFA先頭ノード
	bool         isnfaonly; // NFAコンパイルサポート有無
	JNODE       *regd;      // DFAテーブル
	u_int        nodemax;   // DFAテーブル要素数
	JBM         *bm;        // BM法テーブル
	u_int        reglen;    // 検索対象文字数
	PATTERN_STR  Pattern;   // 検索対象 (文字列/関数ポインタ)
	jreg_opt     execopt;   // 実行時オプション
	jreg_opt     compopt;   // コンパイルオプション
	void        *usrval;    // ユーザ定義値
	
	u_int        backrefcnt; // 後方参照回数
	BACK_REF    *backref;    // 後方参照保存バッファ
	u_int        closurecnt; // ループ回数
	JLOOP       *closure;    // ループ回数保存バッファ
}jreg;

	//- 置き換え情報型定義
typedef struct _JREG_REP
{
	t_code *repstr;  // 置き換え後文字列パターン
	t_code *repbuf;  // 置き換え後文字列取得バッファ
	u_int   bufsize; // 必要バッファ長
	u_int   maxsize; // バッファ最大長
	u_int   start;   // 置き換え開始位置
	u_int   match;   // 置き換えマッチ幅
}jreg_rep;


jreg* jreg_comp   (const t_code *regstr, jreg_opt opt);
bool  jreg_exec   (jreg *reg,                PATTERN_STR Pattern, u_int start, u_int len, jreg_opt opt);
bool  jreg_replace(jreg *reg, jreg_rep *rep, PATTERN_STR Pattern, u_int start, u_int len, jreg_opt opt);
void  jreg_free   (jreg *reg);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INC_JREGEX__
