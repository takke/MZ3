#include "jectlib.h"
#include "JRegex.h"
#include <tchar.h>
#include <locale.h>
#ifndef UNICODE
#include <stdio.h>
#include <string.h>
#endif // UNICODE

#ifdef _MSC_VER
#include <stdio.h>
#include <windows.h>
#include <imagehlp.h>
static LONG CALLBACK err_handler(EXCEPTION_POINTERS *e);
#endif // _MSC_VER

#if !defined(_MSC_VER) && defined(UNICODE)
//int WINAPI _tWinMain(int ins, int prev_ins, TCHAR *_argv, int show)
int _stdcall WinMain(int ins, int prev_ins, TCHAR *_argv, int show)
#else
int _tmain(s_int argc, TCHAR *argv[])
#endif // _MSC_VER && UNICODE
{
#define  t(reg,str,s1,r1)                                 {(t_code *)_T(reg), (t_code *)_T(str), null,              null,              true,  1, {{s1,r1},{0,0},  {0,0},  {0,0},  {0,0}}}
#define t1(reg,str,s1,r1)                                 {(t_code *)_T(reg), (t_code *)_T(str), null,              null,              true,  1, {{s1,r1},{0,0},  {0,0},  {0,0},  {0,0}}}
#define t2(reg,str,s1,r1,s2,r2)                           {(t_code *)_T(reg), (t_code *)_T(str), null,              null,              true,  2, {{s1,r1},{s2,r2},{0,0},  {0,0},  {0,0}}}
#define t3(reg,str,s1,r1,s2,r2,s3,r3)                     {(t_code *)_T(reg), (t_code *)_T(str), null,              null,              true,  3, {{s1,r1},{s2,r2},{s3,r3},{0,0},  {0,0}}}
#define t4(reg,str,s1,r1,s2,r2,s3,r3,s4,r4)               {(t_code *)_T(reg), (t_code *)_T(str), null,              null,              true,  4, {{s1,r1},{s2,r2},{s3,r3},{s4,r4},{0,0}}}
#define t5(reg,str,s1,r1,s2,r2,s3,r3,s4,r4,s5,r5)         {(t_code *)_T(reg), (t_code *)_T(str), null,              null,              true,  5, {{s1,r1},{s2,r2},{s3,r3},{s4,r4},{s5,r5}}}
#define  s(reg,str,rep,ret,s1,r1)                         {(t_code *)_T(reg), (t_code *)_T(str), (t_code *)_T(rep), (t_code *)_T(ret), true,  1, {{s1,r1},{0,0},  {0,0},  {0,0},  {0,0}}}
#define s1(reg,str,rep,ret,s1,r1)                         {(t_code *)_T(reg), (t_code *)_T(str), (t_code *)_T(rep), (t_code *)_T(ret), true,  1, {{s1,r1},{0,0},  {0,0},  {0,0},  {0,0}}}
#define s2(reg,str,rep,ret,s1,r1,s2,r2)                   {(t_code *)_T(reg), (t_code *)_T(str), (t_code *)_T(rep), (t_code *)_T(ret), true,  2, {{s1,r1},{s2,r2},{0,0},  {0,0},  {0,0}}}
#define s3(reg,str,rep,ret,s1,r1,s2,r2,s3,r3)             {(t_code *)_T(reg), (t_code *)_T(str), (t_code *)_T(rep), (t_code *)_T(ret), true,  3, {{s1,r1},{s2,r2},{s3,r3},{0,0},  {0,0}}}
#define s4(reg,str,rep,ret,s1,r1,s2,r2,s3,r3,s4,r4)       {(t_code *)_T(reg), (t_code *)_T(str), (t_code *)_T(rep), (t_code *)_T(ret), true,  4, {{s1,r1},{s2,r2},{s3,r3},{s4,r4},{0,0}}}
#define s5(reg,str,rep,ret,s1,r1,s2,r2,s3,r3,s4,r4,s5,r5) {(t_code *)_T(reg), (t_code *)_T(str), (t_code *)_T(rep), (t_code *)_T(ret), true,  5, {{s1,r1},{s2,r2},{s3,r3},{s4,r4},{s5,r5}}}
#define  x(reg,str)                                       {(t_code *)_T(reg), (t_code *)_T(str), null,              null,              true,  0, {{0,0},  {0,0},  {0,0},  {0,0},  {0,0}}}
#define sx(reg,str,rep,s1,r1)                             {(t_code *)_T(reg), (t_code *)_T(str), (t_code *)_T(rep), null,              true,  1, {{s1,r1},{0,0},  {0,0},  {0,0},  {0,0}}}
#define xc(reg)                                           {(t_code *)_T(reg), (t_code *)_T(""),  null,              null,              false, 0, {{0,0},  {0,0},  {0,0},  {0,0},  {0,0}}}
#define co(c)                                             {null,              null,              null,              null,              false, c, {{0,0},  {0,0},  {0,0},  {0,0},  {0,0}}}

	typedef struct _JREG_TESTCASE
	{
		t_code  *regstr;
		t_code  *str;
		t_code  *repstr;
		t_code  *repret;
		bool     comp;
		u_int    matnum;
		BACK_REF pat[5];
	}JREG_TESTCASE;

	static JREG_TESTCASE RegTest[] = {
		co(JREG_NORMAL),
			// normal-string test
		t("","abc",0,0),				// 全てのパターンにマッチする空表現
		t("abc","abc",0,3),
		x("abc","ab"),					// 文字列長の不足
		x("abc","abd"),
		t("abc","aabc",1,3),			// 先頭以外へのマッチ
		t("abc","abcd",0,3),
		t("abc","zabcd",1,3),
		t("abc", "bcababcab",4,3),
		t("abc", "BCABABCAB",4,3),
		t("ABC", "bcababcab",4,3),
			// or-pattern
		t("abc|","xyz",0,0),			// 空遷移を持つor表現
		t("abc|def","zabcx",1,3),		// ルートノードに対するor表現
		t("abc|def","zdefx",1,3),
		t("abc|def","abc|def",0,3),		// |を文字として扱っていないか
			// closure-pattern
		xc("*"),						// コンパイル不可
		xc("+"),						// コンパイル不可
		xc("a**"),						// コンパイル不可
		xc("a{"),						// コンパイル不可
		xc("a{}"),						// コンパイル不可
		xc("a{,}"),						// コンパイル不可
		xc("a{,1}"),					// コンパイル不可
		xc("ab|?cd"),					// コンパイル不可
		t("a}","a}",0,2),				// 閉じ括弧多い場合、文字として扱う
		t("a*","xyz",0,0),
		x("a+","xyz"),
		t("a*","xaay",0,0),
		t("a+","xaay",1,2),
		t("xa*","xaay",0,3),
		t("xa+","xaay",0,3),
		t("aax?y","xaay",1,3),
		t("a+b*c?","xaabbbbby",1,7),
		x("a{3}","aa"),
		t("a{3}","aaaa",0,3),
		t("a{3,}","aaaa",0,4),
		t("a{3,4}","aaaaa",0,4),
			// non_greeady_closure-pattern
		xc("*?"),									// コンパイル不可
		xc("+?"),									// コンパイル不可
		xc("a*??"),									// コンパイル不可
		xc("a+??"),									// コンパイル不可
		t("a??","a",0,0),							// non_greeady優先
		t("a*?","aaaa",0,0),
		t("a+?","aaaa",0,1),						// 最低一回のマッチを保証するnon_greeady
		t("a*?b","aaaab",0,5),
		t2("(a{3,}?){2,}?","aaaaaaa",0,6,3,3),		// 多重のnon_greeady
		t2("(a{3,}?b){2,}?","aaabaaaba",0,8,4,4),
			// more_greedy_closure-pattern
		xc("++"),						// コンパイル不可
		t("a++","aaa",0,3),
		t("a++b","aaab",0,4),
		x("a++ab","aaab"),				// 全てのパターンを消費し後が続かないmore_greedy
		t("a{3,5}+ab","aaaaaaab",1,7),
		x("a{3,5}+ab","aaaaab"),
			// union-pattern
		xc("(abc"),						// 閉じ括弧なし、コンパイル不可
		t("abc)","abc)",0,4),			// 閉じ括弧多い場合、文字として扱う
		t2("(a)bc","xabcy",1,3,1,1),
		t2("a(b)c","xabcy",1,3,2,1),
		t2("ab(c)","xabcy",1,3,3,1),
			// union & or-pattern
		t2("(abc|)","xyz",0,0,0,0),			// 空遷移を持つor表現
		t2("(abc|def)","zabcx",1,3,1,3),
		t2("(abc|def)","zdefx",1,3,1,3),
		t2("(abc|def)","abc|def",0,3,0,3),	// |を文字として扱っていないか
			// closure & union-pattern
		t2("(a)+a","aaaa",0,4,2,1),					// 後方参照の上書き
		t2("(a+)a","aaaa",0,4,0,3),					// 繰り返しのバックトラック
		t2("(abc)+","xabcabcabcx",1,9,7,3),			// 複合文字列の照会
		t2("(ab|cd)+","xabcdabababcdy",1,12,11,2),	// or表現の繰り返し
		t3("(ab|(cd))+","xababy",1,4,3,2,0,0),
		t2("()+","abc",0,0,0,0),					// 0文字幅無限ループを回避できるか
		t2("(a*)*","a",0,1,1,0),					// 無限分割パターンへの対処
		t("(?:a*)*","a",0,1),						// 無限分割パターンへの対処
		t2("(a{1,2}){2}","aa",0,2,1,1),				// ループ回数の上書きに対処
			// meta_charcter-pattern
		t(".+","# ab_C05",0,8),
		t("\\w+","# ab_C05",2,6),
		t("\\W+","# ab_C05",0,2),
		t("\\d+","# ab_C05",6,2),
		t("\\D+","# ab_C05",0,6),
		t("\\s+","abc\r\n  	d",3,5),
		t("\\S+","abc\r\n  	d",0,3),
		t("\\cA\\C-\\x42X","\01\02X",0,3),
		t("\\N","\r\n",0,2),
		t("\\N+","\r\r\n\n",0,4),
		t("\\d\\d","132456789",0,2),
			// meta_anchors-pattern
		t("^$","",0,0),
		x("^abc","aabc"),
		x("abc$","abcc"),
		t("aa\\b cc","aa cc",0,5),
		x("aa\\bcc","aacc"),
		t("aa\\Bcc","aacc",0,4),
		x("aa\\B cc","aa cc"),
#ifdef _MSC_VER
# ifdef UNICODE
		t("漢字\\jあ","漢字あ",0,3),
		x("漢字\\j中","漢字中"),
		t("漢字\\J中","漢字中",0,3),
		x("漢字\\Jあ","漢字あ"),
# else
		t("漢字\\jあ","漢字あ",0,6),
		x("漢字\\j中","漢字中"),
		t("漢字\\J中","漢字中",0,6),
		x("漢字\\Jあ","漢字あ"),
# endif // UNICODE
#endif // _MSC_VER
		x("ab\\j3","ab3"),				// 英単語境界は含めない
		t("ab\\J3","ab3",0,3),			// 英単語境界を含める
		t("ab\\J+3","ab3",0,3),			// 0文字幅無限ループを回避できるか
		t("\\Ax","xy",0,1),
		x("\\Ay","xy"),
		t("x\\z","yx",1,1),
		x("y\\z","yx"),
		t("x\\Z","yx",1,1),
		t("x\\Z","yx\n",1,1),
		t("x\\Z","yx\r",1,1),
		t("x\\Z","yx\r\n",1,1),
		x("y\\Z","yx"),
		t("\\<abc","abc",0,3),
		x("\\<abc","aabc"),
			// charcter_class-pattern
		xc("[abc"),						// コンパイル不可
		t("abc]","xabc]y",1,4),			// 閉じ括弧のみはただの文字列
		t("[^a]","^",0,1),				// ^を文字として扱っていないか
		t("[abc]","xby",1,1),
		x("[^abc]","abc"),
		x("[abc]","xyz"),
		t("[^abc]","axc",1,1),
		x("[]+","abc"),
		t("[^]+","abc",0,3),			// バッファオーバーフローを回避できるか
		t("[.]+","..abc",0,2),			// ブラケット中の.は文字として扱う
		t("[-]","ab-c",2,1),			// -を文字としてみる
		t("[-xyz]","ab-c",2,1),			// -を文字としてみる
		t("[xyz-]","ab-c",2,1),			// -を文字としてみる
			// meta_charcter & charcter_class-pattern
		t("[\\w]","abc",0,1),
		t("[\\w]+","ABC/123",0,3),
		t("[\\b]","a\bc",1,1),			// 文字クラス式内で後退の認識
			// back_reference-pattern
		t2("()\\1","aaa",0,0,0,0),		// 0幅の後方参照
		x("(abc)\\2","abcabc"),			// 上限を超えた後方参照
		x("(abc\\1)","abcabc"),			// 未確定集合への後方参照
		t2("(abc)\\1","abcabc",0,6,0,3),
		t2("(a+)\\1","aaaaaa",0,6,0,3),	// 集合内の繰り返し
			// multibyte_charcter-pattern
#ifdef _MSC_VER
# ifdef UNICODE
		t("aaa", "あaaa",1,3),
		x("aaa", "aあaa"),
		x("aaa", "aaあa"),
		t("aaa", "aaaあ",0,3),
//		x("表","あ"),
//		t("表","表",0,1),
		t("\\x3042","あ",0,1),
		t("切り?替え","切替え",0,3),
# else
		t("aaa", "あaaa",2,3),
		x("aaa", "aあaa"),
		x("aaa", "aaあa"),
		t("aaa", "aaaあ",0,3),
//		x("表","あ"),
//		t("表","表",0,2),
		t("\\x82a0","あ",0,2),
		t("切り?替え","切替え",0,6),
# endif	// UNICODE
#endif // _MSC_VER
			// multiline-pattern
		co(JREG_NORMAL | JREG_BINARYMODE),
		t(".+","ab\r\ncd",0,6),				// 改行を認識しない (\r\n|\r|\n)
		co(JREG_NORMAL | JREG_MULTILINE),
		t(".+","ab\rcd",0,2),				// 改行の認識 (\r\n|\r|\n)
		t(".+","ab\ncd",0,2),
		t(".+","ab\r\ncd",0,2),
		t(".+$","ab\r\ncd",0,2),			// 行末を含めていないか
		t1("^abc","ab\nabc",3,3),			// ^はマッチ幅なし
		t1(".+$\\rc","ab\rcd",0,4),			// $はマッチ幅なし
			// charcter_class_string-pattern
		xc("[[:xxx:]]"),
		xc("[[:mb:]]"),
		x("[[:alnum:]]","@"),
		t("[[:alnum:]]","3",0,1),
		t("[[:kana:]]","ｱ",0,1),
#ifdef _MSC_VER
		x("[[:^alnum:]]","３"),
		t("[[:alnum:]]","ｱ",0,1),			// 含まれるんだって、MSDNより
# ifdef UNICODE
		t("[[:alnum:]]","３",0,1),
# else
		t("[[:alnum:]]","３",0,2),
# endif	// UNICODE
#endif // _MSC_VER
			// ex_union-pattern
		xc("a(?a)a"),					// コンパイル不可
		xc("a(?*a)a"),					// コンパイル不可
		xc("(?<=a*)"),
		xc("(?<=a+)"),
		xc("(?<=a?)"),
		xc("(?<=a*?)"),
		xc("(?<=a*+)"),
		xc("(?<=a{10,15})"),
		xc("(?<=a{10,})"),
		t("(?<=a{3})a","aaaaa",3,1),
		t("(?<=a{3}?)a","aaaaa",3,1),
		t("(?<=a{3}+)a","aaaaa",3,1),
		t("a(?:a)a","aaa",0,3),
		t("aa(?:a)","aaa",0,3),
		t("aa(?=c)","aac",0,2),
		x("aa(?=d)","aac"),
		t("aa(?!x)","aac",0,2),
		x("aa(?!c)","aac"),
		t("(?=a)aaa","aaa",0,3),
		t("(?<=aa)c","aac",2,1),
		x("(?<=dd)c","aac"),
		t("(?<!xx)c","aac",2,1),
		t("(?<!xx)c","c",0,1),
		x("(?<!aa)c","aac"),
		t("abc|(?<=x)y","axyc",2,1),
#ifdef _MSC_VER
# ifdef UNICODE
		t("(?<=あ|x)c","あc",1,1),
# else
		t("(?<=あ|x)c","あc",2,1),
# endif // UNICODE
#endif // _MSC_VER
		t("(?<!\\w|[abc0-9]|\\n)@","@",0,1),	// 固定長のものを認める
		t("(?<!a{3}|ccc)@","@",0,1),			// 長さ固定の繰り返しを認める
		t("(?<=\\N)@","\n@",1,1),				// \N を \n(0x0A) と等価として扱う
		x("a(?<!xx)c","addc"),					// 長さを持たないためマッチしない
		t("a(?<!cc)d","ad",0,2),				// 中置きの先頭方向否定
		xc("(?<=xx|y)c"),						// 選択の長さが異なるためコンパイル不可
		xc("(?<!xx|yy|z)c"),					// 選択の長さが異なるためコンパイル不可
			// other-pattern
		x("(a+)*c","aaaaaaaaaaaaaaab"),			// NFAならバックトラックにより状態数は O(n!) まで膨れ上がる
#ifdef _MSC_VER
# ifdef UNICODE
		t4("http://([0-9a-zA-Z\\.:@]+)(/([^/[:^ascii:]]+))*",
			"文章中のhttp://www.abc.jp@user:8080/def/ht.htmlとかに",4,39,11,20,35,8,36,7),	// 文章中のURLにマッチさせる
# else
		t4("http://([0-9a-zA-Z\\.:@]+)(/([^/[:^ascii:]]+))*",
			"文章中のhttp://www.abc.jp@user:8080/def/ht.htmlとかに",8,39,15,20,39,8,40,7),	// 文章中のURLにマッチさせる
# endif // UNICODE
#endif // _MSC_VER
		t3("(?<=\\s|=)([*~]?(NL|CL|SL|IL|IC|mod[0-9]|[ML][0-9]|[SMACUDR])-)+(?=[*]?\\w+)",	// 窓使いの憂鬱のモデファイヤにマッチさせる
			"key ~C-*M-M4-*RAlt = _1",4,9,10,3,10,2),
			// DFA (non back-reference , always `t' pattern to match)
		co(JREG_NORMAL | JREG_DFA),
		t2("(a)\\1","aa",0,2,0,1),		// 後方参照はNFAのままコンパイル
		t("\\w+?b","aabaab",0,3),		// 最短一致はNFAのままコンパイル
		x("\\w++b","aabaab"),			// 限定一致はNFAのままコンパイル
		t("\\w++@","aabaab@",0,7),
		x("a","b"),
		t("a","a",0,1),
		x("a\\sb","a c"),
		t("a\\sb","a b",0,3),			// メタ文字の利用
		t("a\\n","a\r",0,2),			// \n メタ文字の利用
		t("a\\n","a\n",0,2),
		t("a\\nb","a\rb",0,3),
		t("a\\nb","a\nb",0,3),
		t("a\\nb","a\r\nb",0,4),
		t("abc\\n123","abc\n123",0,7),
		x("a\\n\\nb","a\r\nb"),			// \r\n に\nが2回マッチしない
		t("a\\n+b","a\r\r\n\r\nb",0,7),	// \n メタ文字繰り返し
		x("a\\n(a|b)","a\r\n\rb"),		// \n メタ文字直後の選択
		t("a\\n(a|b)","a\rb",0,3),
		x("a\\bc","ab"),				// アンカーの利用
		t("a\\b@","a@",0,2),
		t("\\<abc","abc",0,3),
		x("\\<abc","aabc"),
		t("\\<","aabc",0,0),			// アンカーを利用した0文字幅表現
		t("a\\b","a",0,1),
		x("(a)b","ac"),					// エンドマーク除去
		t("a*","b",0,0),
		t("a+","a",0,1),
		t("a+","aaaaa",0,5),
		t("(ab)+","ababab",0,6),
		t("a{5}b","aaaaaaaaaaab",6,6),	// 回数指定の繰り返し
		x("(abac)+","acab"),
		t("()","",0,0),					// 空文字に対して無条件一致
		t("(a|)","b",0,0),				// 初期状態をマッチとする
		t("a+a+a+","aaaaa",0,5),		// 状態数あふれによるコンパイル不可回避
		x("a+a+a+a+a+a+a+a+a+a+a+",""),
		t("(a+|b)*c","aabac",0,5),		// 親状態の遅延評価
		t("(?<=x)c","xc",1,1),
		t("a{0}b","abb",1,1),			// 0回の繰り返し
		t("a{1,3}b","aab",0,3),			// 回数限定繰り返し
		t("a{1,3}b","aaaab",1,4),
		x("a{1,3}b","b"),
		t("(a+|b)*c","aabac",0,5),		// 親状態の遅延評価
		x("(a+)*c","aaaaaaaaaaaaaaab"),	// DFAなら状態数が O(n) で済む
		t("......A","xxxxxxxxAx",2,7),	// DFA状態の爆発、完全な決定性を持たないため通じない
		t("c(?=x)","ccx",1,1),			// DFA先読み
		t("(?<=x)x","cxx",2,1),			// DFA戻り読み
		t("<(.(?!>))+","a<bcd>e",1,3),	// DFA先読みのループ
		t("\\b+","a",0,0),				// 0文字幅繰り返しによる無限ループ回避
		t("(?<=a)+b","aaab",3,1),
		t("c(?=x+?)(x)","ccx",1,2),		// 拡張正規表現内での最短一致は許可
		t("c(?=x++)(x)","ccx",1,2),		// 拡張正規表現内での最長一致は許可
			// normal-replace test
		co(JREG_NORMAL),
		sx("a","a","\\",0,1),					// \ で終わる置き換えメタ文字は不可
		sx("a","a","$",0,1),					// $ で終わる置き換えメタ文字は不可
		sx("a","a","\\1",0,1),					// 存在しない後方参照
		sx("a","a","$1",0,1),					// 存在しない後方参照
		s("b","abc","D","D",1,1),
		s("b","abc","D","D",1,1),
		s("b","abc","DEF","DEF",1,1),
		s2("(b+)c","abbbc","A","A",1,4,1,3),
		s("\\n\\n\\n","\r\n\r\n\r\n","\\n\\n","\n\n",0,6),
			// extends-replace test
		s("c","abcde","$`","ab",2,1),					// マッチ以前に相当
		s("cd","abcde","$&","cd",2,2),					// マッチ全体に相当
		s("c","abcde","$'","de",2,1),					// マッチ以後に相当
		s3("(ab)(c)","abcde","$+","c",0,3,0,2,2,1),		// 最後の集合に相当
		s3("(ab)(c)","abcde","$1","ab",0,3,0,2,2,1),	// 1番目の集合に相当
		s3("(ab)(c)","abcde","$2","c",0,3,0,2,2,1),		// 2番目の集合に相当
		s3("(ab)(c)","abcde","\\1","ab",0,3,0,2,2,1),	// 1番目の集合に相当
		s3("(ab)(c)","abcde","\\2","c",0,3,0,2,2,1),	// 2番目の集合に相当
	};
	jreg        *reg;
	jreg_opt     opt;
	PATTERN_STR  Pattern;
	u_int matcnt,optcnt;
	u_int i;
	bool  ret;
	
#ifdef _MSC_VER
	SetUnhandledExceptionFilter(err_handler);
#endif
	
	opt = JREG_NORMAL;
	_tsetlocale(LC_ALL, _T("Japanese"));
	for(i = matcnt = optcnt = 0; i < lengthof(RegTest); i++)
	{
		t_code str[1024];
		u_int  j,k;

		if(!RegTest[i].regstr)
		{
			opt = (jreg_opt)RegTest[i].matnum;
			optcnt++;
			continue;
		}
		for(j = k = 0; RegTest[i].str[j]; j++, k++)
		{
			switch(_tccode(RegTest[i].str + j))
			{
			case _T(' '):   str[k]   = _T('`'); break;
			case _T('\t'):  str[k++] = _T('^'); str[k] = _T('T'); break;
			case _T('\v'):  str[k++] = _T('^'); str[k] = _T('V'); break;
			case _T('\n'):  str[k++] = _T('^'); str[k] = _T('N'); break;
			case _T('\r'):  str[k++] = _T('^'); str[k] = _T('R'); break;
			case _T('\f'):  str[k++] = _T('^'); str[k] = _T('F'); break;
			case _T('\b'):  str[k++] = _T('^'); str[k] = _T('B'); break;
			case _T('\a'):  str[k++] = _T('^'); str[k] = _T('A'); break;
			case _T('\033'):str[k++] = _T('^'); str[k] = _T('E'); break;
			default:        str[k]   = RegTest[i].str[j]; break;
			}
		}
		str[k] = _T('\0');

		reg = jreg_comp(RegTest[i].regstr, opt);
		if(!reg && !(RegTest[i].comp))
		{
			_tprintf(_T("[%03d] %c/%s/\n"),
				i - optcnt + 1,
				RegTest[i].repstr ? _T('s') : _T('m'),
				RegTest[i].regstr);
			matcnt++;
			continue;
		}
		else if(!reg || !(RegTest[i].comp))
		{
			_tprintf(_T("`%03d' %c/%s/ , assert RegexCompile\n"),
				i - optcnt + 1,
				RegTest[i].repstr ? _T('s') : _T('m'),
				RegTest[i].regstr);
			return(1);
		}
		Pattern.PATTERN_TCODE = RegTest[i].str;
		ret = jreg_exec(reg, Pattern, 0, _tcslen((TCHAR *)RegTest[i].str), opt);
		if((ret && RegTest[i].matnum > 0) || (!ret && RegTest[i].matnum == 0))
		{
			if(ret && RegTest[i].matnum > 0)
			{
				u_int j;
				if(RegTest[i].matnum != reg->backrefcnt)
				{
					_tprintf(_T("<%03d> %c/%s/ =~ `%s' , is match count missing (%d)\n"),
						i - optcnt + 1,
						RegTest[i].repstr ? _T('s') : _T('m'),
						RegTest[i].regstr,
						str,
						reg->backrefcnt);
					continue;
				}
				for(j = 0; j < reg->backrefcnt; j++)
				{
					if(!(RegTest[i].pat[j].start == reg->backref[j].start &&
						((reg->backref[j].match > 0 && RegTest[i].pat[j].match == reg->backref[j].match - 1) ||
							(RegTest[i].pat[j].match == 0 && reg->backref[j].match == 0))))
					{
						_tprintf(_T("<%03d> %c/%s/ =~ `%s' , is num of %d backref missing (%d , %d)\n"),
							i - optcnt + 1,
							RegTest[i].repstr ? _T('s') : _T('m'),
							RegTest[i].regstr,
							str, j,
							reg->backref[j].start,
							reg->backref[j].match == 0 ? 0 : reg->backref[j].match - 1);
						goto __END_LOOP__;
					}
				}
			}
			if(RegTest[i].repstr)
			{
				jreg_rep rep;
				
				rep.maxsize = 0;
				rep.repstr  = RegTest[i].repstr;
				rep.repbuf  = null;
				if(!jreg_replace(reg, &rep, Pattern, 0, _tcslen((TCHAR *)RegTest[i].str), opt | JREG_USELASTRET)) {goto __ERR_REPLACE__;}
				
				if(!RegTest[i].repret) {goto __ERR_REPLACE__;}
				rep.maxsize = rep.bufsize;
				if(!(rep.repbuf = malloc(sizeof(t_code) * (rep.bufsize + 1)))) {goto __ERR_REPLACE__;}
				jreg_replace(reg, &rep, Pattern, 0, _tcslen((TCHAR *)RegTest[i].str), opt | JREG_USELASTRET);
				
				if(_tcslen(RegTest[i].repret) == rep.bufsize &&
					memcmp(RegTest[i].repret, rep.repbuf, sizeof(t_code) * rep.bufsize) == 0)
				{
					_tprintf(_T("[%03d] s/%s/%s/ =~ `%s' => `%s'\n"),
						i - optcnt + 1,
						RegTest[i].regstr,
						RegTest[i].repstr,
						str,
						RegTest[i].repret);
				}
				else
				{
__ERR_REPLACE__:
					if(RegTest[i].repret)
					{
						if(rep.repbuf) {rep.repbuf[rep.bufsize] = _T('\0');}
						_tprintf(_T("<%03d> s/%s/%s/ =~ `%s' => `%s' , is replace unmatch (real `%s')\n"),
							i - optcnt + 1,
							RegTest[i].regstr,
							RegTest[i].repstr,
							str,
							RegTest[i].repret,
							rep.repbuf ? rep.repbuf : _T("nothing"));
					}
					else
					{
						_tprintf(_T("[%03d] s/%s/%s/ =~ `%s' => x\n"),
							i - optcnt + 1,
							RegTest[i].regstr,
							RegTest[i].repstr,
							str);
						matcnt++;
					}
					free(rep.repbuf);
					goto __END_LOOP__;
				}
				free(rep.repbuf);
			}
			else
			{
				_tprintf(_T("[%03d] m/%s/ %s `%s'\n"),
					i - optcnt + 1,
					RegTest[i].regstr,
					RegTest[i].matnum > 0 ? _T("=~") : _T("!~"),
					str);
			}
			matcnt++;
		}
		else
		{
			_tprintf(_T("<%03d> %c/%s/ %s `%s' , is false\n"),
				i - optcnt + 1,
				RegTest[i].repstr ? _T('s') : _T('m'),
				RegTest[i].regstr,
				RegTest[i].matnum > 0 ? _T("=~") : _T("!~"),
				str);
		}
__END_LOOP__:
		jreg_free(reg);
	}
	_tprintf(_T("  true = %d , false = %d\n"),
		matcnt, lengthof(RegTest) - matcnt - optcnt);
	return(0);
}

