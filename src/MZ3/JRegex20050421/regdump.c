#include "jectlib.h"
#include "jectlib.c"
#include "JChar.c"
#include "JRegex.c"
#include <stdio.h>
#include <locale.h>

t_code cc2sc(t_code ch)
{
	switch(ch)
	{
		case _T(' '):   return(_T('`')); break;
		case _T('\t'):  return(_T('T')); break;
		case _T('\v'):  return(_T('V')); break;
		case _T('\n'):  return(_T('N')); break;
		case _T('\r'):  return(_T('R')); break;
		case _T('\f'):  return(_T('F')); break;
		case _T('\b'):  return(_T('B')); break;
		case _T('\a'):  return(_T('A')); break;
		case _T('\033'):return(_T('E')); break;
		case _T('N'):   return(_T('n')); break;
	}
	return(ch);
}

int _tmain(s_int argc, TCHAR *argv[])
{
	s_int i;
	
	if(argc <= 1) {_tprintf("no-arg"); return(0);}
	
	_tsetlocale(LC_ALL, _T("Japanese"));
	for(i = 1; i < argc; i++)
	{
		jreg *reg = jreg_comp(argv[i], JREG_NORMAL | JREG_DFA);
		u_int iy, ix;
		
		_tprintf(_T("m/%s/\n"), argv[i]);
		if(!reg) {_tprintf(_T("  compile-error!!\n\n")); continue;}
		
		_tprintf(_T("             "));
		for(ix = 1; ix < reg->nodemax; ix++) {_tprintf(_T("%d "), ix % 10);}
		_tprintf(_T("\n"));
		
		for(iy = 0; iy < reg->nodemax; iy++)
		{
			JNODE *node = _dfa_index(reg->regd, reg->nodemax, iy);
			
			_tprintf(_T(" %2d "), iy);
			switch(_type_get(node->type))
			{
			case JREG_STRING: /*break throw ...*/
			case JREG_CHAR:   _tprintf(_T(" [%cS:%c] "),  _type_isopt(node->type, JREG_END) ? _T('!') : _T(' '), node->Data.String.str[0]); break;
			case JREG_META:   _tprintf(_T(" [%cM:%c] "),  _type_isopt(node->type, JREG_END) ? _T('!') : _T(' '), cc2sc((t_code)node->Data.Meta.metacode)); break;
			case JREG_CCLASS: _tprintf(_T(" [%cC:%1d] "), _type_isopt(node->type, JREG_END) ? _T('!') : _T(' '), node->Data.CClass.len);    break;
			case JREG_UNION:  _tprintf(_T(" [%cU:-] "),   _type_isopt(node->type, JREG_END) ? _T('!') : _T(' ')); break;
			default:          _tprintf(_T("        ")); break;
			}
			for(ix = 0; ix < reg->nodemax - 1; ix++)
			{
				_tprintf(_T(" %c"), _dfa_fetch(reg->regd, reg->nodemax, iy, ix) ? _T('x') : _T('-'));
			}
			_tprintf(_T("\n"));
		}
		_tprintf(_T("\n"));
		jreg_free(reg);
	}
	return(0);
}
