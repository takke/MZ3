#include "jectlib.h"
#include "JChar.h"
#include "JRegex.h"
#include <string.h>
#include <ctype.h>


#define JREG_REPEAT_INFINITE _limitu(u_int) // ���[�v�񐔏���Ȃ�

	//< �����Ώۂ��當�����擾
#ifdef _JREG_SMARTCODE
# define _rstr2ch(p,n) ((w_code)(p)->Pattern.PATTERN_TCODE[n])
#else
# define _rstr2ch(p,n) \
	((w_code)(((p)->execopt & JREG_STRINGFUNC) ? \
		(p)->Pattern.PATTERN_FUNC(n, (p)->usrval) \
		: (p)->Pattern.PATTERN_TCODE[n]))
#endif // _JREG_SMARTCODE

#ifdef UNICODE
# define _rstrcnt(p,n)  1                 //< �����Ώۂ̕��������o�C�g���擾
# define _rstr2tc(p,n)  _rstr2ch(p,n)     //< �����Ώۂ���1�������擾
# define _rstrprev(p,n) ((n) > 0 ? 1 : 0) //< �����Ώۂ���O���̕������擾
#else
static u_int _rstrcnt(jreg *reg, u_int cnt)
{
	w_code ch = _rstr2ch(reg, cnt);
	if(!_iskanji1(ch)) {return(1);}
	ch = _rstr2ch(reg,cnt + 1);
	if(_iskanji2(ch)) {return(2);}
	return(1);
}
static w_code _rstr2tc(jreg *reg, u_int cnt)
{
	w_code ch = _rstr2ch(reg, cnt);
	if(!_iskanji1(ch)) {return(ch);}
	else
	{
		w_code ch2 = _rstr2ch(reg, cnt + 1);
		if(_iskanji2(ch2)) {return(_mkkanji(ch, ch2));}
	}
	return(ch);
}
static u_int _rstrprev(jreg *reg, u_int cnt)
{
	if(cnt == 0) {return(0);}
	else
	{
		w_code ch = _rstr2ch(reg, cnt - 1);
		if(ch <= 0x3F || 0xFD <= ch || ch == 0x7F || cnt == 1) {return(1);}
		ch = _rstr2ch(reg, cnt - 2);
		if(jregex__range(0x81, 0x9F, ch) || jregex__range(0xE0, 0xFC, ch)) {return(2);}
	}
	return(1);
}
#endif // UNICODE

	//< �����Ώۂ̕����Ɣ�r
static bool _rstrcmp(jreg *reg, u_int cnt, w_code ch)
{
	w_code cch = _rstr2tc(reg, cnt);
	
	return(reg->execopt & JREG_ICASE ? _tclc(cch) == _tclc(ch) : cch == ch);
}

#define _type_set(node,c)    ((node) = (JREG_TYPE)((JREG_NODETYPE & (c)) | (~JREG_NODETYPE & (node))))
#define _type_optadd(node,c) ((node) = (JREG_TYPE)((node) | (JREG_OPTIONTYPE & (c))))
#define _type_optrem(node,c) ((node) &= (~(JREG_OPTIONTYPE & (c))))
#define _type_get(node)      ((node) & JREG_NODETYPE)
#define _type_opt(node)      ((node) & JREG_OPTIONTYPE)
#define _type_istype(node,c) ((JREG_NODETYPE & (node)) == (c))
#define _type_isopt(node,c)  ((JREG_OPTIONTYPE & (node)) & (c))

	//- ���K�\���m�[�h�^�C�v
typedef enum _JREG_TYPE
{
	JREG_EMPTY      = 0x0000, // ��m�[�h��\��
	
	JREG_NODETYPE   = 0x000F, // �m�[�h���ނ̃}�X�N�p�^�[��
	JREG_CHAR       = 0x0001, // �g���s������
	JREG_STRING     = 0x0002, // �g��������
	JREG_META       = 0x0003, // ���^����   (\w \d \s \x81 \z �Ȃ�)
	JREG_CCLASS     = 0x0004, // �����N���X ([...] �ɑ���)
	JREG_CLOSURE    = 0x0005, // �����q     (* + ? {n} �Ȃ�)
	JREG_UNION      = 0x0006, // ��       ((...) �ɑ���)
	
	JREG_OPTIONTYPE = 0xFFF0, // �m�[�h�I�v�V�����̃}�X�N�p�^�[��
	JREG_HEAD       = 0x0010, // ��̐擪 (�g�����K�\���̔���)
	JREG_NONGREEDY  = 0x0020, // �ŒZ��v   (�����q�m�[�h�ɗL��)
	JREG_MOREGREEDY = 0x0040, // �����v   (�����q�m�[�h�ɗL��)
	JREG_NOT        = 0x0080, // �ے��v   ([^...] (?!...) �Ȃ�)
	JREG_NOWIDTH    = 0x0100, // ��ǂ�     ((?=...) �Ȃ�)
	JREG_BACKREF    = 0x0200, // ����Q�ƗL (��m�[�h�ɗL��)
	JREG_NOVLEN     = 0x0400, // �߂�ǂ�   ((?<=...) �Ȃ�)
	JREG_END        = 0x0800, // �m�[�h�I�[
}JREG_TYPE;

	//- �L�����N�^�R�[�h�萔
typedef enum _JREGCC
{
	JREGCC_CHAR,      // �����\��
	JREGCC_META,      // ���^�����\��
	JREGCC_RANGE,     // �����͈͂�\�� `-'
	JREGCC_BACKREF,   // ����Q��
	JREGCC_CTRL,      // �A���J�[
	JREGCC_PROC,      // ���O�t�����N���X
	JREGCC_MATCHALL,  // �}�b�`�ʒu�S��
	JREGCC_MATCHBEF,  // �}�b�`�ʒu�O��
	JREGCC_MATCHAFT,  // �}�b�`�ʒu���
	JREGCC_MATCHLAST, // �ŏI��}�b�`�ʒu
	JREGCC_ERROR,     // �G���[
}JREGCC;

typedef struct _JNODE_STRING
{
	t_code *str;     // ������擪 (jreg.regstr�̕����񒆂��w��)
	u_int   wordcnt; // ������
	u_int   bytecnt; // �o�C�g��
}JNODE_STRING;

typedef struct _JNODE_META
{
	JREGCC type;     // ���^������
	w_code metacode; // ���^�������ʃ^�C�v
	u_int  opt;      // �I�v�V�����l (���^������ɂ�藘�p���@���قȂ�)
}JNODE_META;

typedef struct _JNODE_CCLASS
{
	t_code *ccstr; // ������擪 (jreg.regstr�̕����񒆂��w��)
	u_int   len;   // ������
}JNODE_CCLASS;

typedef struct _JNODE_CLOSURE
{
	u_int  lower;   // ���[�v�񐔉���
	u_int  upper;   // ���[�v�񐔏��
	JNODE *closure; // ���[�v�m�[�h
	u_int  loopno;  // ���[�v�񐔎Q�ƈʒu
}JNODE_CLOSURE;

typedef struct _JNODE_UNION
{
	JNODE *next;    // �q�m�[�h
	JNODE *epsilon; // ��J�ڐ�m�[�h
	u_int  unino;   // �}�b�`�ʒu�Q�ƈʒu
}JNODE_UNION;

#define BM_CHAR_LIMIT 127
#define BM_LEN_MIN    2
#define BM_LEN_MAX    64
typedef u_char bmskip;

struct _JNODE
{
	JREG_TYPE type; // �m�[�h���
	union
	{
		JNODE_STRING   String;  // ������
		JNODE_STRING   Char;    // ���� (String�Ɠ��`)
		JNODE_META     Meta;    // ���^����
		JNODE_CCLASS   CClass;  // �����N���X
		JNODE_CLOSURE  Closure; // ����
		JNODE_UNION    Union;   // ��
	}Data;
	JNODE *next; // �}�b�`�����ۂ̑J�ڃm�[�h
	u_int  selnum;
};

struct _JLOOP
{
	u_int loopcnt;
	u_int prev_start;
};

struct _JBM
{
	u_int  len;
	bmskip skip[BM_CHAR_LIMIT + 1];
};

	//- DFA�e�[�u���⏕
#define _bit_offset(n)         ((n) / 8)
#define _bit_index(n)          ((n) % 8)
#define _bit_set(r,n)          do{((byte *)(r))[_bit_offset(n)] |= 0x01 << _bit_index(n);}while(0)
#define _bit_fetch(r,n)          (((byte *)(r))[_bit_offset(n)] &  0x01 << _bit_index(n))
#define _dfa_tblsize(n)        (_bit_offset(n) + 1)
#define _dfa_sizeof(n)         ((n) * (sizeof(JNODE) + _dfa_tblsize(n)) + _dfa_tblsize(n) * 4)
#define _dfa_index(p,s,n)      ((JNODE *)(((byte *)p) + (sizeof(JNODE) + _dfa_tblsize(s)) * (n)))
#define _dfa_set(p,s,x,n)      _bit_set(_dfa_index(p, s, x) + 1, n)
#define _dfa_fetch(p,s,x,n)    _bit_fetch(_dfa_index(p, s, x) + 1, n)
#define _dfa_tblar(p,s,n)      ((byte *)_dfa_index(p, s, s) + _dfa_tblsize(s) * (n))
#define _dfa_istbl(r,s,f,x)    do{for(x = 0, f = false; x <= _bit_offset(s); (x)++) {if(((byte *)(r))[x]   > 0x00) {f = true; break;}}}while(0)
#define _dfa_isor(r1,r2,s,f,x) do{for(x = 0, f = false; x <= _bit_offset(s); (x)++) {if((((byte *)(r1))[x] | ((byte *)(r2))[x]) > 0x00) {f = true; break;}}}while(0)
#define _dfa_clear(r,s,x)      do{for(x = 0           ; x <= _bit_offset(s); (x)++) {((byte *)(r))[x]  =  0x00;}}while(0)
#define _dfa_or(r1,r2,s,x)     do{for(x = 0           ; x <= _bit_offset(s); (x)++) {((byte *)(r1))[x] |=  ((byte *)(r2))[x];}}while(0)
#define _dfa_nand(r1,r2,s,x)   do{for(x = 0           ; x <= _bit_offset(s); (x)++) {((byte *)(r1))[x] &= ~((byte *)(r2))[x];}}while(0)
#define _dfa_eq(r1,r2,s,x)     do{for(x = 0           ; x <= _bit_offset(s); (x)++) {((byte *)(r1))[x] =   ((byte *)(r2))[x];}}while(0)

	//- �e��ԍ\���̒�`
typedef struct _JREGPARENT
{
	JNODE *node;                // �e�m�[�h���g
	struct _JREGPARENT *Parent; // �e��ԏ��
	u_int  start;               // �ŏ��̎q�m�[�h�̃}�b�`�J�n�ʒu
}JREGPARENT;

	//- ���O�t�����N���X�^�C�v
