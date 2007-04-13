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
		t("","abc",0,0),				// �S�Ẵp�^�[���Ƀ}�b�`�����\��
		t("abc","abc",0,3),
		x("abc","ab"),					// �����񒷂̕s��
		x("abc","abd"),
		t("abc","aabc",1,3),			// �擪�ȊO�ւ̃}�b�`
		t("abc","abcd",0,3),
		t("abc","zabcd",1,3),
		t("abc", "bcababcab",4,3),
		t("abc", "BCABABCAB",4,3),
		t("ABC", "bcababcab",4,3),
			// or-pattern
		t("abc|","xyz",0,0),			// ��J�ڂ�����or�\��
		t("abc|def","zabcx",1,3),		// ���[�g�m�[�h�ɑ΂���or�\��
		t("abc|def","zdefx",1,3),
		t("abc|def","abc|def",0,3),		// |�𕶎��Ƃ��Ĉ����Ă��Ȃ���
			// closure-pattern
		xc("*"),						// �R���p�C���s��
		xc("+"),						// �R���p�C���s��
		xc("a**"),						// �R���p�C���s��
		xc("a{"),						// �R���p�C���s��
		xc("a{}"),						// �R���p�C���s��
		xc("a{,}"),						// �R���p�C���s��
		xc("a{,1}"),					// �R���p�C���s��
		xc("ab|?cd"),					// �R���p�C���s��
		t("a}","a}",0,2),				// �����ʑ����ꍇ�A�����Ƃ��Ĉ���
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
		xc("*?"),									// �R���p�C���s��
		xc("+?"),									// �R���p�C���s��
		xc("a*??"),									// �R���p�C���s��
		xc("a+??"),									// �R���p�C���s��
		t("a??","a",0,0),							// non_greeady�D��
		t("a*?","aaaa",0,0),
		t("a+?","aaaa",0,1),						// �Œ���̃}�b�`��ۏ؂���non_greeady
		t("a*?b","aaaab",0,5),
		t2("(a{3,}?){2,}?","aaaaaaa",0,6,3,3),		// ���d��non_greeady
		t2("(a{3,}?b){2,}?","aaabaaaba",0,8,4,4),
			// more_greedy_closure-pattern
		xc("++"),						// �R���p�C���s��
		t("a++","aaa",0,3),
		t("a++b","aaab",0,4),
		x("a++ab","aaab"),				// �S�Ẵp�^�[��������オ�����Ȃ�more_greedy
		t("a{3,5}+ab","aaaaaaab",1,7),
		x("a{3,5}+ab","aaaaab"),
			// union-pattern
		xc("(abc"),						// �����ʂȂ��A�R���p�C���s��
		t("abc)","abc)",0,4),			// �����ʑ����ꍇ�A�����Ƃ��Ĉ���
		t2("(a)bc","xabcy",1,3,1,1),
		t2("a(b)c","xabcy",1,3,2,1),
		t2("ab(c)","xabcy",1,3,3,1),
			// union & or-pattern
		t2("(abc|)","xyz",0,0,0,0),			// ��J�ڂ�����or�\��
		t2("(abc|def)","zabcx",1,3,1,3),
		t2("(abc|def)","zdefx",1,3,1,3),
		t2("(abc|def)","abc|def",0,3,0,3),	// |�𕶎��Ƃ��Ĉ����Ă��Ȃ���
			// closure & union-pattern
		t2("(a)+a","aaaa",0,4,2,1),					// ����Q�Ƃ̏㏑��
		t2("(a+)a","aaaa",0,4,0,3),					// �J��Ԃ��̃o�b�N�g���b�N
		t2("(abc)+","xabcabcabcx",1,9,7,3),			// ����������̏Ɖ�
		t2("(ab|cd)+","xabcdabababcdy",1,12,11,2),	// or�\���̌J��Ԃ�
		t3("(ab|(cd))+","xababy",1,4,3,2,0,0),
		t2("()+","abc",0,0,0,0),					// 0�������������[�v������ł��邩
		t2("(a*)*","a",0,1,1,0),					// ���������p�^�[���ւ̑Ώ�
		t("(?:a*)*","a",0,1),						// ���������p�^�[���ւ̑Ώ�
		t2("(a{1,2}){2}","aa",0,2,1,1),				// ���[�v�񐔂̏㏑���ɑΏ�
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
		t("����\\j��","������",0,3),
		x("����\\j��","������"),
		t("����\\J��","������",0,3),
		x("����\\J��","������"),
