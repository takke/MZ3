#ifndef __INC_JECTLIB__
#define __INC_JECTLIB__
#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	//- 基本型
#ifndef __RPCNDR_H__
typedef unsigned char  byte;
#endif // __RPCNDR_H__
typedef unsigned short word;
typedef unsigned long  dword;

typedef signed int     s_int;
typedef signed char    s_char;
typedef signed short   s_short;
typedef signed long    s_long;

#ifndef _BSDTYPES_DEFINED
typedef unsigned int   u_int;
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned long  u_long;
#define _BSDTYPES_DEFINED
#endif // _BSDTYPES_DEFINED

#ifdef _MSC_VER
typedef signed __int64         s_dbllong;
typedef unsigned __int64       u_dbllong;
#else
typedef signed long long int   s_dbllong;
typedef unsigned long long int u_dbllong;
#endif // _MSC_VER

#ifndef __cplusplus
typedef enum _BOOL
{
	false = 0,
	true  = !false,
}bool;
#endif // __cplusplus
#define null (0)
//#define nil false


#include <malloc.h>
#ifdef _MSC_VER
# include <stdlib.h>
#endif // _MSC_VER


	//- デバッグ用メモリチェック
void* _db_malloc(u_int size);


	//- 汎用マクロ
#ifdef __cplusplus
# define for           if(0); else for
#endif // __cplusplus
#define lengthof(p)    (sizeof(p) / sizeof(p[0]))
#define foreach(i,p)   for(i = 0; i < (sizeof(p) / sizeof(p[0])); i++)
#define _limitu(a)     (((u_dbllong)1 << (sizeof(a) * 8)) - 1)
#define _limits(a)     (((u_dbllong)1 << (sizeof(a) * 8 - 1)) - 1)
	//- 汎用チェックマクロ
#define _true(a)       ((a)       ? true  : true)
#define _false(a)      ((a)       ? false : false)
#define _and(a,b)      ((a) & (b) ? true  : false)
#define _nand(a,b)     ((a) & (b) ? false : true)
#define _or(a,b)       ((a) | (b) ? true  : false)
#define _xor(a,b)      ((a) ^ (b) ? true  : false)
#define _nor(a,b)      ((a) | (b) ? false : true)
	//- 汎用操作マクロ
#define _max(a,b)      ((a) >= (b) ? (a) : (b))
#define _min(a,b)      ((a) <= (b) ? (a) : (b))
#define _swap(a,b)     ((a) ^= (b) ^= (a) ^= (b))
#define _abs(a)        ((a) < 0 ? -(a) : (a))
#define jregex_range(a,b,c)  jregex__range(_min(a, b), _max(a, b), c)
#define jregex__range(a,b,c) ((a) <= (c) && (c) <= (b))
	//- デバッグ用メモリチェックマクロ
#define malloc(size)   _db_malloc((u_int)(size))


//- 規定
/*
	//      通常コメント
	//--    コードグループ
	//-     コード区切り
	//#     個別機能説明コメント
	//!     重要なコメント
	//+     改造、追加箇所
	//?     デバッグ用、評価コード用コメントアウト
	//<     外部評価用コメント
	//%name 変更者、nameの部分を名前に
	//@date 変更年月日、dateの部分を YYYY/MM/DD の形式で
	//$proc コード説明、procの部分を変数名、関数名などに
	//$$rtn 特別な変数やリテラル、rtnの部分を適所利用
*/


//%zenryaku
//@2003/07/14


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INC_JECTLIB__