typedef enum _JREGISCC
{
	JREGISCC_EMPTY      = 0x0000,// ����`

	JREGISCC_TYPE       = 0x00FF, // �^�C�v���ނ̃}�X�N�p�^�[��
	JREGISCC_ALNUM      = 0x0001, // [:alnum:]
	JREGISCC_ALPHA      = 0x0002, // [:alpha:]
	JREGISCC_ASCII      = 0x0003, // [:ascii:]
	JREGISCC_CNTRL      = 0x0004, // [:cntrl:]
	JREGISCC_XDIGIT     = 0x0005, // [:xdigit:]
	JREGISCC_DIGIT      = 0x0006, // [:digit:]
	JREGISCC_GRAPH      = 0x0007, // [:graph:]
	JREGISCC_PRINT      = 0x0008, // [:print:]
	JREGISCC_PUNCT      = 0x0009, // [:punct:]
	JREGISCC_SPACE      = 0x000A, // [:space:]
	JREGISCC_UPPER      = 0x000B, // [:upper:]
	JREGISCC_LOWER      = 0x000C, // [:lower:]
	
	JREGISCC_BLANK      = 0x000D, // [:blank:]
	JREGISCC_KANA       = 0x000E, // [:kana:]
	JREGISCC_MBALPHA    = 0x000F, // [:mbalpha:]
	JREGISCC_MBDIGIT    = 0x0010, // [:mbdigit:]
	JREGISCC_MBSPACE    = 0x0011, // [:mbspace:]
	JREGISCC_MBUPPER    = 0x0012, // [:mbupper:]
	JREGISCC_MBLOWER    = 0x0013, // [:mblower:]
	JREGISCC_MBHIRA     = 0x0014, // [:mbhira:]
	JREGISCC_MBKATA     = 0x0015, // [:mbkata:]
	JREGISCC_MBGREECE   = 0x0016, // [:mbgreece:]
	JREGISCC_MBRUSSIA   = 0x0017, // [:mbrussia:]
	JREGISCC_MBRUSUPPER = 0x0018, // [:mbrusupper:]
	JREGISCC_MBRUSLOWER = 0x0019, // [:mbruslower:]
	JREGISCC_MBSTR      = 0x0020, // [:mbstr:]

	JREGISCC_NOT        = 0x0100, // [:^xxx:]

	JREGISCC_ERROR      = 0x0200, // �G���[
}JREGISCC;

	//- ���O�t�����N���X���
typedef struct _JREGCCNAME
{
	t_code   *ccstr; // [:xxx:] �� xxx �̕������w��
	JREGISCC  type;  // �^�C�v
}JREGCCNAME;

	//- ���O�t�����N���X
static const JREGCCNAME s_cctbl[] = {
#define ct(p,t) {(t_code *)_T(p),t}
	ct("alnum",  JREGISCC_ALNUM),
	ct("alpha",  JREGISCC_ALPHA),
	ct("ascii",  JREGISCC_ASCII),
	ct("cntrl",  JREGISCC_CNTRL),
	ct("xdigit", JREGISCC_XDIGIT),
	ct("digit",  JREGISCC_DIGIT),
	ct("graph",  JREGISCC_GRAPH),
	ct("print",  JREGISCC_PRINT),
	ct("punct",  JREGISCC_PUNCT),
	ct("space",  JREGISCC_SPACE),
	ct("upper",  JREGISCC_UPPER),
	ct("lower",  JREGISCC_LOWER),
	
	ct("blank",      JREGISCC_BLANK),
	ct("kana",       JREGISCC_KANA),
	ct("mbalpha",    JREGISCC_MBALPHA),
	ct("mbdigit",    JREGISCC_MBDIGIT),
	ct("mbspace",    JREGISCC_MBSPACE),
	ct("mbupper",    JREGISCC_MBUPPER),
	ct("mblower",    JREGISCC_MBLOWER),
	ct("mbhira",     JREGISCC_MBHIRA),
	ct("mbkata",     JREGISCC_MBKATA),
	ct("mbgreece",   JREGISCC_MBGREECE),
	ct("mbrussia",   JREGISCC_MBRUSSIA),
	ct("mbrusupper", JREGISCC_MBRUSUPPER),
	ct("mbruslower", JREGISCC_MBRUSLOWER),
	ct("mbstr",      JREGISCC_MBSTR),
#undef ct
};


static bool  jreg_comp_dfa  (jreg  *reg);
static bool  jreg_node_split(JNODE *node, u_int *cnt, JNODE **prnext);
static bool  jreg_node_copy (JNODE *orig_node, JNODE **prnext);
static bool  jreg_dfa_buff  (jreg  *reg, u_int idx, JREGPARENT *Parent, JNODE *node);
static u_int jreg_scan_dfa  (jreg  *reg, u_int start);



static JNODE* jreg_prev_node(JNODE *par_node);

static JNODE* jreg_new_node(JNODE *par_node, JREG_TYPE type)
{
	JNODE *node;

	if(par_node && _type_get(par_node->type) == JREG_EMPTY)
	{
		if(_type_istype(type, JREG_CLOSURE)) {return(null);}
		_type_set(par_node->type, type);
		node = par_node;
	}
	else {node = (JNODE *)malloc(sizeof(JNODE));}
	
	if(!node) {return(null);}
	
	node->selnum = 0;
	switch(_type_get(type))
	{
	case JREG_CLOSURE: //! closure�m�[�h�͒��O�m�[�h����荞��
		{
			JNODE *prev_node;
			
			node->next = null;
			node->Data.Closure.closure = null;
			if(!(prev_node = jreg_prev_node(par_node))) {break;}
			if(par_node == prev_node)
			{
				*node = *par_node;
				par_node->type = type;
				par_node->Data.Closure.closure = node;
				return(par_node);
			}
			else
			{
				par_node->next = node;
				node->Data.Closure.closure = prev_node;
				node->type = type;
				return(node);
			}
		}
		break;
	case JREG_UNION:
		node->Data.Union.next    = null;
		node->Data.Union.epsilon = null;
	default:
		if(par_node) {par_node->next = node;}
		node->next = null;
		node->type = type;
		return(node);
		break;
	}
	if(node != par_node) {free(node);}
	return(null);
}

static JNODE* jreg_prev_node(JNODE *par_node)
{
	if(!par_node) {return(null);}
	
	switch(_type_get(par_node->type))
	{
	case JREG_STRING: //! string�m�[�h�͍Ō��1������؂�o��
		{
			JNODE *node;
			t_code *str;
			u_int  byte;
			
			if(par_node->Data.String.wordcnt == 1)
			{
				par_node->type = JREG_CHAR;
				return(par_node);
			}
			if(!(node = jreg_new_node(par_node, JREG_CHAR))) {return(null);}
			str  = par_node->Data.String.str;
			byte = par_node->Data.String.bytecnt;
			node->Data.Char.wordcnt = 1;
			node->Data.Char.bytecnt = _tcprev(str + byte, byte);
			par_node->Data.String.bytecnt -= node->Data.Char.bytecnt;
			par_node->Data.String.wordcnt--;
			node->Data.Char.str = par_node->Data.String.str + par_node->Data.String.bytecnt;
			return(node);
		}
		break;
	case JREG_CHAR:
	case JREG_CCLASS:
	case JREG_META:
	case JREG_UNION:
		return(par_node);
		break;
	}
	return(null);
}

/*<
	���K�\���m�[�h���}�b�`�����ۂ̒������擾����
	
	//$node [in] ���K�\���m�[�h���
	//$type [in] ���K�\���m�[�h�^�C�v
	
	//! �߂�ǂ݂̐������R���p�C���ɕK�v
*/
static u_int jreg_get_cnt(JNODE *node, JREG_TYPE type)
{
	u_int cnt;

	if(!node) {return(1);}
	
	if((cnt = jreg_get_cnt(node->next, type)) == 0) {return(0);}
	cnt--;
	switch(_type_get(node->type))
	{
	case JREG_STRING: case JREG_CHAR: //! string/char�m�[�h�͕�������Ԃ�
		return(cnt + node->Data.String.wordcnt + 1);
		break;
	case JREG_CCLASS: //! cclass�m�[�h�͕�����1
		return(cnt + 2);
		break;
	case JREG_META:
		switch(node->Data.Meta.type)
		{
		case JREGCC_CHAR: case JREGCC_META: case JREGCC_PROC: // �����A���^�����A���O�t�����N���X�͕�����1
			return(cnt + 2);
			break;
		case JREGCC_CTRL:// �A���J�[�͕�����0
			return(cnt + 1);
			break;
		case JREGCC_BACKREF: //! ����Q�Ƃ͖߂�ǂݓ����ł̓G���[
			if(_type_isopt(type, JREG_NOVLEN)) {return(0);}
			return(cnt + 1);
			break;
		default: return(0); break;
		}
		break;
	case JREG_UNION: //! union�m�[�h�͖߂�ǂ݂̍� (p1|p2) �̍��E�̕����̒������ꏏ�łȂ��ƃG���[
		{
			u_int lcnt, rcnt;

			_type_optadd(type, node->type & JREG_NOVLEN);
			if((lcnt = jreg_get_cnt(node->Data.Union.next,    type)) == 0) {return(0);}
			if((rcnt = jreg_get_cnt(node->Data.Union.epsilon, type)) == 0) {return(0);}
			if(_type_isopt(type, JREG_NOVLEN))
			{
				if(node->Data.Union.epsilon && lcnt != rcnt) {return(0);}
				node->Data.Union.unino = lcnt; //! �߂�ǂ݃m�[�h�͂��炩���߃}�b�`�����Z�b�g���Ă���
			}
			return(cnt + lcnt);
		}
		break;
	case JREG_CLOSURE: //! closure�m�[�h�͖߂�ǂݓ����ł̉񐔕s��̏ꍇ�G���[
		{
			u_int ccnt;

			if(_type_isopt(type, JREG_NOVLEN) && node->Data.Closure.lower != node->Data.Closure.upper) {return(0);}
			if((ccnt = jreg_get_cnt(node->Data.Closure.closure, type)) == 0) {return(0);}
			return(cnt + (ccnt - 1) * node->Data.Closure.lower + 1);
		}
		break;
	case JREG_EMPTY:
		if(_type_isopt(node->type, JREG_END)) {return(cnt + 1);}
		break;
	}
	return(0);
}

static JREGCC jreg_meta(jreg *reg, t_code *regstr, u_int *cnt, w_code *ch, u_int *opt, bool ismeta);

static JREGCC jreg_meta_ctrl(jreg *reg, t_code *regstr, u_int *cnt, w_code *ch)
{
	switch(*ch)
	{
	case _T('M'): //# M-x �`���̃R�[�h��F��
		if(_tccode(regstr + *cnt) != _T('-')) {return(JREGCC_ERROR);}
		*cnt += _tccnt(regstr + *cnt);
		if(jreg_meta(reg, regstr, cnt, ch, null, false) != JREGCC_CHAR) {return(JREGCC_ERROR);}
		*ch = (w_code)((*ch & 0xFF) | 0x80);
		return(JREGCC_CHAR);
		break;
	case _T('C'): //# C-x �`���̃R�[�h��F�� (���ۂ̉�͕����� cx �̕����Ɉˑ�)
		if(_tccode(regstr + *cnt) != _T('-')) {return(JREGCC_ERROR);}
		*cnt += _tccnt(regstr + *cnt);
	case _T('c'): //# cx �`���̃R�[�h��F��
		if(jreg_meta(reg, regstr, cnt, ch, null, false) != JREGCC_CHAR) {return(JREGCC_ERROR);}
		*ch = (w_code)(*ch & 0x9F);
		return(JREGCC_CHAR);
		break;
	}
	return(JREGCC_ERROR);
}