# else
		t("����\\j��","������",0,6),
		x("����\\j��","������"),
		t("����\\J��","������",0,6),
		x("����\\J��","������"),
# endif // UNICODE
#endif // _MSC_VER
		x("ab\\j3","ab3"),				// �p�P�ꋫ�E�͊܂߂Ȃ�
		t("ab\\J3","ab3",0,3),			// �p�P�ꋫ�E���܂߂�
		t("ab\\J+3","ab3",0,3),			// 0�������������[�v������ł��邩
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
		xc("[abc"),						// �R���p�C���s��
		t("abc]","xabc]y",1,4),			// �����ʂ݂̂͂����̕�����
		t("[^a]","^",0,1),				// ^�𕶎��Ƃ��Ĉ����Ă��Ȃ���
		t("[abc]","xby",1,1),
		x("[^abc]","abc"),
		x("[abc]","xyz"),
		t("[^abc]","axc",1,1),
		x("[]+","abc"),
		t("[^]+","abc",0,3),			// �o�b�t�@�I�[�o�[�t���[������ł��邩
		t("[.]+","..abc",0,2),			// �u���P�b�g����.�͕����Ƃ��Ĉ���
		t("[-]","ab-c",2,1),			// -�𕶎��Ƃ��Ă݂�
		t("[-xyz]","ab-c",2,1),			// -�𕶎��Ƃ��Ă݂�
		t("[xyz-]","ab-c",2,1),			// -�𕶎��Ƃ��Ă݂�
			// meta_charcter & charcter_class-pattern
		t("[\\w]","abc",0,1),
		t("[\\w]+","ABC/123",0,3),
		t("[\\b]","a\bc",1,1),			// �����N���X�����Ō�ނ̔F��
			// back_reference-pattern
		t2("()\\1","aaa",0,0,0,0),		// 0���̌���Q��
		x("(abc)\\2","abcabc"),			// ����𒴂�������Q��
		x("(abc\\1)","abcabc"),			// ���m��W���ւ̌���Q��
		t2("(abc)\\1","abcabc",0,6,0,3),
		t2("(a+)\\1","aaaaaa",0,6,0,3),	// �W�����̌J��Ԃ�
			// multibyte_charcter-pattern
#ifdef _MSC_VER
# ifdef UNICODE
		t("aaa", "��aaa",1,3),
		x("aaa", "a��aa"),
		x("aaa", "aa��a"),
		t("aaa", "aaa��",0,3),
//		x("�\","��"),
//		t("�\","�\",0,1),
		t("\\x3042","��",0,1),
		t("�؂�?�ւ�","�ؑւ�",0,3),
# else
		t("aaa", "��aaa",2,3),
		x("aaa", "a��aa"),
		x("aaa", "aa��a"),
		t("aaa", "aaa��",0,3),
//		x("�\","��"),
//		t("�\","�\",0,2),
		t("\\x82a0","��",0,2),
		t("�؂�?�ւ�","�ؑւ�",0,6),
# endif	// UNICODE
#endif // _MSC_VER
			// multiline-pattern
		co(JREG_NORMAL | JREG_BINARYMODE),
		t(".+","ab\r\ncd",0,6),				// ���s��F�����Ȃ� (\r\n|\r|\n)
		co(JREG_NORMAL | JREG_MULTILINE),
		t(".+","ab\rcd",0,2),				// ���s�̔F�� (\r\n|\r|\n)
		t(".+","ab\ncd",0,2),
		t(".+","ab\r\ncd",0,2),
		t(".+$","ab\r\ncd",0,2),			// �s�����܂߂Ă��Ȃ���
		t1("^abc","ab\nabc",3,3),			// ^�̓}�b�`���Ȃ�
		t1(".+$\\rc","ab\rcd",0,4),			// $�̓}�b�`���Ȃ�
			// charcter_class_string-pattern
		xc("[[:xxx:]]"),
		xc("[[:mb:]]"),
		x("[[:alnum:]]","@"),
		t("[[:alnum:]]","3",0,1),
		t("[[:kana:]]","�",0,1),
