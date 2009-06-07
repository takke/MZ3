#include "jectlib.h"
#include "JChar.h"
#include "JRegex.h"
#include <string.h>
#include <ctype.h>


#define JREG_REPEAT_INFINITE _limitu(u_int) // ループ回数上限なし

	//< 検索対象から文字を取得
#ifdef _JREG_SMARTCODE
# define _rstr2ch(p,n) ((w_code)(p)->Pattern.PATTERN_TCODE[n])
#else
# define _rstr2ch(p,n) \
	((w_code)(((p)->execopt & JREG_STRINGFUNC) ? \
		(p)->Pattern.PATTERN_FUNC(n, (p)->usrval) \
		: (p)->Pattern.PATTERN_TCODE[n]))
#endif // _JREG_SMARTCODE

#ifdef UNICODE
# define _rstrcnt(p,n)  1                 //< 検索対象の文字が何バイトか取得
# define _rstr2tc(p,n)  _rstr2ch(p,n)     //< 検索対象から1文字を取得
# define _rstrprev(p,n) ((n) > 0 ? 1 : 0) //< 検索対象から前方の文字を取得
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

	//< 検索対象の文字と比較
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

	//- 正規表現ノードタイプ
typedef enum _JREG_TYPE
{
	JREG_EMPTY      = 0x0000, // 空ノードを表す
	
	JREG_NODETYPE   = 0x000F, // ノード分類のマスクパターン
	JREG_CHAR       = 0x0001, // 拡張不可文字列
	JREG_STRING     = 0x0002, // 拡張可文字列
	JREG_META       = 0x0003, // メタ文字   (\w \d \s \x81 \z など)
	JREG_CCLASS     = 0x0004, // 文字クラス ([...] に相当)
	JREG_CLOSURE    = 0x0005, // 反復子     (* + ? {n} など)
	JREG_UNION      = 0x0006, // 閉包       ((...) に相当)
	
	JREG_OPTIONTYPE = 0xFFF0, // ノードオプションのマスクパターン
	JREG_HEAD       = 0x0010, // 閉包の先頭 (拡張正規表現の判別)
	JREG_NONGREEDY  = 0x0020, // 最短一致   (反復子ノードに有効)
	JREG_MOREGREEDY = 0x0040, // 限定一致   (反復子ノードに有効)
	JREG_NOT        = 0x0080, // 否定一致   ([^...] (?!...) など)
	JREG_NOWIDTH    = 0x0100, // 先読み     ((?=...) など)
	JREG_BACKREF    = 0x0200, // 後方参照有 (閉包ノードに有効)
	JREG_NOVLEN     = 0x0400, // 戻り読み   ((?<=...) など)
	JREG_END        = 0x0800, // ノード終端
}JREG_TYPE;

	//- キャラクタコード定数
typedef enum _JREGCC
{
	JREGCC_CHAR,      // 文字表現
	JREGCC_META,      // メタ文字表現
	JREGCC_RANGE,     // 文字範囲を表す `-'
	JREGCC_BACKREF,   // 後方参照
	JREGCC_CTRL,      // アンカー
	JREGCC_PROC,      // 名前付文字クラス
	JREGCC_MATCHALL,  // マッチ位置全体
	JREGCC_MATCHBEF,  // マッチ位置前方
	JREGCC_MATCHAFT,  // マッチ位置後方
	JREGCC_MATCHLAST, // 最終閉包マッチ位置
	JREGCC_ERROR,     // エラー
}JREGCC;

typedef struct _JNODE_STRING
{
	t_code *str;     // 文字列先頭 (jreg.regstrの文字列中を指す)
	u_int   wordcnt; // 文字数
	u_int   bytecnt; // バイト数
}JNODE_STRING;

typedef struct _JNODE_META
{
	JREGCC type;     // メタ文字種
	w_code metacode; // メタ文字識別タイプ
	u_int  opt;      // オプション値 (メタ文字種により利用方法が異なる)
}JNODE_META;

typedef struct _JNODE_CCLASS
{
	t_code *ccstr; // 文字列先頭 (jreg.regstrの文字列中を指す)
	u_int   len;   // 文字数
}JNODE_CCLASS;

typedef struct _JNODE_CLOSURE
{
	u_int  lower;   // ループ回数下限
	u_int  upper;   // ループ回数上限
	JNODE *closure; // ループノード
	u_int  loopno;  // ループ回数参照位置
}JNODE_CLOSURE;

typedef struct _JNODE_UNION
{
	JNODE *next;    // 子ノード
	JNODE *epsilon; // 空遷移先ノード
	u_int  unino;   // マッチ位置参照位置
}JNODE_UNION;