static JREGCC jreg_meta(jreg *reg, t_code *regstr, u_int *cnt, w_code *ch, u_int *opt, bool ismeta)
{
	*ch  =  _tccode(regstr + *cnt);
	*cnt += _tccnt (regstr + *cnt);
	if(*ch == _T('\0')) {return(JREGCC_ERROR);}
	if(*ch == _T('.'))
	{
		if(!ismeta) {return(JREGCC_CHAR);} //+ �����N���X���ł� . �� . ���̂��̂�\��
		return(JREGCC_META);
	}
	if(ismeta && (*ch == _T('^') || *ch == _T('$'))) {return(JREGCC_CTRL);}
	if(*ch != _T('\\')) {return(JREGCC_CHAR);}
	*ch  =  _tccode(regstr + *cnt);
	*cnt += _tccnt (regstr + *cnt);
	if(ismeta)
	{
		switch(*ch)
		{
		case _T('b'): case _T('B'): case _T('j'): case _T('J'):
		case _T('A'): case _T('z'): case _T('Z'):
		case _T('<'): case _T('>'):
			return(JREGCC_CTRL);
			break;
		case _T('N'):
			return(JREGCC_META);
			break;
		case _T('n'): //# �Ǝ��g���AJREG_BINARYMODE�łȂ���� \n �� \N �͓��`
			if(!(reg->compopt & JREG_BINARYMODE)) {return(JREGCC_META);}
			break;
		}
	}
	switch(*ch)
	{
	case _T('\0'): return(JREGCC_ERROR); break;

	case _T('t'): *ch = (w_code)_T('\t'); return(JREGCC_CHAR); break;
	case _T('v'): *ch = (w_code)_T('\v'); return(JREGCC_CHAR); break;
	case _T('n'): *ch = (w_code)_T('\n'); return(JREGCC_CHAR); break;
	case _T('r'): *ch = (w_code)_T('\r'); return(JREGCC_CHAR); break;
	case _T('f'): *ch = (w_code)_T('\f'); return(JREGCC_CHAR); break;
	case _T('b'): *ch = (w_code)_T('\b'); return(JREGCC_CHAR); break;
	case _T('a'): *ch = (w_code)_T('\a'); return(JREGCC_CHAR); break;
	case _T('e'): *ch = (w_code)_T('\x1b'); return(JREGCC_CHAR); break;

	case _T('w'): case _T('s'): case _T('d'): case _T('k'):
	case _T('W'): case _T('S'): case _T('D'): case _T('K'):
		return(JREGCC_META);
		break;
	case _T('M'): case _T('C'): case _T('c'):
		return(jreg_meta_ctrl(reg, regstr, cnt, ch));
		break;
	case _T('x'):
		{
			word num = 0;

			*ch = _tccode(regstr + *cnt);
			while(1)
			{
				if(     jregex__range(_T('0'), _T('9'), *ch)) {num = (word)(*ch - _T('0') + num * 16);}
				else if(jregex__range(_T('a'), _T('f'), *ch)) {num = (word)(*ch - _T('a') + 10 + num * 16);}
				else if(jregex__range(_T('A'), _T('F'), *ch)) {num = (word)(*ch - _T('A') + 10 + num * 16);}
				else {break;}
				*cnt += _tccnt (regstr + *cnt);
				*ch  =  _tccode(regstr + *cnt);
			}
			*ch = (w_code)num;
			return(JREGCC_CHAR);
		}
		break;
	case _T('o'): case _T('0'):
		{
			word num = 0;

			*ch = _tccode(regstr + *cnt);
			while(jregex__range(_T('0'), _T('7'), *ch))
			{
				num  =  (word)(*ch - _T('0') + num * 8);
				*cnt += _tccnt (regstr + *cnt);
				*ch  =  _tccode(regstr + *cnt);
			}
			*ch = (w_code)num;
			return(JREGCC_CHAR);
		}
		break;
	case _T('1'): case _T('2'): case _T('3'): case _T('4'): case _T('5'):
	case _T('6'): case _T('7'): case _T('8'): case _T('9'):
		{
			word num = 0;

			do
			{
				num = (word)(*ch - _T('0') + num * 10);
				*ch = _tccode(regstr + *cnt);
				if(!jregex__range(_T('0'), _T('9'), *ch)) {break;}
				*cnt += _tccnt(regstr + *cnt);
			}while(1);
			if(opt) {*opt = (u_int)num;}
			return(JREGCC_BACKREF);
		}
		break;
	}
	return(JREGCC_CHAR);
}

static JREGCC jreg_metax(jreg *reg, t_code *regstr, u_int *cnt, w_code *ch, u_int *opt)
{
	*ch = _tccode(regstr + *cnt);
	if(*ch != _T('$')) {return(jreg_meta(reg, regstr, cnt, ch, opt, false));} //! $ �Ŏn�܂郁�^�����ȊO�� jreg_meta �Ńp�[�X
	*cnt += _tccnt (regstr + *cnt);
	*ch  =  _tccode(regstr + *cnt);
	*cnt += _tccnt (regstr + *cnt);
	switch(*ch)
	{
	case _T('\0'): case _T('0'): return(JREGCC_ERROR); break;
	
	case _T('&'):  return(JREGCC_MATCHALL); break;
	case _T('`'):  return(JREGCC_MATCHBEF); break;
	case _T('\''): return(JREGCC_MATCHAFT); break;
	case _T('+'):  return(JREGCC_MATCHLAST); break;
	
	case _T('1'): case _T('2'): case _T('3'): case _T('4'): case _T('5'):
	case _T('6'): case _T('7'): case _T('8'): case _T('9'):
		{
			word num = 0;

			do
			{
				num = (word)(*ch - _T('0') + num * 10);
				*ch = _tccode(regstr + *cnt);
				if(!jregex__range(_T('0'), _T('9'), *ch)) {break;}
				*cnt += _tccnt(regstr + *cnt);
			}while(1);
			if(opt) {*opt = (u_int)num;}
			return(JREGCC_BACKREF);
		}
		break;
	}
	return(JREGCC_CHAR);
}

static JREGCC jreg_cc(jreg *reg, t_code *regstr, u_int *cnt, w_code *ch, u_int *opt, bool range)
{
	switch(*ch = _tccode(regstr + *cnt))
	{
	case _T(']'):
		return(JREGCC_ERROR);
		break;
	case _T('['):
		{
			w_code   wrd;
			u_int    i, j, len, start;
			JREGISCC type = JREGISCC_EMPTY;

			*cnt += _tccnt(regstr + *cnt);
			if(_tccode(regstr + *cnt) != _T(':')) {return(JREGCC_ERROR);}
			start = (*cnt += _tccnt(regstr + *cnt));
			if(_tccode(regstr + *cnt) == _T('^'))
			{
				start = (*cnt += _tccnt(regstr + *cnt));
				type  = JREGISCC_NOT;
			}
			for(i = 0; (wrd = _tccode(regstr + *cnt + i)) != _T(':'); i += _tccnt(regstr + *cnt + i))
			{
				if(wrd == _T('\0')) {return(JREGCC_ERROR);}
			}
			*cnt = *cnt + i + _tccnt(regstr + *cnt + i);
			if(_tccode(regstr + *cnt) != _T(']')) {return(JREGCC_ERROR);}
			*cnt += _tccnt(regstr + *cnt);
			for(len = i, j = 0; j < lengthof(s_cctbl); j++)
			{
				for(i = 0; i < len;i += _tccnt(regstr + start + i))
				{
					if(_tccode(regstr + start + i) != _tccode(s_cctbl[j].ccstr + i)) {goto __CCNAME_UNMATCH__;}
				}
				if(_tccode(s_cctbl[j].ccstr + i) != _T('\0')) {break;}
				type = (JREGISCC)(type | s_cctbl[j].type);
				break;
__CCNAME_UNMATCH__:
				continue;
			}
			if((type & JREGISCC_TYPE) == JREGISCC_EMPTY || (type & JREGISCC_TYPE) == JREGISCC_NOT) {return(JREGCC_ERROR);}
			if(opt) {*opt = (u_int)type;}
			return(JREGCC_PROC);
		}
		break;
	case _T('.'): case _T('\\'):
		switch(jreg_meta(reg, regstr, cnt, ch, opt, false))
		{
		case JREGCC_CHAR: return(JREGCC_CHAR); break;
		case JREGCC_META: return(JREGCC_META); break;
		default: return(JREGCC_ERROR); break;
		}
		break;
	case _T('-'):
		if(range)
		{
			*cnt += _tccnt(regstr + *cnt);
			return(JREGCC_RANGE);
		}
	default:
		*cnt += _tccnt(regstr + *cnt);
		return(JREGCC_CHAR);
		break;
	}
	return(JREGCC_ERROR);
}

static bool jreg_isstr(jreg *reg, u_int *cnt, t_code *str, u_int len)
{
	u_int i;
	
	if(*cnt + len > reg->reglen) {return(false);}
	
	for(i = 0; i < len; i += _rstrcnt(reg, *cnt + i))
	{
		if(!_rstrcmp(reg, *cnt + i, _tccode(str + i))) {return(false);}
	}
	*cnt += i;
	return(true);
}

static bool jreg_ismeta(jreg *reg, u_int *cnt, w_code ch)
{
	if(*cnt >= reg->reglen) {return(false);}
	else
	{
		tc_type type  = _tctype(_rstr2tc(reg, *cnt));
		u_int   chcnt = _rstrcnt(reg, *cnt);
		
		*cnt += chcnt;
		switch(ch)
		{
		case _T('w'): if(type & tc_w) {return(true);} break;
		case _T('s'): if(type & tc_s) {return(true);} break;
		case _T('d'): if(type & tc_d) {return(true);} break;
		case _T('k'): if(type & tc_k) {return(true);} break;
		case _T('W'): if(!(type & tc_w)) {return(true);} break;
		case _T('S'): if(!(type & tc_s)) {return(true);} break;
		case _T('D'): if(!(type & tc_d)) {return(true);} break;
		case _T('K'): if(!(type & tc_k)) {return(true);} break;
		case _T('.'):
			if((reg->compopt & JREG_BINARYMODE) && (type & tc_crlf)) {return(true);}
			if(type & (tc_null | tc_crlf)) {break;}
			return(true);
			break;
		case _T('n'): case _T('N'):
			if(type & tc_lf) {return(true);}
			if(type & tc_cr)
			{
				type = _tctype(_rstr2tc(reg, *cnt));
				if(type & tc_lf) {*cnt += _rstrcnt(reg, *cnt);}
				return(true);
			}
			break;
		}
		*cnt -= chcnt;
	}
	return(false);
}