#ifdef _MSC_VER
		x("[[:^alnum:]]","�R"),
		t("[[:alnum:]]","�",0,1),			// �܂܂��񂾂��āAMSDN���
# ifdef UNICODE
		t("[[:alnum:]]","�R",0,1),
# else
		t("[[:alnum:]]","�R",0,2),
# endif	// UNICODE
#endif // _MSC_VER
			// ex_union-pattern
		xc("a(?a)a"),					// �R���p�C���s��
		xc("a(?*a)a"),					// �R���p�C���s��
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
		t("(?<=��|x)c","��c",1,1),
# else
		t("(?<=��|x)c","��c",2,1),
# endif // UNICODE
#endif // _MSC_VER
		t("(?<!\\w|[abc0-9]|\\n)@","@",0,1),	// �Œ蒷�̂��̂�F�߂�
		t("(?<!a{3}|ccc)@","@",0,1),			// �����Œ�̌J��Ԃ���F�߂�
		t("(?<=\\N)@","\n@",1,1),				// \N �� \n(0x0A) �Ɠ����Ƃ��Ĉ���
		x("a(?<!xx)c","addc"),					// �����������Ȃ����߃}�b�`���Ȃ�
		t("a(?<!cc)d","ad",0,2),				// ���u���̐擪�����ے�
		xc("(?<=xx|y)c"),						// �I���̒������قȂ邽�߃R���p�C���s��
		xc("(?<!xx|yy|z)c"),					// �I���̒������قȂ邽�߃R���p�C���s��
			// other-pattern
		x("(a+)*c","aaaaaaaaaaaaaaab"),			// NFA�Ȃ�o�b�N�g���b�N�ɂ���Ԑ��� O(n!) �܂Ŗc��オ��
#ifdef _MSC_VER
# ifdef UNICODE
		t4("http://([0-9a-zA-Z\\.:@]+)(/([^/[:^ascii:]]+))*",
			"���͒���http://www.abc.jp@user:8080/def/ht.html�Ƃ���",4,39,11,20,35,8,36,7),	// ���͒���URL�Ƀ}�b�`������
# else
		t4("http://([0-9a-zA-Z\\.:@]+)(/([^/[:^ascii:]]+))*",
			"���͒���http://www.abc.jp@user:8080/def/ht.html�Ƃ���",8,39,15,20,39,8,40,7),	// ���͒���URL�Ƀ}�b�`������
# endif // UNICODE
#endif // _MSC_VER
		t3("(?<=\\s|=)([*~]?(NL|CL|SL|IL|IC|mod[0-9]|[ML][0-9]|[SMACUDR])-)+(?=[*]?\\w+)",	// ���g���̗J�T�̃��f�t�@�C���Ƀ}�b�`������
			"key ~C-*M-M4-*RAlt = _1",4,9,10,3,10,2),
			// DFA (non back-reference , always `t' pattern to match)
		co(JREG_NORMAL | JREG_DFA),
		t2("(a)\\1","aa",0,2,0,1),		// ����Q�Ƃ�NFA�̂܂܃R���p�C��
		t("\\w+?b","aabaab",0,3),		// �ŒZ��v��NFA�̂܂܃R���p�C��
		x("\\w++b","aabaab"),			// �����v��NFA�̂܂܃R���p�C��
		t("\\w++@","aabaab@",0,7),
		x("a","b"),
		t("a","a",0,1),
		x("a\\sb","a c"),
		t("a\\sb","a b",0,3),			// ���^�����̗��p
		t("a\\n","a\r",0,2),			// \n ���^�����̗��p
		t("a\\n","a\n",0,2),
		t("a\\nb","a\rb",0,3),
		t("a\\nb","a\nb",0,3),
		t("a\\nb","a\r\nb",0,4),
		t("abc\\n123","abc\n123",0,7),
		x("a\\n\\nb","a\r\nb"),			// \r\n ��\n��2��}�b�`���Ȃ�
		t("a\\n+b","a\r\r\n\r\nb",0,7),	// \n ���^�����J��Ԃ�
		x("a\\n(a|b)","a\r\n\rb"),		// \n ���^��������̑I��
		t("a\\n(a|b)","a\rb",0,3),
		x("a\\bc","ab"),				// �A���J�[�̗��p
		t("a\\b@","a@",0,2),
		t("\\<abc","abc",0,3),
		x("\\<abc","aabc"),
		t("\\<","aabc",0,0),			// �A���J�[�𗘗p����0�������\��
		t("a\\b","a",0,1),
		x("(a)b","ac"),					// �G���h�}�[�N����
		t("a*","b",0,0),
		t("a+","a",0,1),
		t("a+","aaaaa",0,5),
		t("(ab)+","ababab",0,6),
		t("a{5}b","aaaaaaaaaaab",6,6),	// �񐔎w��̌J��Ԃ�
		x("(abac)+","acab"),
		t("()","",0,0),					// �󕶎��ɑ΂��Ė�������v
		t("(a|)","b",0,0),				// ������Ԃ��}�b�`�Ƃ���
		t("a+a+a+","aaaaa",0,5),		// ��Ԑ����ӂ�ɂ��R���p�C���s���
		x("a+a+a+a+a+a+a+a+a+a+a+",""),
		t("(a+|b)*c","aabac",0,5),		// �e��Ԃ̒x���]��
		t("(?<=x)c","xc",1,1),
		t("a{0}b","abb",1,1),			// 0��̌J��Ԃ�
		t("a{1,3}b","aab",0,3),			// �񐔌���J��Ԃ�
		t("a{1,3}b","aaaab",1,4),
		x("a{1,3}b","b"),
		t("(a+|b)*c","aabac",0,5),		// �e��Ԃ̒x���]��
		x("(a+)*c","aaaaaaaaaaaaaaab"),	// DFA�Ȃ��Ԑ��� O(n) �ōς�
		t("......A","xxxxxxxxAx",2,7),	// DFA��Ԃ̔����A���S�Ȍ��萫�������Ȃ����ߒʂ��Ȃ�
		t("c(?=x)","ccx",1,1),			// DFA��ǂ�
		t("(?<=x)x","cxx",2,1),			// DFA�߂�ǂ�
		t("<(.(?!>))+","a<bcd>e",1,3),	// DFA��ǂ݂̃��[�v
		t("\\b+","a",0,0),				// 0�������J��Ԃ��ɂ�閳�����[�v���
		t("(?<=a)+b","aaab",3,1),
		t("c(?=x+?)(x)","ccx",1,2),		// �g�����K�\�����ł̍ŒZ��v�͋���
		t("c(?=x++)(x)","ccx",1,2),		// �g�����K�\�����ł̍Œ���v�͋���
			// normal-replace test
		co(JREG_NORMAL),
		sx("a","a","\\",0,1),					// \ �ŏI���u���������^�����͕s��
		sx("a","a","$",0,1),					// $ �ŏI���u���������^�����͕s��
		sx("a","a","\\1",0,1),					// ���݂��Ȃ�����Q��
		sx("a","a","$1",0,1),					// ���݂��Ȃ�����Q��
		s("b","abc","D","D",1,1),
		s("b","abc","D","D",1,1),
		s("b","abc","DEF","DEF",1,1),
		s2("(b+)c","abbbc","A","A",1,4,1,3),
		s("\\n\\n\\n","\r\n\r\n\r\n","\\n\\n","\n\n",0,6),
			// extends-replace test
		s("c","abcde","$`","ab",2,1),					// �}�b�`�ȑO�ɑ���
		s("cd","abcde","$&","cd",2,2),					// �}�b�`�S�̂ɑ���
		s("c","abcde","$'","de",2,1),					// �}�b�`�Ȍ�ɑ���
		s3("(ab)(c)","abcde","$+","c",0,3,0,2,2,1),		// �Ō�̏W���ɑ���
		s3("(ab)(c)","abcde","$1","ab",0,3,0,2,2,1),	// 1�Ԗڂ̏W���ɑ���
		s3("(ab)(c)","abcde","$2","c",0,3,0,2,2,1),		// 2�Ԗڂ̏W���ɑ���
		s3("(ab)(c)","abcde","\\1","ab",0,3,0,2,2,1),	// 1�Ԗڂ̏W���ɑ���
		s3("(ab)(c)","abcde","\\2","c",0,3,0,2,2,1),	// 2�Ԗڂ̏W���ɑ���
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
				IMAGE_FILE_MACHINE_I386, //? i386�Œ�ł�����?
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
