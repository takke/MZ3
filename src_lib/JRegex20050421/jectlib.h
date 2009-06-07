#ifndef __INC_JECTLIB__
#define __INC_JECTLIB__
#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	//- ��{�^
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


	//- �f�o�b�O�p�������`�F�b�N
void* _db_malloc(u_int size);


	//- �ėp�}�N��
#ifdef __cplusplus
# define for           if(0); else for
#endif // __cplusplus
#define lengthof(p)    (sizeof(p) / sizeof(p[0]))
#define foreach(i,p)   for(i = 0; i < (sizeof(p) / sizeof(p[0])); i++)
#define _limitu(a)     (((u_dbllong)1 << (sizeof(a) * 8)) - 1)
#define _limits(a)     (((u_dbllong)1 << (sizeof(a) * 8 - 1)) - 1)
	//- �ėp�`�F�b�N�}�N��
#define _true(a)       ((a)       ? true  : true)
#define _false(a)      ((a)       ? false : false)
#define _and(a,b)      ((a) & (b) ? true  : false)
#define _nand(a,b)     ((a) & (b) ? false : true)
#define _or(a,b)       ((a) | (b) ? true  : false)
#define _xor(a,b)      ((a) ^ (b) ? true  : false)
#define _nor(a,b)      ((a) | (b) ? false : true)
	//- �ėp����}�N��
#define _max(a,b)      ((a) >= (b) ? (a) : (b))
#define _min(a,b)      ((a) <= (b) ? (a) : (b))
#define _swap(a,b)     ((a) ^= (b) ^= (a) ^= (b))
#define _abs(a)        ((a) < 0 ? -(a) : (a))
#define jregex_range(a,b,c)  jregex__range(_min(a, b), _max(a, b), c)
#define jregex__range(a,b,c) ((a) <= (c) && (c) <= (b))
	//- �f�o�b�O�p�������`�F�b�N�}�N��
#define malloc(size)   _db_malloc((u_int)(size))


//- �K��
/*
	//      �ʏ�R�����g
	//--    �R�[�h�O���[�v
	//-     �R�[�h��؂�
	//#     �ʋ@�\�����R�����g
	//!     �d�v�ȃR�����g
	//+     �����A�ǉ��ӏ�
	//?     �f�o�b�O�p�A�]���R�[�h�p�R�����g�A�E�g
	//<     �O���]���p�R�����g
	//%name �ύX�ҁAname�̕����𖼑O��
	//@date �ύX�N�����Adate�̕����� YYYY/MM/DD �̌`����
	//$proc �R�[�h�����Aproc�̕�����ϐ����A�֐����Ȃǂ�
	//$$rtn ���ʂȕϐ��⃊�e�����Artn�̕�����K�����p
*/


//%zenryaku
//@2003/07/14


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INC_JECTLIB__