static bool jreg_ismeta_anc(jreg *reg, u_int *cnt, w_code ch)
{
	tc_type type, prevtype;
	bool    ret = false;

	if(*cnt == 0) {prevtype = tc_null;}
	else {prevtype = _tctype(_rstr2tc(reg, *cnt - _rstrprev(reg, *cnt)));}
	if(*cnt == reg->reglen) {type = tc_null;}
	else {type = _tctype(_rstr2tc(reg, *cnt));}
	
	switch(ch)
	{
	case _T('^'): // �s���܂��͉��s�̒���A������ \r �� \n �̊Ԃɂ̓}�b�`���Ȃ�
		if(*cnt == 0) {ret = true;}
		else if(reg->compopt & JREG_STARTLINE && prevtype & tc_crlf)
		{
			if(_nand(reg->compopt, JREG_BINARYMODE) && ((prevtype == tc_cr && type==tc_lf) || *cnt == reg->reglen)) {break;}
			ret = true;
		}
		break;
	case _T('$'): // �s���܂��͉��s�̒��O�A������ \r �� \n �̊Ԃɂ̓}�b�`���Ȃ�
		if(*cnt == reg->reglen)
		{
			//! �o�b�t�@���ł����Ă����O�����s�ł���΃}�b�`���Ȃ�
			if(_nand(reg->compopt, JREG_BINARYMODE) && prevtype & tc_crlf) {break;}
			ret = true;
		}
		else if(reg->compopt & JREG_ENDLINE && type & tc_crlf)
		{
			if(_nand(reg->compopt, JREG_BINARYMODE) && prevtype == tc_cr && type==tc_lf) {break;}
			ret = true;
		}
		break;
	case _T('b'): if(  _and(prevtype, tc_w) ^ _and(type, tc_w))  {ret = true;} break;
	case _T('B'): if(!(_and(prevtype, tc_w) ^ _and(type, tc_w))) {ret = true;} break;
	case _T('j'):
		if(!(_and(prevtype, tc_s) & _and(type, tc_s)))
		{
			if(!(_and(prevtype, tc_w) & _and(type, tc_w))) {if(prevtype != type) {ret = true;}}
		}
		break;
	case _T('J'):
		if(_and(prevtype, tc_s) & _and(type, tc_s)) {ret = true;}
		else
		{
			if(_and(prevtype, tc_w) & _and(type, tc_w)) {ret = true;}
			else if(prevtype == type) {ret = true;}
		}
		break;
	case _T('A'): if(*cnt == 0) {ret = true;} break;
	case _T('Z'): if(*cnt == reg->reglen || type & tc_crlf) {ret = true;} break;
	case _T('z'): if(*cnt == reg->reglen) {ret = true;} break;
	case _T('<'): if(!(prevtype & tc_w) && type & tc_w) {ret = true;} break;
	case _T('>'): if(prevtype & tc_w && !(type & tc_w)) {ret = true;} break;
	
		//! DFA�p���^����
	case _T('N'): if(!(type & tc_lf)) {ret = true;} break;
	}
	if(ret) {*cnt++;}
	return(ret);
}