#ifdef _MSC_VER
LONG CALLBACK err_handler(EXCEPTION_POINTERS *e)
{
	byte             psymbol[sizeof(IMAGEHLP_SYMBOL) + 64];
	IMAGEHLP_SYMBOL *symbol;
	STACKFRAME       frame;
	FILE            *fp;
	HANDLE           pid = GetCurrentProcess();
	
	if(!(fp = _tfopen(_T("regvc.dump"), _T("w")))) {return(EXCEPTION_EXECUTE_HANDLER);}
	
	symbol = (IMAGEHLP_SYMBOL*)psymbol;
	symbol->SizeOfStruct  = sizeof(psymbol);
	symbol->MaxNameLength = sizeof(psymbol) - sizeof(IMAGEHLP_SYMBOL);
	
	memset(&frame, 0, sizeof(frame));
	frame.AddrPC.Offset    = e->ContextRecord->Eip;
	frame.AddrStack.Offset = e->ContextRecord->Esp;
	frame.AddrFrame.Offset = e->ContextRecord->Ebp;
	frame.AddrPC.Mode      = AddrModeFlat;
	frame.AddrStack.Mode   = AddrModeFlat;
	frame.AddrFrame.Mode   = AddrModeFlat;
	
	SymInitialize(pid, NULL, TRUE);
	while(1)
	{
		BOOL  ret;
		DWORD disp;
		
		ret = StackWalk(
				IMAGE_FILE_MACHINE_I386, //? i386固定でええんか?
				pid,
				GetCurrentThread(),
				&frame, NULL, NULL,
				SymFunctionTableAccess,
				SymGetModuleBase,
				NULL);
		
		if(!ret || frame.AddrFrame.Offset == 0) {break;}
		
		ret = SymGetSymFromAddr(pid, frame.AddrPC.Offset, &disp, symbol);
		
		if(ret) {_ftprintf(fp, _T("0x%08x %s() + 0x%x\n"), (u_int)frame.AddrPC.Offset, symbol->Name, (u_int)disp);}
		else    {_ftprintf(fp, _T("0x%08x ---\n"),         (u_int)frame.AddrPC.Offset);}
	}
	SymCleanup(pid);
	fclose(fp);
	
	_tprintf(_T("Segmentation fault."));
	
	return(EXCEPTION_EXECUTE_HANDLER);
}
#endif // _MSC_VER
