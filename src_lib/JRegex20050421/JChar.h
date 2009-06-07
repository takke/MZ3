#ifndef __INC_JCHAR__
#define __INC_JCHAR__
#include "jectlib.h"
#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	//- •¶Žšƒ^ƒCƒv
typedef enum _TC_TYPE
{
	tc_null       = 0x00000001, // \0
	tc_tab        = 0x00000002, // \t
	tc_vtab       = 0x00000004, // \v
	tc_cr         = 0x00000008, // \r
	tc_lf         = 0x00000010, // \n
	tc_page       = 0x00000020, // \f
	tc_bell       = 0x00000040, // \a
	tc_back       = 0x00000080, // \b
	tc_esc        = 0x00000100, // 0x1b (gcc \e)
	tc_space      = 0x00000200, // ` '

	tc_lcalpha    = 0x00000400, // [a-z]
	tc_ucalpha    = 0x00000800, // [A-Z]
	tc_num        = 0x00001000, // [0-9]
	tc_under      = 0x00002000, // _
	tc_kana       = 0x00004000, // [¡-ß]

	tc_mbspace    = 0x00008000, // `@'
	tc_mblcalpha  = 0x00010000, // [‚-‚š]
	tc_mbucalpha  = 0x00020000, // [‚`-‚y]
	tc_mbnum      = 0x00040000, // [‚O-‚X]
	tc_mbkana     = 0x00080000, // [ƒ@-ƒ–]
	tc_mbhira     = 0x00100000, // [‚Ÿ-‚ñ]

	tc_mbunder    = 0x00200000, // Q
	tc_mbgreece   = 0x00400000, // [ƒŸ-ƒÖ]
	tc_mblcrussia = 0x00800000, // [„p-„‘]
	tc_mbucrussia = 0x01000000, // [„@-„`]

	tc_other      = 0x10000000,
	tc_mbother    = 0x20000000,

	tc_error      = 0x80000000,

	tc_crlf       = tc_cr | tc_lf,
	tc_blank      = tc_space | tc_tab,
	tc_s          = tc_blank | tc_vtab | tc_crlf | tc_page,

	tc_alpha      = tc_lcalpha | tc_ucalpha,
	tc_d          = tc_num,
	tc_w          = tc_num | tc_alpha | tc_under,

	tc_mbalpha    = tc_mblcalpha | tc_mbucalpha,
	tc_mbrussia   = tc_mblcrussia | tc_mbucrussia,
	tc_mbw        = tc_mbnum | tc_mbalpha | tc_mbunder,
	tc_mb         = tc_mbspace | tc_mbw | tc_mbkana | tc_mbhira | tc_mbgreece | tc_mbrussia | tc_mbother,
	tc_k          = tc_mb,
}tc_type;


	//- •¶ŽšƒR[ƒh
typedef byte s_code;
typedef word w_code;
#ifdef UNICODE
typedef word t_code;
#else
typedef byte t_code;
#endif // UNICODE

	//- •¶Žš•ÏŠ·ƒ}ƒNƒ
#define _tclc(c) \
	(jregex__range(_T('A'), _T('Z'), c) ? c + _T('a') - _T('A') : c)
#define _tcuc(c) \
	(jregex__range(_T('a'), _T('z'), c) ? c - _T('a') + _T('A') : c)

	//- •¶Žš—ñ‘€ìƒ}ƒNƒ
#ifdef UNICODE
#define _iskanji(n) (jregex__range(0x0100, 0xFF5E, n))
#define _tccnt(p)   1
#define _tccntb(n)  1
#define _tccode(p)  *(p)
#define _tcprev(p,n)   (n > 0 ? 1 : 0)
#define _tcset(p,n)    do{*(p) = n;}while(0)
#define _tc2wc(p)      ((w_code*)p)
#define _tc2wc_free(p) do{}while(0)
#define _tc2cc(p)      _tc2cc_dup(p)
#define _tc2cc_free(p) free(p)
#define _tc2wc_dup(p)  _tcsdup(p)
#define _wc2tc_dup(p)  _tcsdup(p)
#else
#define _iskanji(n)     (_iskanji1(((n) >> 8) & 0xFF) && _iskanji2((n) & 0xFF))
#define _iskanji1(n)    (jregex__range(0x81, 0x9F, n) || jregex__range(0xE0, 0xFC, n))
#define _iskanji2(n)    ((n) != 0x7F && jregex__range(0x40, 0xFC, n))
#define _mkkanji(n1,n2) ((w_code)((n1) << 8 | (n2)))
#define _tcset(p,n) \
	do{ \
		if(_iskanji(n)){ \
			*(p) = (s_code)(((n) >> 8) & 0xFF); *(p + 1) = (s_code)((n) & 0xFF);} \
		else{ \
			*(p) = (s_code)((n) & 0xFF);} \
	}while(0)
#define _tc2wc(p)      _tc2wc_dup(p)
#define _tc2wc_free(p) free(p)
#define _tc2cc(p)      ((s_code *)p)
#define _tc2cc_free(p) do{}while(0)
#define _tc2cc_dup(p)  _tcsdup(p)
#define _cc2tc_dup(p)  _tcsdup(p)
#endif // UNICODE

#ifndef __CYGWIN__
# undef  _sntprintf
# define _sntprintf _w32_sntprintf
#endif // __CYGWIN__


t_code* _tstrndup(t_code *str, u_int maxs);
#ifdef UNICODE
s_code* _tc2cc_dup(t_code *str);
t_code* _cc2tc_dup(s_code *str);
#else
w_code* _tc2wc_dup(t_code *str);
t_code* _wc2tc_dup(w_code *str);
#endif // UNICODE
tc_type _tctype(w_code ch);
w_code  _tcnext(t_code **str);
bool    _tcsetc(t_code **str, w_code ch);
#ifndef UNICODE
u_int  _tccnt (t_code *str);
u_int  _tccntb(w_code  ch);
w_code _tccode(t_code *str);
u_int  _tcprev(t_code *str, u_int cnt);
#endif // UNICODE

size_t _tstrlcpy(t_code *dst, t_code *src, size_t cnt);
size_t _tstrlcat(t_code *dst, t_code *src, size_t cnt);
#ifndef __CYGWIN__
s_int _w32_sntprintf(t_code *buf, size_t size, t_code *format, ...);
#endif // __CYGWIN__


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INC_JCHAR__