static bool jreg_iscc(jreg *reg, u_int *cnt, t_code *str, u_int len, bool isnot)
{
	u_int i;

	if(*cnt >= reg->reglen) {return(false);}
	
	for(i = 0; i < len;)
	{
		w_code ch;
		u_int  ccopt;
		
		switch(jreg_cc(reg, str, &i, &ch, &ccopt, false))
		{
		case JREGCC_CHAR: //# ����-���� �̏��ɕ���ł���Δ͈͂�\��
			{
				u_int tmpcnt = i;
				w_code range;
				
				if(jreg_cc(reg, str, &tmpcnt, &range, null, true) != JREGCC_RANGE)
				{
					if(_rstrcmp(reg, *cnt, ch)) {goto __CC_MATCH__;}
				}
				else
				{
					w_code ch2;
					
					if(jreg_cc(reg, str, &tmpcnt, &range, null, false) != JREGCC_CHAR) {continue;}
					i   = tmpcnt;
					ch2 = _rstr2tc(reg, *cnt);
					if(reg->execopt & JREG_ICASE)
					{
						if(jregex_range(ch, range, _tclc(ch2)) || jregex_range(ch, range, _tcuc(ch2))) {goto __CC_MATCH__;}
					}
					else if(jregex_range(ch, range, ch2)) {goto __CC_MATCH__;}
				}
			}
			break;
		case JREGCC_META:
			{
				u_int tmpcnt = *cnt;
				
				if(jreg_ismeta(reg, &tmpcnt, ch)) {goto __CC_MATCH__;}
			}
			break;
		case JREGCC_PROC:
			{
				ch = _rstr2tc(reg, *cnt);
				switch(ccopt & JREGISCC_TYPE) // _istxxx���p (���P�[�����f)
				{
				case JREGISCC_ALNUM:  if(_istalnum(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_ALPHA:  if(_istalpha(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_ASCII:  if(_istascii(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_CNTRL:  if(_istcntrl(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_XDIGIT: if(_istxdigit(ch) != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_DIGIT:  if(_istdigit(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_GRAPH:  if(_istgraph(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_PRINT:  if(_istprint(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_PUNCT:  if(_istpunct(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_SPACE:  if(_istspace(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_UPPER:  if(_istupper(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				case JREGISCC_LOWER:  if(_istlower(ch)  != 0) {goto __CCNAME_MATCH__;} break;
				default:
					{
						tc_type type = _tctype(ch);
						
						switch(ccopt & JREGISCC_TYPE) // �Ǝ��Ή� (���P�[������)
						{
						case JREGISCC_BLANK:      if(type & tc_blank)      {goto __CCNAME_MATCH__;} break;
						case JREGISCC_KANA:       if(type & tc_kana)       {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBALPHA:    if(type & tc_mbalpha)    {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBDIGIT:    if(type & tc_mbnum)      {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBSPACE:    if(type & tc_mbspace)    {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBUPPER:    if(type & tc_mbucalpha)  {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBLOWER:    if(type & tc_mblcalpha)  {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBHIRA:     if(type & tc_mbhira)     {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBKATA:     if(type & tc_mbkana)     {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBGREECE:   if(type & tc_mbgreece)   {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBRUSSIA:   if(type & tc_mbrussia)   {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBRUSUPPER: if(type & tc_mbucrussia) {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBRUSLOWER: if(type & tc_mblcrussia) {goto __CCNAME_MATCH__;} break;
						case JREGISCC_MBSTR:      if(type & tc_mb)         {goto __CCNAME_MATCH__;} break;
						default: return(false); break;
						}
					}
					break;
				}
				if(ccopt & JREGISCC_NOT) {goto __CC_MATCH__;} break;
__CCNAME_MATCH__:
				if(!(ccopt & JREGISCC_NOT)) {goto __CC_MATCH__;}
			}
			break;
		default:
			return(false);
			break;
		}
	}
	if(!isnot) {return(false);}
	*cnt += _rstrcnt(reg, *cnt);
	return(true);
__CC_MATCH__:
	if(isnot) {return(false);}
	*cnt += _rstrcnt(reg, *cnt);
	return(true);
}

static bool jreg_parse(jreg *reg, JNODE *par_node, t_code endch, u_int *cnt, JREG_TYPE type);

/*<
	���K�\���̊��ʂ̕������p�[�X����
	
	//$reg      [in]     ���K�\�����
	//$par_node [in/out] ���O�̐��K�\���m�[�h���
	//$endch    [in]     �I�[����
	//$cnt      [in/out] �p�[�X�ʒu
	//$type     [in]     �e��������p�������K�\���^�C�v
	
	//# �g�����K�\���ƑI��( | )�݂̂��p�[�X�����̑��� jreg_parse �Ɉς˂�
*/
static bool jreg_parse_union(jreg *reg, JNODE *par_node, t_code endch, u_int *cnt, JREG_TYPE type)
{
	par_node->Data.Union.next = jreg_new_node(null, JREG_EMPTY);

	if(!par_node->Data.Union.next) {return(false);}
	
	if(_type_isopt(par_node->type, JREG_HEAD))
	{
		if(_tccode(reg->regstr + *cnt) != _T('?'))
		{
			_type_optadd(par_node->type, JREG_BACKREF);
			par_node->Data.Union.unino = reg->backrefcnt++;
		}
		else
		{
			w_code ch;
			
			*cnt += _tccnt(reg->regstr + *cnt);
			ch   =  _tccode(reg->regstr + *cnt);
			*cnt += _tccnt(reg->regstr + *cnt);
			switch(ch)
			{
			case _T(':'): break; // (?:) ����Q�ƂȂ�
			case _T('='): // (?=) ��ǂ�
				_type_optadd(type, JREG_NOWIDTH);
				_type_optadd(par_node->type, JREG_NOWIDTH);
				break;
			case _T('!'): // (?!) ��ǂݔے�
				_type_optadd(type, JREG_NOWIDTH);
				_type_optadd(par_node->type, JREG_NOWIDTH | JREG_NOT);
				break;
			case _T('<'):
				ch   =  _tccode(reg->regstr + *cnt);
				*cnt += _tccnt(reg->regstr + *cnt);
				switch(ch)
				{
				case _T('='): // (?<=) �߂�ǂ�
					_type_optadd(type, JREG_NOVLEN);
					_type_optadd(par_node->type, JREG_NOVLEN);
					goto __UNIONEX_END__;
					break;
				case _T('!'): // (?<!) �߂�ǂݔے�
					_type_optadd(type, JREG_NOVLEN);
					_type_optadd(par_node->type, JREG_NOVLEN | JREG_NOT);
					goto __UNIONEX_END__;
					break;
				}
			default: return(0); break;
			}
		}
	}
__UNIONEX_END__:
	if(!jreg_parse(reg, par_node->Data.Union.next, endch, cnt, type)) {return(false);}
	if(_T('|') == _tccode(reg->regstr + *cnt - _tcprev(reg->regstr, *cnt))) //! �I�������� | �ł����epsilon�J�ڂ�ǉ�����
	{
		par_node->Data.Union.epsilon = jreg_new_node(null, JREG_UNION);
		
		if(!par_node->Data.Union.next) {return(false);}
		
		return(jreg_parse_union(reg, par_node->Data.Union.epsilon, endch, cnt, type));
	}
	return(true);
}

/*<
	���K�\���̊��ʈȊO�̕������p�[�X����
	
	//$reg      [in]     ���K�\�����
	//$par_node [in/out] ���O�̐��K�\���m�[�h���
	//$endch    [in]     �I�[����
	//$cnt      [in/out] �p�[�X�ʒu
	//$type     [in]     �e��������p�������K�\���^�C�v
*/
static bool jreg_parse(jreg *reg, JNODE *par_node, t_code endch, u_int *cnt, JREG_TYPE type)
{
	u_int bcnt = *cnt;
	
	*cnt += _tccnt(reg->regstr + *cnt);
	if(endch == _tccode(reg->regstr + bcnt))
	{
		_type_optadd(par_node->type, JREG_END);
		return(true);
	}
	switch(_tccode(reg->regstr + bcnt))
	{
	case _T('\0'):
		if(endch != _T('\0')) {return(false);}
	case _T('|'): //# | ������΃p�[�X�𐳏�I�� (jreg_parse_union �ő������̔��f���s����)
		_type_optadd(par_node->type, JREG_END);
		return(true);
		break;
	case _T('('):
		{
			JNODE *node = jreg_new_node(par_node, (JREG_TYPE)(JREG_UNION | JREG_HEAD));
			
			if(!node) {return(false);}
			if(!jreg_parse_union(reg, node, _T(')'), cnt, type)) {return(false);}
			return(jreg_parse(reg, node, endch, cnt, type));
		}
		break;
	case _T('['):
		{
			JNODE  *node = jreg_new_node(par_node, JREG_CCLASS);
			w_code  ch;
			u_int   i;

			if(!node) {return(false);}
			if(_tccode(reg->regstr + *cnt) == _T('^'))
			{
				_type_optadd(node->type, JREG_NOT);
				*cnt += _tccnt(reg->regstr + *cnt);
			}
			node->Data.CClass.ccstr = reg->regstr + *cnt;
			for(i = *cnt; _tccode(reg->regstr+i) != _T(']');)
			{
				if(_tccode(reg->regstr + i) == _T('\0')) {return(false);}
				if(jreg_cc(reg, reg->regstr, &i, &ch, null, false) == JREGCC_ERROR) {return(false);}
			}
			node->Data.CClass.len = i - *cnt;
			*cnt = i + _tccnt(reg->regstr + i);
			return(jreg_parse(reg, node, endch, cnt, type));
		}
		break;
	case _T('{'):
		{
			JNODE  *node = jreg_new_node(par_node, JREG_CLOSURE);
			w_code  ch;
			u_int   loopnum;

			if(!node) {return(false);}
			if(!jregex__range(_T('0'), _T('9'), (ch = _tccode(reg->regstr + *cnt)))) {return(false);}
			loopnum = 0;
			do
			{
				loopnum =  ch - _T('0') + loopnum * 10;
				*cnt    += _tccnt(reg->regstr + *cnt);
			}while(jregex__range(_T('0'), _T('9'), (ch = _tccode(reg->regstr + *cnt))));
			*cnt += _tccnt(reg->regstr + *cnt);
			node->Data.Closure.lower  = loopnum;
			node->Data.Closure.loopno = reg->closurecnt++;
			switch(ch)
			{
			case _T('}'): //# {n} �̉���
				node->Data.Closure.upper = loopnum;
				return(jreg_parse(reg, node, endch, cnt, type));
				break;
			case _T(','): //# {n,} �܂��� {n,m} �̉���
				node->Data.Closure.upper = JREG_REPEAT_INFINITE;
				if(jregex__range(_T('0'), _T('9'), (ch = _tccode(reg->regstr + *cnt))))
				{
					loopnum = 0;
					do
					{
						loopnum =  ch - _T('0') + loopnum * 10;
						*cnt    += _tccnt(reg->regstr + *cnt);
					}while(jregex__range(_T('0'), _T('9'), (ch = _tccode(reg->regstr + *cnt))));
					node->Data.Closure.upper = _max(node->Data.Closure.lower, loopnum);
					node->Data.Closure.lower = _min(node->Data.Closure.lower, loopnum);
				}
				if(ch != _T('}')) {return(false);}
				//! �߂�ǂ݂̒��ł̓��[�v�񐔕s����G���[�Ƃ���
				if(_type_isopt(type, JREG_NOVLEN) &&
					node->Data.Closure.lower != node->Data.Closure.upper) {return(false);}
				*cnt += _tccnt(reg->regstr + *cnt);
				return(jreg_parse(reg, node, endch, cnt, type));
				break;
			default: return(false); break;
			}
		}
	case _T('*'):
		if(!_type_isopt(type, JREG_NOVLEN))
		{
			JNODE *node = jreg_new_node(par_node, JREG_CLOSURE);
			
			if(!node) {return(false);}
			node->Data.Closure.lower  = 0;
			node->Data.Closure.upper  = JREG_REPEAT_INFINITE;
			node->Data.Closure.loopno = reg->closurecnt++;
			return(jreg_parse(reg, node, endch, cnt, type));
		}
		break;
	case _T('+'):
		{
			JNODE *node = jreg_new_node(par_node, JREG_CLOSURE);
			
			//+ �߂�ǂݒ��� {n}+ �͈Ӗ��͂Ȃ����A���[�v������Ƃ����Ēe���Ă͂����Ȃ�
			if(node)
			{
				if(_type_isopt(type, JREG_NOVLEN)) {return(false);}
				node->Data.Closure.lower  = 1;
				node->Data.Closure.upper  = JREG_REPEAT_INFINITE;
				node->Data.Closure.loopno = reg->closurecnt++;
				return(jreg_parse(reg, node, endch, cnt, type));
			}
			else if(_type_get(par_node->type) == JREG_CLOSURE &&
					!_type_isopt(par_node->type, JREG_NONGREEDY | JREG_MOREGREEDY))
			{
				//! ���O��closure�m�[�h�������v���[�v�ɂ���
				if(!_type_isopt(type, JREG_NOVLEN | JREG_NOWIDTH)) {reg->isnfaonly = true;}
				_type_optadd(par_node->type, JREG_MOREGREEDY);
				return(jreg_parse(reg, par_node, endch, cnt, type));
			}
		}
		break;
	case _T('?'): //# closure�m�[�h�J�n
		{
			JNODE *node = jreg_new_node(par_node, JREG_CLOSURE);
			
			//+ �߂�ǂݒ��� {n}? �͈Ӗ��͂Ȃ����A���[�v������Ƃ����Ēe���Ă͂����Ȃ�
			if(node)
			{
				if(_type_isopt(type, JREG_NOVLEN)) {return(false);}
				node->Data.Closure.lower  = 0;
				node->Data.Closure.upper  = 1;
				node->Data.Closure.loopno = reg->closurecnt++;
				return(jreg_parse(reg, node, endch, cnt, type));
			}
			else if(_type_get(par_node->type) == JREG_CLOSURE &&
					!_type_isopt(par_node->type, JREG_NONGREEDY | JREG_MOREGREEDY))
			{
				//! ���O��closure�m�[�h���ŒZ��v���[�v�ɂ���
				if(!_type_isopt(type, JREG_NOVLEN | JREG_NOWIDTH)) {reg->isnfaonly = true;}
				_type_optadd(par_node->type, JREG_NONGREEDY);
				return(jreg_parse(reg, par_node, endch, cnt, type));
			}
			return(false);
		}
		break;
	case _T('.'): case _T('\\'): case _T('^'): case _T('$'):
		{
			JNODE *node = jreg_new_node(par_node, JREG_META);
			
			if(!node) {return(false);}
			switch(node->Data.Meta.type = jreg_meta(reg, reg->regstr, &bcnt, &(node->Data.Meta.metacode), &(node->Data.Meta.opt), true))
			{
			case JREGCC_META:
				//! �Ǝ��g�� \N �� (\r\n|\r|\n) �����ɂȂ�}�b�`���s��Ȃ̂ŁA�߂�ǂ݂̒��ł��� \n(\x0A) �ɕϊ�
				if(_type_isopt(type, JREG_NOVLEN) &&
					(node->Data.Meta.metacode == _T('n') || node->Data.Meta.metacode == _T('N')))
				{
					node->Data.Meta.type = JREGCC_CHAR;
					node->Data.Meta.metacode = _T('\n');
				}
				while(0)
			case JREGCC_BACKREF:
				reg->isnfaonly = true;
			case JREGCC_CHAR:
			case JREGCC_CTRL:
			case JREGCC_PROC:
				*cnt = bcnt;
				return(jreg_parse(reg, node, endch, cnt, type));
				break;
			default:
				return(false);
				break;
			}
		}
		break;
	default:
		{
			JNODE *node;
			
			if(_type_get(par_node->type) == JREG_STRING)
			{
				par_node->Data.String.wordcnt++;
				par_node->Data.String.bytecnt += _tccnt(reg->regstr + bcnt);
				node = par_node;
			}
			else if((node = jreg_new_node(par_node, JREG_STRING)))
			{
				node->Data.String.str = reg->regstr + bcnt;
				node->Data.String.wordcnt = 1;
				node->Data.String.bytecnt = _tccnt(reg->regstr + bcnt);
			}
			else {return(false);}
			return(jreg_parse(reg, node, endch, cnt, type));
		}
		break;
	}
	return(false);
}

/*<
	�擪����̊m�蕶���񒷎擾
*/
static u_int jreg_isbm_node(JBM *bm, const JNODE *node)
{
	t_code *str = null;
	
	if(!node) {return(1);}
	if(bm) {str = (t_code*)(bm + 1);}
	
	switch(_type_get(node->type))
	{
	case JREG_CHAR:
	case JREG_STRING:
		{
			u_int i;
			
			for(i = 0; i < node->Data.String.bytecnt;)
			{
				w_code ch  = _tccode(node->Data.String.str + i);
				u_int  len = _tccntb(ch);
				
				if(!jregex__range(0, BM_CHAR_LIMIT, ch)) {return(i);}
				if(bm)
				{
					if(bm->len + len > BM_LEN_MAX) {return(i);}
					_tcset(&(str[bm->len]), ch);
					bm->len += len;
				}
				i += len;
			}
			return(i + jreg_isbm_node(bm, node->next));
		}
		break;
	case JREG_META:
		if(node->Data.Meta.type == JREGCC_CHAR)
		{
			w_code ch = node->Data.Meta.metacode;
			
			if(!jregex__range(0, BM_CHAR_LIMIT, ch)) {return(0);}
			if(bm)
			{
				if(bm->len + _tccntb(ch) > BM_LEN_MAX) {return(0);}
				_tcset(&(str[bm->len]), ch);
				bm->len += _tccntb(ch);
			}
			return(_tccntb(ch) + jreg_isbm_node(bm, node->next));
		}
		break;
	case JREG_UNION:
		{
			u_int len;
			
			if(_type_isopt(node->type, JREG_NOWIDTH | JREG_NOVLEN)) {return(1);}
			if(node->Data.Union.epsilon) {return(1);}
			if((len = jreg_isbm_node(bm, node->Data.Union.next)) <= 1) {return(1);}
			return(len - 1 + jreg_isbm_node(bm, node->next));
		}
		break;
	}
	return(1);
}

/*<
	BM�@�e�[�u���쐬
*/
static bool jreg_bm_make(jreg *reg)
{
	u_int bmlen;
	
	if(!reg || !reg->node) {return(false);}
	
	bmlen = jreg_isbm_node(null, reg->node) - 1;
	if(bmlen >= BM_LEN_MIN)
	{
		JBM     *bm;
		t_code  *str;
		u_int    i;
		
		if(!(bm = (JBM*)malloc(sizeof(JBM) + sizeof(t_code) * bmlen))) {return(false);}
		
		bm->len = 0;
		for(i = 0; i <= BM_CHAR_LIMIT; i++)
		{
			bm->skip[i] = bmlen;
		}
		jreg_isbm_node(bm, reg->node);
		str = (t_code*)(bm + 1);
		for(i = 0; i < bmlen;)
		{
			w_code ch = _tccode(str + i);
			
			bm->skip[ch] = bmlen - i - 1;
			i += _tccntb(ch);
		}
		reg->bm = bm;
	}
	return(true);
}

/*<
	���K�\���̃R���p�C�����s���܂�
	����/�u�����������s����Ƃ����̊֐��̖߂�l���K�{�ƂȂ�܂�
	
	//$str [in] ���K�\���p�^�[��
	//$opt [in] �R���p�C���I�v�V����
*/
jreg* jreg_comp(const t_code *str, jreg_opt opt)
{
	jreg  *reg;
	u_int  cnt;
	
	if(!(reg = (jreg *)malloc(sizeof(jreg)))) {return(null);}
	
	reg->regstr    = (t_code *)_tcsdup((const TCHAR *)str);
	reg->compopt   = opt;
	reg->node      = jreg_new_node(null, JREG_UNION); //! union�m�[�h���쐬�����[�g�m�[�h�ɂ���
	reg->regd      = null;
	reg->bm        = null;
	reg->isnfaonly = false;
	if(reg->node)
	{
		cnt = 0;

		reg->backrefcnt = 0;
		reg->closurecnt = 0;
		reg->backref    = null;
		reg->closure    = null;
		
		reg->node->Data.Union.unino = reg->backrefcnt++;
		if(jreg_parse_union(reg, reg->node, _T('\0'), &cnt, JREG_EMPTY))
		{
			if(jreg_get_cnt(reg->node, JREG_EMPTY) > 0 && jreg_bm_make(reg))
			{
				reg->backref = (BACK_REF *)malloc(sizeof(BACK_REF) * reg->backrefcnt);
				reg->closure = (JLOOP *)   malloc(sizeof(JLOOP)    * reg->closurecnt);
				if(reg->backref && reg->closure)
				{
					memset(reg->backref, 0, sizeof(BACK_REF) * reg->backrefcnt);
					memset(reg->closure, 0, sizeof(JLOOP)    * reg->closurecnt);
					if(!(opt & JREG_DFA) || reg->isnfaonly)
					{
						reg->compopt &= ~JREG_DFA;
						return(reg);
					}
					if(jreg_comp_dfa(reg))
					{
						reg->backrefcnt = 1;
						return(reg);
					}
				}
			}
		}
		
	}
	jreg_free(reg);
	return(null);
}

/*<
	���K�\���Ƃ̃}�b�`���O���s��
	
	//$reg    [in] ���K�\�����
	//$Parent [in] �e�̏��
	//$node   [in] �]�����鐳�K�\���m�[�h���
	//$start  [in] �]���J�n�ʒu
*/
u_int jreg_scan_node(jreg *reg, JREGPARENT *Parent, JNODE *node, u_int start)
{
	u_int len = 0;
	
	while(node)
	{
		u_int cnt = 0;

		switch(_type_get(node->type))
		{
		case JREG_EMPTY: cnt++; break;//! union�̎q�v�f�Ȃ���empty�ƂȂ�
		case JREG_STRING: case JREG_CHAR:
			{
				u_int matchcnt = start + len;
				
				if(!jreg_isstr(reg, &matchcnt, node->Data.String.str, node->Data.String.bytecnt)) {return(0);}
				cnt = matchcnt - start - len + 1;
			}
			break;
		case JREG_META:
			{
				switch(node->Data.Meta.type)
				{
				case JREGCC_CHAR:
					if(reg->reglen <= start + len) {return(0);}
					if(!_rstrcmp(reg, start + len, node->Data.Meta.metacode)) {return(0);}
					cnt = _rstrcnt(reg, start + len) + 1;
					break;
				case JREGCC_META:
					{
						u_int metacnt = start + len;
						
						if(!jreg_ismeta(reg, &metacnt, node->Data.Meta.metacode)) {return(0);}
						cnt = metacnt - start - len + 1;
					}
					break;
				case JREGCC_BACKREF:
					{
						u_int i;

						if(reg->backrefcnt <= node->Data.Meta.opt) {return(0);}
						if(reg->backref[node->Data.Meta.opt].match == 0 ||
							start + len + reg->backref[node->Data.Meta.opt].match - 1 > reg->reglen) {return(0);}
						for(i = 0; i < reg->backref[node->Data.Meta.opt].match - 1; i += _rstrcnt(reg, start + len + i))
						{
							if(_rstr2tc(reg, start + len + i) != _rstr2tc(reg, reg->backref[node->Data.Meta.opt].start + i)) {return(0);}
						}
						cnt = reg->backref[node->Data.Meta.opt].match;
					}
					break;
				case JREGCC_CTRL:
					{
						u_int metacnt = start + len;
						
						if(!jreg_ismeta_anc(reg, &metacnt, node->Data.Meta.metacode)) {return(0);}
						cnt = metacnt - start - len + 1;
					}
					break;
				default: return(0); break;
				}
			}
			break;
		case JREG_CCLASS:
			{
				u_int matchcnt = start + len;
				
				if(!jreg_iscc(reg, &matchcnt, node->Data.CClass.ccstr, node->Data.CClass.len, _type_isopt(node->type, JREG_NOT))) {return(0);}
				cnt = matchcnt - start - len + 1;
			}
			break;
		case JREG_CLOSURE:
			{
				JREGPARENT     NParent;
				JNODE_CLOSURE *closure = &node->Data.Closure;
				u_int loopcnt    = reg->closure[closure->loopno].loopcnt;
				u_int prev_start = reg->closure[closure->loopno].prev_start;
				
				NParent.node   = node;
				NParent.Parent = Parent;
				NParent.start  = start + len;

				reg->closure[closure->loopno].loopcnt    = 0;
				reg->closure[closure->loopno].prev_start = NParent.start;
				if(_type_isopt(node->type, JREG_NONGREEDY))
				{
					//! �ŒZ��v�A���[�v�񐔉�����0�ł���ΐ�Ɏ��̃m�[�h��]��
					if(closure->lower == 0 && (cnt = jreg_scan_node(reg, Parent, node->next, NParent.start)) > 0) {return(len + cnt);}
					if((cnt = jreg_scan_node(reg, &NParent, closure->closure, NParent.start)) > 0) {return(len + cnt);}
				}
				else if(_type_isopt(node->type, JREG_MOREGREEDY))
				{
					cnt = jreg_scan_node(reg, &NParent, closure->closure, NParent.start);
					if(cnt == 0 && closure->lower > 0) {return(0);}
					if(closure->lower <= reg->closure[closure->loopno].loopcnt &&
						closure->upper >= reg->closure[closure->loopno].loopcnt)
					{
						len += cnt;
						if((cnt = jreg_scan_node(reg, Parent, node->next, start + len)) > 0) {return(len + cnt);}
					}
				}
				else
				{
					//! �Œ���v�A���[�v�񐔉�����0�ł���Ό�Ŏ��̃m�[�h��]��
					if((cnt = jreg_scan_node(reg, &NParent, closure->closure, NParent.start)) > 0) {return(len + cnt);}
					if(closure->lower == 0 && (cnt = jreg_scan_node(reg, Parent, node->next, NParent.start)) > 0) {return(len + cnt);}
				}
				reg->closure[closure->loopno].loopcnt    = loopcnt;
				reg->closure[closure->loopno].prev_start = prev_start;
			}
			return(0);
			break;
		case JREG_UNION:
			{
				JREGPARENT NParent;
				u_int      prev = 0;
				
				NParent.node   = node;
				NParent.Parent = Parent;
				NParent.start  = start + len;
				if(_type_isopt(node->type, JREG_NOVLEN)) //! �߂�ǂ݂̓}�b�`�\�蕶�������߂��ĕ]��
				{
					u_int i;
					
					for(i = 1; i < node->Data.Union.unino; i++)
					{
						if(prev >= start + len) {goto __NOVLEN_NOMATCH__;}
						prev += _rstrprev(reg, start + len - prev);
					}
					NParent.start -= prev;
				}
				if((cnt = jreg_scan_node(reg, &NParent, node->Data.Union.next, NParent.start)) > 0)
				{
					if(_type_isopt(node->type, JREG_NOT)) {return(0);}
					if(_type_isopt(node->type, JREG_NOWIDTH)) {return(len + node->Data.Union.unino);}
					if(_type_isopt(node->type, JREG_NOVLEN))  {return(len + cnt - prev);}
					return(len + cnt);
				}
				if(node->Data.Union.epsilon)
				{
					if((cnt = jreg_scan_node(reg, &NParent, node->Data.Union.epsilon, NParent.start)) > 0)
					{
						if(_type_isopt(node->type, JREG_NOT)) {return(0);}
						if(_type_isopt(node->type, JREG_NOWIDTH)) {return(len + node->Data.Union.unino);}
						if(_type_isopt(node->type, JREG_NOVLEN))  {return(len + cnt - prev);}
						return(len + cnt);
					}
				}
__NOVLEN_NOMATCH__:
				if(!_type_isopt(node->type, JREG_NOT)) {return(0);}
				cnt++;
			}
			break;
		default: return(0); break;
		}
		if(cnt == 0) {return(0);}
		else
		{
			len += cnt - 1;
			if(_type_isopt(node->type, JREG_END)) {break;}
			node = node->next;
		}
	}
	if(Parent->Parent)
	{
		switch(_type_get(Parent->node->type))
		{
		case JREG_CLOSURE:
			{
				JNODE_CLOSURE *closure = &Parent->node->Data.Closure;
				u_int prev_start = reg->closure[closure->loopno].prev_start;
				u_int cnt;
				
				if(prev_start == start + len)
				{
					if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start)) > 0) {return(cnt);}
					return(0);
				}
				reg->closure[closure->loopno].loopcnt++;
				reg->closure[closure->loopno].prev_start = start + len;
				//! ���[�v�񐔂��K��񐔈ȓ��ł���ΐe�m�[�h�̎��̑J�ڐ��]���A����ȊO�͐e�̎q��]��
				if(reg->closure[closure->loopno].loopcnt <= closure->upper)
				{
					if(_type_isopt(Parent->node->type, JREG_NONGREEDY)) //! �ŒZ��v�A��ɐe�̎��m�[�h��]��
					{
						if(closure->lower <= reg->closure[closure->loopno].loopcnt)
						{
							if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start + len)) > 0) {return(len + cnt);}
						}
						if(reg->closure[closure->loopno].loopcnt < closure->upper &&
							(cnt = jreg_scan_node(reg, Parent, closure->closure, start + len)) > 0) {return(len + cnt);}
					}
					else if(_type_isopt(Parent->node->type, JREG_MOREGREEDY)) //! �����v�A��ɐe�̎q�m�[�h��]��
					{
						if(reg->closure[closure->loopno].loopcnt < closure->upper)
						{
							if((cnt = jreg_scan_node(reg, Parent, closure->closure, start + len)) > 0) {return(len + cnt);}
						}
						return(len);
					}
					else //! �Œ���v�A��ɐe�̎q�m�[�h��]��
					{
						if(reg->closure[closure->loopno].loopcnt < closure->upper &&
							(cnt = jreg_scan_node(reg, Parent, closure->closure, start + len)) > 0) {return(len + cnt);}
						if(closure->lower <= reg->closure[closure->loopno].loopcnt)
						{
							if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start + len)) > 0) {return(len + cnt);}
						}
					}
				}
				reg->closure[closure->loopno].loopcnt--;
				reg->closure[closure->loopno].prev_start = prev_start;
			}
			break;
		case JREG_UNION:
			{
				u_int cnt;
				
				if(_type_isopt(Parent->node->type, JREG_NOT)) {return(len);} //! �ے�������t���Ă���ꍇ�s��v�Ƃ���
				else if(_type_isopt(Parent->node->type, JREG_NOWIDTH)) //! ���������������t���Ă���ꍇ�}�b�`���𖳎����Ď��m�[�h�֑J�ڂ���
				{
					if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start)) > 0)
					{
						//+ ���������̒��Ń��[�v���Ă����Ƃ��̂��ߏ����ȃ}�b�`���� unino �ɕۑ�
						Parent->node->Data.Union.unino = cnt;
						return(cnt);
					}
				}
				else if(_type_isopt(Parent->node->type, JREG_BACKREF))
				{
					BACK_REF *backref = &reg->backref[Parent->node->Data.Union.unino];
					u_int starttmp = backref->start;
					u_int matchtmp = backref->match;

					backref->start = Parent->start;
					backref->match = start - Parent->start + len + 1;

					if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start + len)) > 0) {return(len + cnt);}

					backref->start = starttmp;
					backref->match = matchtmp;
				}
				else
				{
					if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start + len)) > 0) {return(len + cnt);}
				}
			}
			break;
		}
		return(0);
	}
	return(len + 1);
}

/*<
	�^����ꂽ�p�^�[���ƃ}�b�`���O���s���܂�
	
	//$reg   [in/out] ���K�\�����
	//$pat   [in]     �����Ώ�
	//$start [in]     �����J�n�ʒu
	//$len   [in]     �����̈�T�C�Y
	//$opt   [in]     ���s�I�v�V����
*/
bool jreg_exec(jreg *reg, PATTERN_STR pat, u_int start, u_int len, jreg_opt opt)
{
	JREGPARENT  Parent;
	u_int       cnt;
	t_code     *bmstr;
	
	if(!reg ||
		((_and(opt, JREG_STRINGFUNC) && !pat.PATTERN_FUNC) ||
		(_nand(opt, JREG_STRINGFUNC) && !pat.PATTERN_TCODE))) {return(false);}
	
	bmstr = null;
	if(reg->bm && !(reg->execopt & JREG_SINGLESTEP))
	{
		start += reg->bm->len - 1;
		bmstr =  (t_code*)(reg->bm + 1);
	}
	
	memset(reg->backref, 0, sizeof(BACK_REF) * reg->backrefcnt);
	Parent.node   = reg->node;
	Parent.Parent = null;
	reg->reglen   = len;
	reg->Pattern  = pat;
	reg->execopt  = opt;
	while(start <= len)
	{
		if(bmstr)
		{
			u_int offset = reg->bm->len;
			
			while(1)
			{
				w_code ch  = _tccode(bmstr + --offset);
				w_code ch2 = _rstr2tc(reg, start);
				
				if(reg->execopt & JREG_ICASE)
				{
					ch  = _tclc(ch);
					ch2 = _tclc(ch2);
				}
				if(ch != ch2)
				{
					u_int jump;
					
					if(reg->execopt & JREG_ICASE) {jump = ch2 >= BM_CHAR_LIMIT ? reg->bm->len : _min(reg->bm->skip[_tcuc(ch2)], reg->bm->skip[ch2]);}
					else                          {jump = ch2 >= BM_CHAR_LIMIT ? reg->bm->len : reg->bm->skip[ch2];}
					
					start += _max(jump, reg->bm->len - offset);
					goto __FAILED_MATCH__;
				}
				if(offset == 0) {break;}
				start--; //! �������̈׃}���`�o�C�g�����ؒf�̉\���̂����ނ��l�����Ȃ�
			}
		}
		
		if(reg->compopt & JREG_DFA) {cnt = jreg_scan_dfa(reg, start);}
		else
		{
			Parent.start = start;
			cnt = jreg_scan_node(reg, &Parent, reg->node, start);
		}
		if(cnt > 0)
		{
			reg->backref[0].start = start;
			reg->backref[0].match = cnt;
			return(true);
		}
		if(reg->execopt & JREG_SINGLESTEP) {break;}
		
		if(bmstr) {start += reg->bm->len * 2 - 1;}
		else      {start += _rstrcnt(reg, start);}
__FAILED_MATCH__:;
	}
	return(false);
}

/*<
	�^����ꂽ�p�^�[���ƃ}�b�`���O���u�������㕶������쐬���܂�

	//$reg     [in/out] ���K�\�����
	//$rep     [in/out] �u���������
	//$Pattern [in]     �����Ώ�
	//$start   [in]     �����J�n�ʒu
	//$len     [in]     �����̈�T�C�Y
	//$opt     [in]     ���s�I�v�V����
*/
bool jreg_replace(jreg *reg, jreg_rep *rep, PATTERN_STR Pattern, u_int start, u_int len, jreg_opt opt)
{
#define __rstrcpy(s,p,f,t,m,b,x) do{ \
		for(x = f; x < t; (x)++) { \
			if(s && m > b) {*((s)++) = (t_code)_rstr2ch(p, x);} \
			(b)++; \
		}}while(0)
	
	t_code *buf;
	w_code  ch;
	u_int   i, x;
	
	if(!reg || !rep || !rep->repstr ||
		((_and(opt, JREG_STRINGFUNC) && !Pattern.PATTERN_FUNC) ||
		(_nand(opt, JREG_STRINGFUNC) && !Pattern.PATTERN_TCODE))) {return(false);}
	
	if(_nand(opt, JREG_USELASTRET) && !jreg_exec(reg, Pattern, start, len, opt)) {return(false);}
	
	buf = rep->repbuf;
	rep->bufsize = 0;
	for(i = 0; _tccode(rep->repstr + i) != _T('\0');)
	{
		u_int meta_opt;
		
		switch(jreg_metax(reg, rep->repstr, &i, &ch, &meta_opt))
		{
		case JREGCC_CHAR: //# \xnn \( \] �Ȃǂ̕���
			if(buf && rep->maxsize >= rep->bufsize + _tccntb(ch)) {_tcsetc(&buf, ch);}
			rep->bufsize += _tccntb(ch);
			break;
		case JREGCC_BACKREF: //# \nn ����Q��
			if(reg->backrefcnt <= meta_opt) {return(false);}
			__rstrcpy(buf, reg,
				reg->backref[meta_opt].start,
				reg->backref[meta_opt].start + reg->backref[meta_opt].match - 1,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHALL: //# $& �}�b�`�S�Ă̏ꏊ
			__rstrcpy(buf, reg,
				reg->backref[0].start,
				reg->backref[0].start + reg->backref[0].match - 1,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHBEF: //# $` �}�b�`���������ȑO
			__rstrcpy(buf, reg,
				0, reg->backref[0].start,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHAFT: //# $' �}�b�`���������ȍ~
			__rstrcpy(buf, reg,
				reg->backref[0].start + reg->backref[0].match - 1, len,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHLAST: //# $+ �Ō�Ƀ}�b�`������ɑ���
			if(reg->backrefcnt <= 1) {return(false);}
			if(reg->backref[reg->backrefcnt - 1].match > 1)
			{
				__rstrcpy(buf, reg,
					reg->backref[reg->backrefcnt - 1].start,
					reg->backref[reg->backrefcnt - 1].start + reg->backref[reg->backrefcnt - 1].match - 1,
					rep->maxsize, rep->bufsize, x);
			}
			break;
		case JREGCC_ERROR:
		default: return(false); break;
		}
	}
	return(true);
#undef __rstrcpy
}

static void jreg_free_node(JNODE *node)
{
	if(node)
	{
		switch(_type_get(node->type))
		{
		case JREG_CLOSURE: // closure�m�[�h�͂��̃��[�v���J��
			jreg_free_node(node->Data.Closure.closure);
			break;
		case JREG_UNION: // union�m�[�h�͎q�m�[�h��epsilon�J�ڃm�[�h���J��
			jreg_free_node(node->Data.Union.next);
			jreg_free_node(node->Data.Union.epsilon);
			break;
		}
		jreg_free_node(node->next);
		free(node);
	}
}

/*<
	jreg_comp �ō쐬�������K�\������������܂�

	//$reg [in] ���K�\�����
*/
void jreg_free(jreg *reg)
{
	if(reg)
	{
		free(reg->bm);
		free(reg->regd);
		jreg_free_node(reg->node);
		
		free(reg->regstr);
		free(reg->backref);
		free(reg->closure);
		free(reg);
	}
}

/*<
	NFA����DFA�ւ̕ϊ�
	//! ���̑���͔j��I
*/
static bool jreg_comp_dfa(jreg *reg)
{
	u_int i;
	
	if(!reg || !(reg->node) || !(reg->compopt & JREG_DFA)) {return(false);}
	
	reg->nodemax = 1; //! �e�[�u����0�Ԗڂ͏�����ԂƂ��Ĉ���
	if(!jreg_node_split(reg->node, &(reg->nodemax), &(reg->node))) {return(false);}
	if(!(reg->regd = (JNODE *)malloc(_dfa_sizeof(reg->nodemax)))) {return(false);}
	
	memset(reg->regd, 0, _dfa_sizeof(reg->nodemax));
	for(i = 0; i < reg->nodemax; i++) {_dfa_index(reg->regd, reg->nodemax, i)->type = JREG_EMPTY;}
	if(!(jreg_dfa_buff(reg, 0, null, reg->node))) {return(false);}
	return(true);
}

/*<
	�m�[�h�̕���
*/
static bool jreg_node_split(JNODE *node, u_int *cnt, JNODE **prnext)
{
	JNODE *next;
	
	if(!cnt || !prnext) {return(false);}
	if(!node)
	{
		*prnext = null;
		return(true);
	}
	
	*prnext = node;
	switch(_type_get(node->type))
	{
	case JREG_EMPTY: return(true); break;
	case JREG_CHAR: case JREG_STRING:
		if(node->Data.String.wordcnt > 1)
		{
			JNODE  *next = node->next;
			t_code *str  = str = node->Data.String.str;
			u_int   i;
			
			for(i = node->Data.String.wordcnt; i > 1; i--)
			{
				node->selnum = (*cnt)++;
				node->Data.String.wordcnt = 1;
				node->Data.String.bytecnt = _tccnt(str);
				str += node->Data.String.bytecnt;
				
				if(!(node = jreg_new_node(node, JREG_CHAR))) {return(false);}
				
				node->Data.Char.str = str;
				node->next = next; //+ ���̗v�f�m�ۂɎ��s���Ă�����͑S�Ă���邽�߂ɒǉ�
			}
			node->Data.String.wordcnt = 1;
			node->Data.String.bytecnt = _tccnt(str);
		}
		_type_set(node->type, JREG_CHAR);
		node->selnum = (*cnt)++;
		break;
	case JREG_CLOSURE: //! �J��Ԃ��� {1} {0,} {1,} �̂����ꂩ�ɂȂ�܂ŕ���
		if((node->Data.Closure.lower <= 1 && (node->Data.Closure.upper == JREG_REPEAT_INFINITE || node->Data.Closure.upper == 1)) ||
			(node->Data.Closure.lower == 1 && node->Data.Closure.upper == 1)) //+ {0} �͏������Ȃ�
		{
			if(!jreg_node_split(node->Data.Closure.closure, cnt, &(node->Data.Closure.closure))) {return(false);}
		}
		else if(node->Data.Closure.upper > 0) //! {n,} {n,m} {0,m} �𕪉� (n > 1)
		{
			if(node->Data.Closure.lower == 0)
			{
				if(!(*prnext = jreg_new_node(null, JREG_EMPTY))) {return(false);}
				(*prnext)->type = JREG_CLOSURE;
				(*prnext)->Data.Closure.lower = 0;
				(*prnext)->Data.Closure.upper = 1;
				if(!jreg_node_copy(node->Data.Closure.closure, &((*prnext)->Data.Closure.closure))) {return(false);}
				node->Data.Closure.upper--;
				(*prnext)->next = node->next;
				node->next = null;
				(*prnext)->Data.Closure.closure->next = node;
				return(jreg_node_split(*prnext, cnt, prnext));
			}
			else
			{
				if(!jreg_node_copy(node->Data.Closure.closure, prnext)) {return(false);}
				(*prnext)->next = node;
				node->Data.Closure.lower--;
				if(node->Data.Closure.upper != JREG_REPEAT_INFINITE) {node->Data.Closure.upper--;}
				return(jreg_node_split(*prnext, cnt, prnext));
			}
		}
		break;
	case JREG_UNION:
		if(_type_isopt(node->type, JREG_NOT) ||
			_type_isopt(node->type, JREG_NOWIDTH) ||
			_type_isopt(node->type, JREG_NOVLEN)) {node->selnum = (*cnt)++;}
		else
		{
			if(!jreg_node_split(node->Data.Union.next,    cnt, &(node->Data.Union.next)))    {return(false);}
			if(!jreg_node_split(node->Data.Union.epsilon, cnt, &(node->Data.Union.epsilon))) {return(false);}
		}
		break;
	case JREG_META:
		if(node->Data.Meta.type == JREGCC_BACKREF) {return(false);}
		node->selnum = (*cnt)++;
		if(node->Data.Meta.type == JREGCC_META &&
			(node->Data.Meta.metacode == _T('n') || node->Data.Meta.metacode == _T('N'))) {(*cnt) += 2;}
		break;
	case JREG_CCLASS: node->selnum = (*cnt)++; break;
	default: return(false); break;
	}
	if(!jreg_node_split(node->next, cnt, &next)) {return(false);}
	node->next = next;
	return(true);
}

/*<
	�m�[�h�̃R�s�[
*/
static bool jreg_node_copy(JNODE *orig_node, JNODE **prnext)
{
	JNODE *node;
	
	if(!prnext) {return(false);}
	if(!orig_node)
	{
		*prnext = null;
		return(true);
	}
	
	if(!(node = jreg_new_node(null, JREG_EMPTY))) {return(false);}
	*node = *orig_node;
	*prnext = node;
	switch(_type_get(node->type))
	{
	case JREG_CLOSURE:
		{
			JNODE *closure = node->Data.Closure.closure;
			
			node->Data.Closure.closure = null;
			if(!jreg_node_copy(closure, &(node->Data.Closure.closure))) {return(false);}
		}
		break;
	case JREG_UNION:
		{
			JNODE *next    = node->Data.Union.next;
			JNODE *epsilon = node->Data.Union.epsilon;
			
			node->Data.Union.next    = null;
			node->Data.Union.epsilon = null;
			if(!jreg_node_copy(next,    &(node->Data.Union.next)))    {return(false);}
			if(!jreg_node_copy(epsilon, &(node->Data.Union.epsilon))) {return(false);}
		}
		break;
	}
	return(jreg_node_copy(node->next, &(node->next)));
}

/*<
	�m�[�h��DFA�e�[�u���̊֘A�t��
*/
static bool jreg_dfa_buff(jreg *reg, u_int idx, JREGPARENT *Parent, JNODE *node)
{
	if(!reg || !(reg->regd))  {return(false);}

	if(!node)
	{
		if(Parent)
		{
			if(_type_istype(Parent->node->type, JREG_CLOSURE) && Parent->node->Data.Closure.upper == JREG_REPEAT_INFINITE)
			{
				if(!jreg_dfa_buff(reg, idx, Parent, Parent->node->Data.Closure.closure)) {return(false);}
			}
			return(jreg_dfa_buff(reg, idx, Parent->Parent, Parent->node->next));
		}
		_type_optadd(_dfa_index(reg->regd, reg->nodemax, idx)->type, JREG_END);
		return(true);
	}

	//! �֐��}�N��������return����\��������
#define DFA_ADDBUF(r,x,n,f) \
	do{ \
		if((r)->nodemax < x) {return(false);} \
		_dfa_set((r)->regd, (r)->nodemax, x, (n)->selnum - 1); \
		if(!(f) && !_type_istype(_dfa_index((r)->regd, (r)->nodemax, (n)->selnum)->type, JREG_EMPTY)) {return(true);} \
		*_dfa_index((r)->regd, (r)->nodemax, (n)->selnum) = *(n); \
		(x) = (n)->selnum; \
	}while(0)

	_type_optrem(node->type, JREG_END);
	switch(_type_get(node->type))
	{
	case JREG_EMPTY:
		return(jreg_dfa_buff(reg, idx, Parent->Parent, Parent->node->next));
		break;
	case JREG_META: //! �Ǝ��g�� \N �� (\r((?!\n)|\n)|\n) �����ɂȂ蕪��
		if(node->Data.Meta.type == JREGCC_BACKREF) {return(false);}
		if(node->Data.Meta.type == JREGCC_META &&
			(node->Data.Meta.metacode == _T('n') || node->Data.Meta.metacode == _T('N')))
		{
			JNODE tmp;
			u_int no = idx;
			
			tmp.type               = JREG_META;
			tmp.Data.Meta.type     = JREGCC_CHAR;
			tmp.Data.Meta.metacode = _T('\r');
			tmp.selnum             = node->selnum;
			DFA_ADDBUF(reg, idx, &tmp, false);
			
			tmp.Data.Meta.type     = JREGCC_CTRL;
			tmp.Data.Meta.metacode = _T('N');
			tmp.selnum             = node->selnum + 1;
			DFA_ADDBUF(reg, idx, &tmp, false);
			if(!jreg_dfa_buff(reg, idx, Parent, node->next)) {return(false);}
			
			tmp.Data.Meta.type     = JREGCC_CHAR;
			tmp.Data.Meta.metacode = _T('\n');
			tmp.selnum             = node->selnum + 2;
			idx                    = node->selnum;
			DFA_ADDBUF(reg, idx, &tmp, true);
			if(!jreg_dfa_buff(reg, idx, Parent, node->next)) {return(false);}

			idx = no;
			DFA_ADDBUF(reg, idx, &tmp, true);
			return(jreg_dfa_buff(reg, idx, Parent, node->next));
		}
	case JREG_STRING: case JREG_CHAR: case JREG_CCLASS:
		DFA_ADDBUF(reg, idx, node, false);
		return(jreg_dfa_buff(reg, idx, Parent, node->next));
		break;
	case JREG_CLOSURE:
		if(node->Data.Closure.lower > 0 || node->Data.Closure.upper > 0)
		{
			JREGPARENT NParent;
			
			NParent.node   = node;
			NParent.Parent = Parent;
			if(!jreg_dfa_buff(reg, idx, &NParent, node->Data.Closure.closure)) {return(false);}
		}
		if(node->Data.Closure.lower == 0) {return(jreg_dfa_buff(reg, idx, Parent, node->next));}
		return(true);
		break;
	case JREG_UNION:
		{
			JREGPARENT NParent;
			
			if(_type_isopt(node->type, JREG_NOT) ||
				_type_isopt(node->type, JREG_NOWIDTH) ||
				_type_isopt(node->type, JREG_NOVLEN))
			{
				DFA_ADDBUF(reg, idx, node, false);
				_dfa_index(reg->regd, reg->nodemax, idx)->next = null;
				return(jreg_dfa_buff(reg, idx, Parent,  node->next));
			}
			
			NParent.node   = node;
			NParent.Parent = Parent;
			if(!jreg_dfa_buff(reg, idx, &NParent, node->Data.Union.next)) {return(false);}
			if(node->Data.Union.epsilon) {return(jreg_dfa_buff(reg, idx, &NParent, node->Data.Union.epsilon));}
			return(true);
		}
		break;
	default: return(false); break;
	}
	return(false);
#undef DFA_ADDBUF
}

/*<
	DFA�̑J�ڃe�[�u����萳�K�\���Ƃ̃}�b�`���O���s��
*/
static u_int jreg_scan_dfa(jreg *reg, u_int start)
{
	byte  *tbl, *cur, *nwb, *aldy;
	u_int  i, x, match, len;
	
	tbl  = _dfa_tblar(reg->regd, reg->nodemax, 0); //# ���݈ʒu�e�[�u��
	cur  = _dfa_tblar(reg->regd, reg->nodemax, 1); //# ���J�ڈʒu�e�[�u��
	nwb  = _dfa_tblar(reg->regd, reg->nodemax, 2); //# 0����v�J�ڃe�[�u��
	aldy = _dfa_tblar(reg->regd, reg->nodemax, 3); //# �J�ڗ����e�[�u��
	_dfa_clear(tbl, reg->nodemax, x); //! �e�[�u����0�Ԃ�������ԂƂ���
	for(i = 0; i < reg->nodemax; i++) {if(_dfa_fetch(reg->regd, reg->nodemax, 0, i)) {_bit_set(tbl, i);}}
	
	len   = 0;
	match = 0;
	if(_type_isopt(_dfa_index(reg->regd, reg->nodemax, 0)->type, JREG_END)) {match = 1;}
	_dfa_clear(cur,  reg->nodemax, x);
	_dfa_clear(aldy, reg->nodemax, x);
	while(1)
	{
		bool isbit, isend;
		
		_dfa_istbl(tbl, reg->nodemax, isbit, x);
		if(!isbit) {break;}
		
		isend = false;
		_dfa_clear(nwb, reg->nodemax, x);
		for(i = 0; i < reg->nodemax - 1; i++)
		{
			JNODE *node;
			bool   ret;
			
			if(_bit_fetch(tbl, i) == 0x00) {continue;}
			ret  = false;
			node = _dfa_index(reg->regd, reg->nodemax, i + 1);
			switch(_type_get(node->type))
			{
			case JREG_STRING: case JREG_CHAR:
				{
					u_int matchcnt = start + len;
					
					if(jreg_isstr(reg, &matchcnt,
						node->Data.String.str,
						node->Data.String.bytecnt)) {ret = true;}
				}
				break;
			case JREG_META:
				switch(node->Data.Meta.type)
				{
				case JREGCC_CHAR:
					if(node->Data.Meta.metacode == _rstr2tc(reg, start + len)) {ret = true;}
					break;
				case JREGCC_META:
					{
						u_int matchcnt = start + len;
						
						if(jreg_ismeta(reg, &matchcnt, node->Data.Meta.metacode)) {ret = true;}
					}
					break;
				case JREGCC_CTRL:
					{
						u_int matchcnt = start + len;
						
						if(jreg_ismeta_anc(reg, &matchcnt, node->Data.Meta.metacode))
						{
							_dfa_or(nwb, node + 1, reg->nodemax, x);
							if(_type_isopt(node->type, JREG_END)) {match = len + 1;}
						}
					}
					break;
				default: return(0); break;
				}
				break;
			case JREG_CCLASS:
				{
					u_int matchcnt = start + len;
					
					if(jreg_iscc(reg, &matchcnt,
						node->Data.CClass.ccstr,
						node->Data.CClass.len,
						_type_isopt(node->type, JREG_NOT))) {ret = true;}
				}
				break;
			case JREG_UNION:
				{
					JREGPARENT Parent;
					
					Parent.node   = node;
					Parent.Parent = null;
					if(jreg_scan_node(reg, &Parent, node, start + len) > 0)
					{
						_dfa_or(nwb, node + 1, reg->nodemax, x);
						if(_type_isopt(node->type, JREG_END)) {match = len + 1;}
					}
				}
				break;
			default: return(0); break;
			}
			if(ret)
			{
				_dfa_or(cur, node + 1, reg->nodemax, x);
				if(_type_isopt(node->type, JREG_END)) {isend = true;}
			}
		}
		_dfa_nand(nwb, aldy, reg->nodemax, x);
		_dfa_istbl(nwb, reg->nodemax, isbit, x);
		if(isbit)
		{
			_dfa_or(aldy, nwb,  reg->nodemax, x);
			_dfa_eq(tbl,  nwb,  reg->nodemax, x);
			continue;
		}
		len += _rstrcnt(reg, start + len);
		if(isend) {match = len + 1;}
		_dfa_eq(tbl, cur, reg->nodemax, x);
		_dfa_clear(cur,  reg->nodemax, x);
		_dfa_clear(aldy, reg->nodemax, x);
	}
	return(match);
}