#define BM_CHAR_LIMIT 127
#define BM_LEN_MIN    2
#define BM_LEN_MAX    64
typedef u_char bmskip;

struct _JNODE
{
	JREG_TYPE type; // ノード種類
	union
	{
		JNODE_STRING   String;  // 文字列
		JNODE_STRING   Char;    // 文字 (Stringと同義)
		JNODE_META     Meta;    // メタ文字
		JNODE_CCLASS   CClass;  // 文字クラス
		JNODE_CLOSURE  Closure; // 反復
		JNODE_UNION    Union;   // 閉包
	}Data;
	JNODE *next; // マッチした際の遷移ノード
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

	//- DFAテーブル補助
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

	//- 親状態構造体定義
typedef struct _JREGPARENT
{
	JNODE *node;                // 親ノード自身
	struct _JREGPARENT *Parent; // 親状態情報
	u_int  start;               // 最初の子ノードのマッチ開始位置
}JREGPARENT;

	//- 名前付文字クラスタイプ
typedef enum _JREGISCC
{
	JREGISCC_EMPTY      = 0x0000,// 未定義

	JREGISCC_TYPE       = 0x00FF, // タイプ分類のマスクパターン
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

	JREGISCC_ERROR      = 0x0200, // エラー
}JREGISCC;

	//- 名前付文字クラス情報
typedef struct _JREGCCNAME
{
	t_code   *ccstr; // [:xxx:] の xxx の部分を指定
	JREGISCC  type;  // タイプ
}JREGCCNAME;

	//- 名前付文字クラス
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
	case JREG_CLOSURE: //! closureノードは直前ノードを取り込む
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
	case JREG_STRING: //! stringノードは最後の1文字を切り出す
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
	正規表現ノードがマッチした際の長さを取得する
	
	//$node [in] 正規表現ノード情報
	//$type [in] 正規表現ノードタイプ
	
