/*
** mbenc.h : lead-byte checking functions for multi-byte encoding
**
** multi-byte encoding support for Lua
** by ham
**   - OS/Architecture independent
**   - using ANSI-C only 
*/

#ifndef mbenc_h
#define mbenc_h

/* set one of these macros to 1 */
/* if all values are 0, lua just process as single-byte encoding */
/* EUC and UTF-8 is just ok with default single-byte mode */
#define LUA_USE_ENC_SHIFT_JIS 1
#define LUA_USE_ENC_BIG5 0
#define LUA_USE_ENC_ANSI 0

/* -- japanese shift jis -- */
#if LUA_USE_ENC_SHIFT_JIS
#define is_sjis_lead_byte(c) \
  ((0x81 <= ((unsigned char) (c)) && ((unsigned char) (c)) <= 0x9f) \
  || (0xe0 <= ((unsigned char) (c)) && ((unsigned char) (c)) <=0xfc))
#define is_lead_byte(c) is_sjis_lead_byte(c)
#endif

/* -- taiwanese big5 -- */
#if LUA_USE_ENC_BIG5
/* maybe 0xA1 to 0xFE is better, but this range is ok with some sjis-japan code.
 (notice: this is not ok with japanese sjis 1byte-kana letters) */
#define is_big5_lead_byte(c) \
  (0x81 <= ((unsigned char) (c)) && ((unsigned char) (c)) <= 0xfe)
#define is_lead_byte(c) is_big5_lead_byte(c)
#endif

/* -- ANSI method --
   if you use this method, put
     setlocale(LC_ALL, ""); 
   on your main routine before executing lua functions.
*/
#if LUA_USE_ENC_ANSI
#include <ctype.h>
#define is_lead_byte(c) isleadbyte(c)
#endif

/* if no multi-byte encoding selected, just return 0(=false) */
#ifndef is_lead_byte
#define is_lead_byte(c) (0)
#endif

#endif
