#include "jectlib.h"
#include "JChar.h"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>


t_code* _tstrndup(t_code *str, u_int maxs)
{
	u_int   size;
	t_code *mstr;
	
	if(!str) {return(null);}
	size = _tcslen(str);
	size = (maxs < size ? maxs : size) + 1;
	
	if(!(mstr = (t_code *)malloc(sizeof(t_code) * size))) {return(null);}
	
	_tstrlcpy(mstr, str, size);
	return(mstr);
}

#ifdef UNICODE
# define _wc2cc_type s_code
s_code* _tc2cc_dup(t_code *str)
#else
# define _wc2cc_type t_code
t_code* _wc2tc_dup(w_code *str)
#endif // UNICODE
{
	if(str)
	{
		int          len;
		_wc2cc_type *cstr = (_wc2cc_type *)malloc(len = WideCharToMultiByte(CP_ACP, 0, str, -1, null, 0, null, null));
		if(cstr)
		{
			if(WideCharToMultiByte(CP_ACP, 0, str, -1, cstr, len, null, null) > 0) {return(cstr);}
			free(cstr);
		}
	}
	return(null);
}
#undef _wc2cc_type

#ifdef UNICODE
# define _cc2wc_type t_code
t_code* _cc2tc_dup(s_code *str)
#else
# define _cc2wc_type w_code
w_code* _tc2wc_dup(t_code *str)
#endif // UNICODE
{
	if(str)
	{
		int          len;
		_cc2wc_type *wstr = (_cc2wc_type *)malloc(len = MultiByteToWideChar(CP_ACP, 0, (CHAR *)str, -1, null, 0));
		if(wstr)
		{
			if(MultiByteToWideChar(CP_ACP, 0, (CHAR *)str, -1, wstr, len) > 0) {return(wstr);}
			free(wstr);
		}
	}
	return(null);
}
#undef _cc2wc_type

tc_type _tctype(w_code ch)
{
	switch(ch)
	{
	case _T('\0'):   return(tc_null);  break;
	case _T('\t'):   return(tc_tab);   break;
	case _T('\v'):   return(tc_vtab);  break;
	case _T('\r'):   return(tc_cr);    break;
	case _T('\n'):   return(tc_lf);    break;
	case _T('\f'):   return(tc_page);  break;
	case _T('\a'):   return(tc_bell);  break;
	case _T('\b'):   return(tc_back);  break;
	case _T('\x1b'): return(tc_esc);   break;
	case _T(' '):    return(tc_space); break;

	case _T('_'):    return(tc_under); break;

	case _T('Å@'):   return(tc_mbspace); break;
	case _T('ÅQ'):   return(tc_mbunder); break;
	default:
		if(_iskanji(ch))
		{
			if(__range((w_code)_T('ÇÅ'), (w_code)_T('Çö'), ch)) {return(tc_mblcalpha);}
			if(__range((w_code)_T('Ç`'), (w_code)_T('Çy'), ch)) {return(tc_mbucalpha);}
			if(__range((w_code)_T('ÇO'), (w_code)_T('ÇX'), ch)) {return(tc_mbnum);}
			if(__range((w_code)_T('É@'), (w_code)_T('Éñ'), ch)) {return(tc_mbkana);}
			if(__range((w_code)_T('Çü'), (w_code)_T('ÇÒ'), ch)) {return(tc_mbhira);}

			if(__range((w_code)_T('Éü'), (w_code)_T('É÷'), ch)) {return(tc_mbgreece);}
			if(__range((w_code)_T('Ñp'), (w_code)_T('Ñë'), ch)) {return(tc_mblcrussia);}
			if(__range((w_code)_T('Ñ@'), (w_code)_T('Ñ`'), ch)) {return(tc_mbucrussia);}
			return(tc_mbother);
		}
		else
		{
			if(__range((t_code)_T('a'), (t_code)_T('z'), ch)) {return(tc_lcalpha);}
			if(__range((t_code)_T('A'), (t_code)_T('Z'), ch)) {return(tc_ucalpha);}
			if(__range((t_code)_T('0'), (t_code)_T('9'), ch)) {return(tc_num);}

			if(__range((t_code)_T('°'), (t_code)_T('ﬂ'), ch)) {return(tc_kana);}
			return(tc_other);
		}
		break;
	}
	return(tc_error);
}