	//! 戻り読みの正しいコンパイルに必要
*/
static u_int jreg_get_cnt(JNODE *node, JREG_TYPE type)
{
	u_int cnt;

	if(!node) {return(1);}
	
	if((cnt = jreg_get_cnt(node->next, type)) == 0) {return(0);}
	cnt--;
	switch(_type_get(node->type))
	{
	case JREG_STRING: case JREG_CHAR: //! string/charノードは文字数を返す
		return(cnt + node->Data.String.wordcnt + 1);
		break;
	case JREG_CCLASS: //! cclassノードは文字幅1
		return(cnt + 2);
		break;
	case JREG_META:
		switch(node->Data.Meta.type)
		{
		case JREGCC_CHAR: case JREGCC_META: case JREGCC_PROC: // 文字、メタ文字、名前付文字クラスは文字幅1
			return(cnt + 2);
			break;
		case JREGCC_CTRL:// アンカーは文字幅0
			return(cnt + 1);
			break;
		case JREGCC_BACKREF: //! 後方参照は戻り読み内部ではエラー
			if(_type_isopt(type, JREG_NOVLEN)) {return(0);}
			return(cnt + 1);
			break;
		default: return(0); break;
		}
		break;
	case JREG_UNION: //! unionノードは戻り読みの際 (p1|p2) の左右の部分の長さが一緒でないとエラー
		{
			u_int lcnt, rcnt;

			_type_optadd(type, node->type & JREG_NOVLEN);
			if((lcnt = jreg_get_cnt(node->Data.Union.next,    type)) == 0) {return(0);}
			if((rcnt = jreg_get_cnt(node->Data.Union.epsilon, type)) == 0) {return(0);}
			if(_type_isopt(type, JREG_NOVLEN))
			{
				if(node->Data.Union.epsilon && lcnt != rcnt) {return(0);}
				node->Data.Union.unino = lcnt; //! 戻り読みノードはあらかじめマッチ幅をセットしておく
			}
			return(cnt + lcnt);
		}
		break;
	case JREG_CLOSURE: //! closureノードは戻り読み内部での回数不定の場合エラー
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
	case _T('M'): //# M-x 形式のコードを認識
		if(_tccode(regstr + *cnt) != _T('-')) {return(JREGCC_ERROR);}
		*cnt += _tccnt(regstr + *cnt);
		if(jreg_meta(reg, regstr, cnt, ch, null, false) != JREGCC_CHAR) {return(JREGCC_ERROR);}
		*ch = (w_code)((*ch & 0xFF) | 0x80);
		return(JREGCC_CHAR);
		break;
	case _T('C'): //# C-x 形式のコードを認識 (実際の解析部分は cx の部分に依存)
		if(_tccode(regstr + *cnt) != _T('-')) {return(JREGCC_ERROR);}
		*cnt += _tccnt(regstr + *cnt);
	case _T('c'): //# cx 形式のコードを認識
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
		if(!ismeta) {return(JREGCC_CHAR);} //+ 文字クラス内での . は . そのものを表す
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
		case _T('n'): //# 独自拡張、JREG_BINARYMODEでなければ \n と \N は同義
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
	if(*ch != _T('$')) {return(jreg_meta(reg, regstr, cnt, ch, opt, false));} //! $ で始まるメタ文字以外は jreg_meta でパース
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
	case _T('^'): // 行頭または改行の直後、ただし \r と \n の間にはマッチしない
		if(*cnt == 0) {ret = true;}
		else if(reg->compopt & JREG_STARTLINE && prevtype & tc_crlf)
		{
			if(_nand(reg->compopt, JREG_BINARYMODE) && ((prevtype == tc_cr && type==tc_lf) || *cnt == reg->reglen)) {break;}
			ret = true;
		}
		break;
	case _T('$'): // 行末または改行の直前、ただし \r と \n の間にはマッチしない
		if(*cnt == reg->reglen)
		{
			//! バッファ末であっても直前が改行であればマッチしない
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
	
		//! DFA用メタ文字
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
		case JREGCC_CHAR: //# 文字-文字 の順に並んでいれば範囲を表す
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
				switch(ccopt & JREGISCC_TYPE) // _istxxx利用 (ロケール反映)
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
						
						switch(ccopt & JREGISCC_TYPE) // 独自対応 (ロケール無視)
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
	正規表現の括弧の部分をパースする
	
	//$reg      [in]     正規表現情報
	//$par_node [in/out] 直前の正規表現ノード情報
	//$endch    [in]     終端文字
	//$cnt      [in/out] パース位置
	//$type     [in]     親から引き継いだ正規表現タイプ
	
	//# 拡張正規表現と選択( | )のみをパースしその他は jreg_parse に委ねる
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
			case _T(':'): break; // (?:) 後方参照なし
			case _T('='): // (?=) 先読み
				_type_optadd(type, JREG_NOWIDTH);
				_type_optadd(par_node->type, JREG_NOWIDTH);
				break;
			case _T('!'): // (?!) 先読み否定
				_type_optadd(type, JREG_NOWIDTH);
				_type_optadd(par_node->type, JREG_NOWIDTH | JREG_NOT);
				break;
			case _T('<'):
				ch   =  _tccode(reg->regstr + *cnt);
				*cnt += _tccnt(reg->regstr + *cnt);
				switch(ch)
				{
				case _T('='): // (?<=) 戻り読み
					_type_optadd(type, JREG_NOVLEN);
					_type_optadd(par_node->type, JREG_NOVLEN);
					goto __UNIONEX_END__;
					break;
				case _T('!'): // (?<!) 戻り読み否定
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
	if(_T('|') == _tccode(reg->regstr + *cnt - _tcprev(reg->regstr, *cnt))) //! 終了文字が | であればepsilon遷移を追加する
	{
		par_node->Data.Union.epsilon = jreg_new_node(null, JREG_UNION);
		
		if(!par_node->Data.Union.next) {return(false);}
		
		return(jreg_parse_union(reg, par_node->Data.Union.epsilon, endch, cnt, type));
	}
	return(true);
}

/*<
	正規表現の括弧以外の部分をパースする
	
	//$reg      [in]     正規表現情報
	//$par_node [in/out] 直前の正規表現ノード情報
	//$endch    [in]     終端文字
	//$cnt      [in/out] パース位置
	//$type     [in]     親から引き継いだ正規表現タイプ
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
	case _T('|'): //# | があればパースを正常終了 (jreg_parse_union で続くかの判断が行われる)
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
			case _T('}'): //# {n} の解釈
				node->Data.Closure.upper = loopnum;
				return(jreg_parse(reg, node, endch, cnt, type));
				break;
			case _T(','): //# {n,} または {n,m} の解釈
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
				//! 戻り読みの中ではループ回数不定をエラーとする
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
			
			//+ 戻り読み中の {n}+ は意味はないが、ループだからといって弾いてはいけない
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
				//! 直前のclosureノードを限定一致ループにする
				if(!_type_isopt(type, JREG_NOVLEN | JREG_NOWIDTH)) {reg->isnfaonly = true;}
				_type_optadd(par_node->type, JREG_MOREGREEDY);
				return(jreg_parse(reg, par_node, endch, cnt, type));
			}
		}
		break;
	case _T('?'): //# closureノード開始
		{
			JNODE *node = jreg_new_node(par_node, JREG_CLOSURE);
			
			//+ 戻り読み中の {n}? は意味はないが、ループだからといって弾いてはいけない
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
				//! 直前のclosureノードを最短一致ループにする
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
				//! 独自拡張 \N は (\r\n|\r|\n) 相当になりマッチ幅不定なので、戻り読みの中でだけ \n(\x0A) に変換
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
	先頭からの確定文字列長取得
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
	BM法テーブル作成
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
	正規表現のコンパイルを行います
	検索/置き換えを実行するときこの関数の戻り値が必須となります
	
	//$str [in] 正規表現パターン
	//$opt [in] コンパイルオプション
*/
jreg* jreg_comp(const t_code *str, jreg_opt opt)
{
	jreg  *reg;
	u_int  cnt;
	
	if(!(reg = (jreg *)malloc(sizeof(jreg)))) {return(null);}
	
	reg->regstr    = (t_code *)_tcsdup((const TCHAR *)str);
	reg->compopt   = opt;
	reg->node      = jreg_new_node(null, JREG_UNION); //! unionノードを作成しルートノードにする
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
	正規表現とのマッチングを行う
	
	//$reg    [in] 正規表現情報
	//$Parent [in] 親の状態
	//$node   [in] 評価する正規表現ノード情報
	//$start  [in] 評価開始位置
*/
u_int jreg_scan_node(jreg *reg, JREGPARENT *Parent, JNODE *node, u_int start)
{
	u_int len = 0;
	
	while(node)
	{
		u_int cnt = 0;

		switch(_type_get(node->type))
		{
		case JREG_EMPTY: cnt++; break;//! unionの子要素なしはemptyとなる
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
					//! 最短一致、ループ回数下限が0であれば先に次のノードを評価
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
					//! 最長一致、ループ回数下限が0であれば後で次のノードを評価
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
				if(_type_isopt(node->type, JREG_NOVLEN)) //! 戻り読みはマッチ予定文字数分戻して評価
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
				//! ループ回数が規定回数以内であれば親ノードの次の遷移先を評価、それ以外は親の子を評価
				if(reg->closure[closure->loopno].loopcnt <= closure->upper)
				{
					if(_type_isopt(Parent->node->type, JREG_NONGREEDY)) //! 最短一致、先に親の次ノードを評価
					{
						if(closure->lower <= reg->closure[closure->loopno].loopcnt)
						{
							if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start + len)) > 0) {return(len + cnt);}
						}
						if(reg->closure[closure->loopno].loopcnt < closure->upper &&
							(cnt = jreg_scan_node(reg, Parent, closure->closure, start + len)) > 0) {return(len + cnt);}
					}
					else if(_type_isopt(Parent->node->type, JREG_MOREGREEDY)) //! 限定一致、常に親の子ノードを評価
					{
						if(reg->closure[closure->loopno].loopcnt < closure->upper)
						{
							if((cnt = jreg_scan_node(reg, Parent, closure->closure, start + len)) > 0) {return(len + cnt);}
						}
						return(len);
					}
					else //! 最長一致、先に親の子ノードを評価
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
				
				if(_type_isopt(Parent->node->type, JREG_NOT)) {return(len);} //! 否定条件が付いている場合不一致とする
				else if(_type_isopt(Parent->node->type, JREG_NOWIDTH)) //! 長さ無視条件が付いている場合マッチ幅を無視して次ノードへ遷移する
				{
					if((cnt = jreg_scan_node(reg, Parent->Parent, Parent->node->next, start)) > 0)
					{
						//+ 長さ無視の中でループしていたときのため純粋なマッチ幅を unino に保存
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
	与えられたパターンとマッチングを行います
	
	//$reg   [in/out] 正規表現情報
	//$pat   [in]     検索対象
	//$start [in]     検索開始位置
	//$len   [in]     検索領域サイズ
	//$opt   [in]     実行オプション
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
				start--; //! 高速化の為マルチバイト文字切断の可能性のある後退を考慮しない
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
	与えられたパターンとマッチングし置き換え後文字列を作成します

	//$reg     [in/out] 正規表現情報
	//$rep     [in/out] 置き換え情報
	//$Pattern [in]     検索対象
	//$start   [in]     検索開始位置
	//$len     [in]     検索領域サイズ
	//$opt     [in]     実行オプション
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
		case JREGCC_CHAR: //# \xnn \( \] などの文字
			if(buf && rep->maxsize >= rep->bufsize + _tccntb(ch)) {_tcsetc(&buf, ch);}
			rep->bufsize += _tccntb(ch);
			break;
		case JREGCC_BACKREF: //# \nn 後方参照
			if(reg->backrefcnt <= meta_opt) {return(false);}
			__rstrcpy(buf, reg,
				reg->backref[meta_opt].start,
				reg->backref[meta_opt].start + reg->backref[meta_opt].match - 1,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHALL: //# $& マッチ全ての場所
			__rstrcpy(buf, reg,
				reg->backref[0].start,
				reg->backref[0].start + reg->backref[0].match - 1,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHBEF: //# $` マッチした部分以前
			__rstrcpy(buf, reg,
				0, reg->backref[0].start,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHAFT: //# $' マッチした部分以降
			__rstrcpy(buf, reg,
				reg->backref[0].start + reg->backref[0].match - 1, len,
				rep->maxsize, rep->bufsize, x);
			break;
		case JREGCC_MATCHLAST: //# $+ 最後にマッチした閉包に相当
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
		case JREG_CLOSURE: // closureノードはそのループを開放
			jreg_free_node(node->Data.Closure.closure);
			break;
		case JREG_UNION: // unionノードは子ノードとepsilon遷移ノードを開放
			jreg_free_node(node->Data.Union.next);
			jreg_free_node(node->Data.Union.epsilon);
			break;
		}
		jreg_free_node(node->next);
		free(node);
	}
}

/*<
	jreg_comp で作成した正規表現情報を解放します

	//$reg [in] 正規表現情報
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
	NFAからDFAへの変換
	//! この操作は破壊的
*/
static bool jreg_comp_dfa(jreg *reg)
{
	u_int i;
	
	if(!reg || !(reg->node) || !(reg->compopt & JREG_DFA)) {return(false);}
	
	reg->nodemax = 1; //! テーブルの0番目は初期状態として扱う
	if(!jreg_node_split(reg->node, &(reg->nodemax), &(reg->node))) {return(false);}
	if(!(reg->regd = (JNODE *)malloc(_dfa_sizeof(reg->nodemax)))) {return(false);}
	
	memset(reg->regd, 0, _dfa_sizeof(reg->nodemax));
	for(i = 0; i < reg->nodemax; i++) {_dfa_index(reg->regd, reg->nodemax, i)->type = JREG_EMPTY;}
	if(!(jreg_dfa_buff(reg, 0, null, reg->node))) {return(false);}
	return(true);
}

/*<
	ノードの分解
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
				node->next = next; //+ 次の要素確保に失敗しても解放は全てされるために追加
			}
			node->Data.String.wordcnt = 1;
			node->Data.String.bytecnt = _tccnt(str);
		}
		_type_set(node->type, JREG_CHAR);
		node->selnum = (*cnt)++;
		break;
	case JREG_CLOSURE: //! 繰り返しを {1} {0,} {1,} のいずれかになるまで分解
		if((node->Data.Closure.lower <= 1 && (node->Data.Closure.upper == JREG_REPEAT_INFINITE || node->Data.Closure.upper == 1)) ||
			(node->Data.Closure.lower == 1 && node->Data.Closure.upper == 1)) //+ {0} は処理しない
		{
			if(!jreg_node_split(node->Data.Closure.closure, cnt, &(node->Data.Closure.closure))) {return(false);}
		}
		else if(node->Data.Closure.upper > 0) //! {n,} {n,m} {0,m} を分解 (n > 1)
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
	ノードのコピー
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
	ノード対DFAテーブルの関連付け
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

	//! 関数マクロ内部でreturnする可能性がある
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
	case JREG_META: //! 独自拡張 \N は (\r((?!\n)|\n)|\n) 相当になり分解
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
	DFAの遷移テーブルより正規表現とのマッチングを行う
*/
static u_int jreg_scan_dfa(jreg *reg, u_int start)
{
	byte  *tbl, *cur, *nwb, *aldy;
	u_int  i, x, match, len;
	
	tbl  = _dfa_tblar(reg->regd, reg->nodemax, 0); //# 現在位置テーブル
	cur  = _dfa_tblar(reg->regd, reg->nodemax, 1); //# 次遷移位置テーブル
	nwb  = _dfa_tblar(reg->regd, reg->nodemax, 2); //# 0幅一致遷移テーブル
	aldy = _dfa_tblar(reg->regd, reg->nodemax, 3); //# 遷移履歴テーブル
	_dfa_clear(tbl, reg->nodemax, x); //! テーブルの0番を初期状態とする
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