w_code _tcnext(t_code **str)
{
	w_code ch;
	
	if(!str || !*str) {return(_T('\0'));}
	
	ch   =  _tccode(*str);
	*str += _tccntb(ch);
	return(ch);
}

bool _tcsetc(t_code **str, w_code ch)
{
	if(!str || !*str) {return(false);}

	_tcset(*str, ch);
	*str += _tccntb(ch);
	return(true);
}

#ifndef UNICODE
u_int _tccnt(t_code *str)
{
	if(!_iskanji1(*str)) {return(1);}
	if(_iskanji2(*(str + 1))) {return(2);}
	return(1);
}

u_int _tccntb(w_code ch)
{
	if(_iskanji(ch)) {return(2);}
	return(1);
}

w_code _tccode(t_code *str)
{
	w_code ch = (w_code)*str;
	if(!_iskanji1(ch)) {return(ch);}
	else
	{
		w_code ch2 = (w_code)*(str + 1);
		if(_iskanji2(ch2)) {return(_mkkanji(ch, ch2));}
	}
	return(ch);
}

u_int _tcprev(t_code *str, u_int cnt)
{
	if(cnt == 0) {return(0);}
	else
	{
		w_code ch = (w_code)*(str - 1);
		if(ch <= 0x3F || 0xFD <= ch || ch == 0x7F || cnt == 1) {return(1);}
		ch = (w_code)*(str - 2);
		if(__range(0x81, 0x9F, ch) || __range(0xE0, 0xFC, ch)) {return(2);}
	}
	return(1);
}
#endif // UNICODE

size_t _tstrlcpy(t_code *dst, t_code *src, size_t cnt)
{
	const t_code *s    = src;
	size_t        size = cnt;
	
	if(cnt-- >= 1)
	{
		w_code ch;
		do
		{
			if(cnt < _tccnt(src)) {cnt = 0; break;}
			ch = _tcnext(&src);
			_tcsetc(&dst, ch);
			if(ch == _T('\0')) {break;}
		}while((cnt -= _tccntb(ch)) != 0);
	}
	if(cnt == 0)
	{
		if(size != 0) {_tcset(dst, _T('\0'));}
		while(_tcnext(&src) != _T('\0')) ;
	}
	return(src - s - 1);
}

size_t _tstrlcat(t_code *dst, t_code *src, size_t cnt)
{
	t_code       *d = dst;
	const t_code *s = src;
	size_t        len;
	
	while(cnt != 0 && _tccode(dst) != _T('\0'))
	{
		u_int n = _tccnt(dst);
		
		if(cnt < n) {cnt = 0; break;}
		cnt -= n;
		dst += n;
	}
	len = dst - d;
	
	if(cnt == 0) {return(len + _tcslen(src));}
	while(*src)
	{
		w_code ch = _tccode(src);
		if(cnt != 1)
		{
			if(cnt < _tccntb(ch)) {cnt = 1;}
			else
			{
				_tcsetc(&dst, ch);
				cnt -= _tccntb(ch);
			}
		}
		src += _tccntb(ch);
	}
	*dst = _T('\0');
	return(len + (src - s));
}

#ifndef __CYGWIN__
s_int _w32_sntprintf(t_code *buf, size_t size, t_code *format, ...)
{
	s_int   ret;
	va_list va;

	va_start(va, format);
	ret = _vsntprintf(buf, size, format, va);
	va_end(va);
	
	if(size > 0) {buf[size - 1] = _T('\0');}
	return(ret);
}
#endif // __CYGWIN__
