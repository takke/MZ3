// kfm.cpp : 実装ファイル
//

#include "stdafx.h"
#include "kfm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

namespace kfm {

#define isjis(c) (((c)>=0x21 && (c)<=0x7e))
#define iseuc(c) (((c)>=0xa1 && (c)<=0xfe))

/* シフトJIS 1バイト目 */
#define issjis1(c) (((c)>=0x81 && (c)<=0x9f) || ((c)>=0xe0 && (c)<=0xef))

/* シフトJIS 2バイト目 */
#define issjis2(c) ((c)>=0x40 && (c)<=0xfc && (c)!=0x7f)

/* いわゆる半角カナ */
#define ishankana(c) ((c)>=0xa0 && (c)<=0xdf)

static char *codename[] = { "UNKNOWN", "SJIS", "EUC", "JIS" };

static unsigned char mimetbl[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};


void kfm::error(char *format, ...)   /* エラーを報告し，終了 */
{
    va_list argptr;

    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    fprintf(stderr, "\n");
    exit(1);
}

void kfm::sjis_to_jis(int *ph, int *pl)  /* シフトJISをJISに */
{
    if (*ph <= 0x9f) {
        if (*pl < 0x9f)  *ph = (*ph << 1) - 0xe1;
        else             *ph = (*ph << 1) - 0xe0;
    } else {
        if (*pl < 0x9f)  *ph = (*ph << 1) - 0x161;
        else             *ph = (*ph << 1) - 0x160;
    }
    if      (*pl < 0x7f) *pl -= 0x1f;
    else if (*pl < 0x9f) *pl -= 0x20;
    else                 *pl -= 0x7e;
}

void kfm::guess(int imax, unsigned char buf[])
{
    int i, bad_euc, bad_sjis;

    bad_euc = 0;
    for (i = 0; i < imax; i++) {
        if (iseuc(buf[i]) && ++i < imax) {
            if (! iseuc(buf[i])) {  bad_euc += 10;  i--;  }
            else if (buf[i-1] >= 0xd0) bad_euc++; /* 第2水準 */
            /* 1999-02-01 bug fixed.  Thanks: massangeana */
        } else if (buf[i] == 0x8e && ++i < imax) {
            if (ishankana(buf[i])) bad_euc++;
            else {  bad_euc += 10;  i--;  }
        } else if (buf[i] >= 0x80) bad_euc += 10;
    }
    bad_sjis = 0;
    for (i = 0; i < imax; i++) {
        if (issjis1(buf[i]) && ++i < imax) {
            if (! issjis2(buf[i])) {  bad_sjis += 10;  i--;  }
            else if ((unsigned) (buf[i-1] * 256U + buf[i]) >= 0x989f)
                bad_sjis++;  /* 第2水準 */
        } else if (buf[i] >= 0x80) {
            if (ishankana(buf[i])) bad_sjis++;
            else                   bad_sjis += 10;
        }
    }
    if      (bad_sjis < bad_euc) icode = SJIS;
    else if (bad_sjis > bad_euc) icode = EUC;
    else                         icode = UNKNOWN;
    if (verbose)
        fprintf(stderr,
                "%s->%s (bad_euc=%d, bad_sjis=%d)\n",
                codename[icode], codename[ocode],
                bad_euc, bad_sjis);
}

void kfm::jis_to_sjis(int *ph, int *pl)  /* JISをシフトJISに */
{
    if (*ph & 1) {
        if (*pl < 0x60)  *pl += 0x1f;
        else             *pl += 0x20;
    } else               *pl += 0x7e;
    if (*ph < 0x5f)      *ph = (*ph + 0xe1) >> 1;
    else                 *ph = (*ph + 0x161) >> 1;
}


int kfm::dehex(char c)
{
    if (c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

int mimetrue = 0;

int kfm::decode_mime(unsigned char *buf)
{
    int k, p, q;
    unsigned long c;

    p = q = 0;
    mimetrue = 0;
    /* The following line is just an optimization. */
	char* buf2 = new char(strlen((char*)buf) + 1);
	strcpy(buf2, (char*)buf);
	char* tmp;
	tmp = _strupr(buf2);

    if (buf[0] != '=') return strlen((char*)buf);
//     if (strncasecmp(buf, "=?ISO-2022-JP?B?", 16) == 0) {
    if (strncmp(tmp, "=?ISO-2022-JP?B?", 16) == 0) {
        p = 16;
        while (mimetbl[buf[p]] != 64) p++;
        q = p % 4;
        if ((q == 0 && strncmp((const char*)&buf[p],   "?=", 2) == 0) ||
            (q == 2 && strncmp((const char*)&buf[p], "==?=", 4) == 0) ||
            (q == 3 && strncmp((const char*)&buf[p],  "=?=", 3) == 0)) {
            p = 16;
            q = 0;
            c = 1;
            while ((k = mimetbl[buf[p]]) != 64) {
                c = (c << 6) | k;
                if (c & 0x1000000) {
                    c &= 0xffffff;
                    buf[q++] = (unsigned char)(c >> 16);
                    c &= 0xffff;
                    buf[q++] = (unsigned char)(c >> 8);
                    c &= 0xff;
                    buf[q++] = (unsigned char)c;
                    c = 1;
                }
                p++;
            }
            if (c & (1 << 18)) {
                c &= 0x3ffff;
                buf[q++] = (unsigned char)(c >> 10);
                c &= 0x3ff;
                buf[q++] = (unsigned char)(c >> 2);
            } else if (c & (1 << 12)) {
                c &= 0xfff;
                buf[q++] = (unsigned char)(c >> 4);
            }
            while (buf[p] == '=') p++;
            p += 2;             /* ?= */
            mimetrue = 1;
        }
//     } else if (strncasecmp(buf, "=?ISO-2022-JP?Q?", 16) == 0) {
    } else if (strncmp(tmp, "=?ISO-2022-JP?Q?", 16) == 0) {
        p = 16;
        while (buf[p] > 0x20 && buf[p] < 0x7f && buf[p] != '?') {
            if (buf[p] == '=') {
                if (dehex(buf[p + 1]) < 0) break;
                if (dehex(buf[p + 2]) < 0) break;
            }
            p++;
        }
        if (strncmp((const char*)&buf[p], "?=", 2) == 0) {
            p = 16;
            q = 0;
            while (buf[p] != '?') {
                if (buf[p] == '=') {
                    buf[q] = dehex(buf[p+1]) * 16 + dehex(buf[p+2]);
                    p += 2;
                } else if (buf[p] == '_') {
                    buf[q] = ' ';
                } else {
                    buf[q] = buf[p];
                }
                p++;  q++;
            }
            p += 2;             /* ?= */
            mimetrue = 1;
        }
    }
    if (mimetrue) {
        while (buf[p]) {
            buf[q++] = buf[p++];
        }
        buf[q] = '\0';
    } else {
	  q = strlen((const char*)buf);
    }

	delete buf2;
    return q;
}

int kfm::quoted_getc(void)
{
    int c;
    static int d = 0;
    static int e = 0;

    if (quote == 0) {
        c = inbuf.get_char();
    } else if (d != 0) {
        c = d;
        d = 0;
    } else if (e != 0) {
        c = e;
        e = 0;
    } else if ((c = inbuf.get_char()) == quote) {
        d = inbuf.get_char();
        if (isxdigit(d)) {
            e = inbuf.get_char();
            if (isxdigit(e)) {
                c = 16 * dehex(d) + dehex(e);
                d = e = 0;
            } else {
                c = quote;
            }
        } else {
            c = quote;
        }
    }
    return c;
}

int kfm::mime_getword(unsigned char *buf, int c)
{
    int p, count;

    p = count = 0;
    if (c != '=') {
        buf[p] = '\0';  return c;
    }
    buf[p++] = c;  c = quoted_getc();
    if (c != '?') {
        buf[p] = '\0';  return c;
    }
    buf[p++] = c;  c = quoted_getc();
    while (p < MAXMIMELEN - 2 && c != EOF && c > 0x20 && c < 0x7f) {
        if (c == '?' && ++count == 3) break;
        buf[p++] = c;  c = quoted_getc();
    }
    if (c != '?') {
        buf[p] = '\0';  return c;
    }
    buf[p++] = c;  c = quoted_getc();
    if (c != '=') {
        buf[p] = '\0';  return c;
    }
    buf[p++] = c;  c = quoted_getc();
    buf[p] = '\0';
    return c;
}

int kfm::mime_getspaces(unsigned char *buf, int c)
{
    int i;

    i = 0;
    while (i < 10 &&
           (c == '\n' || c == '\r' || c == ' ' || c == '\t')) {
        buf[i++] = c;  c = quoted_getc();
    }
    buf[i] = '\0';
    return c;
}

int kfm::mime_getc(void)
{
    static unsigned char buf1[MAXMIMELEN + 1], buf2[16], buf3[16];
    static int ptr1 = 0, len1 = 0, ptr2 = 0;
    static int c = EOF;
    int d;

    if (mime == 0) return quoted_getc();

    if (buf2[ptr2]) return buf2[ptr2++];
    if (ptr1 < len1) return buf1[ptr1++];
    if (c == EOF) c = quoted_getc();
    c = mime_getword(buf1, c);
    len1 = decode_mime(buf1);  ptr1 = 0;
    if (mimetrue) {
        c = mime_getspaces(buf3, c);
    } else {
	  strcpy((char*)buf2, (char*)buf3);  ptr2 = 0;  buf3[0] = '\0';
    }
    if (buf2[ptr2]) return buf2[ptr2++];
    if (ptr1 < len1) return buf1[ptr1++];
    d = c;  c = EOF;  return d;
}

int kfm::mygetc(void)
{
    static int i, imax = 0;
    static unsigned char buf[BUFFER_SIZE];
    int c;

    if (imax == 0) {
        if ((c = mime_getc()) < 0x80 || c == EOF) {
            return c;
        } else {
            do {
                buf[imax++] = c;
            } while (imax < (int) sizeof(buf) && (c = mime_getc()) != EOF);
            if (icode == UNKNOWN) guess(imax, (unsigned char*)buf);
            i = 0;
            return buf[i++];
        }
    } else if (i < imax) {
        return buf[i++];
    } else {
        if ((c = mime_getc()) == EOF) imax = 0;
        return c;
    }
}

void kfm::myputc(int c)
{
    static int crcount = 0;
    static int lastc = '\n';

    if (c == '\r') {
        crcount++;
    } else if (c == '\n') {
#ifdef unix
        if (ocode == SJIS) outbuf.put_char('\r');
#endif
        if (outbuf.put_char('\n') == EOF) error("Write error");
        crcount = 0;  lastc = '\n';
    } else {
        while (crcount) {
#ifdef unix
            if (ocode == SJIS) outbuf.put_char('\r');
#endif
            if (outbuf.put_char('\n') == EOF) error("Write error");
            crcount--;  lastc = '\n';
        }
        if (c != EOF) {
            outbuf.put_char(c);  lastc = c;
        }
    }
}

void kfm::tosjis(void)
{
    int c=0, d, flag;
    enum {NORMAL, KANJI, HANKANA} mode = NORMAL;

    icode = default_icode;
    flag = 0;
    while (flag || (c = mygetc()) != EOF) {
        flag = 0;
        if (c == 0x1b) {
            if ((c = mygetc()) == '$') {
                if ((c = mygetc()) == '@' || c == 'B') {
                    mode = KANJI;
                } else if (! text) {
                    myputc(0x1b);  myputc('$');  myputc(c);
                }
            } else if (c == '(') {
                if ((c = mygetc()) == 'B' || c == 'J') {
                    mode = NORMAL;
                } else if (c == 'I') {
                    mode = HANKANA;
                } else if (! text) {
                    myputc(0x1b);  myputc('(');  myputc(c);
                }
            } else if (! text) {
                myputc(0x1b);  myputc(c);
            }
        } else if (c == 0x0e) {
            mode = HANKANA;
        } else if (c == 0x0f) {
            mode = NORMAL;
        } else if (mode == KANJI && isjis(c)) {
            d = mygetc();
            if (isjis(d)) {
                jis_to_sjis(&c, &d);
                myputc(c);  myputc(d);
            } else {
                c = d;  flag = 1;
            }
        } else if (mode == HANKANA) {
            if (c >= 0x20 && c <= 0x5f) {
                myputc(c | 0x80);
            } else {
                mode = NORMAL;
                flag = 1;
            }
        } else if (icode == EUC) {
            if (iseuc(c)) {
                d = mygetc();
                if (iseuc(d)) {
                    c &= 0x7f;  d &= 0x7f;
                    jis_to_sjis(&c, &d);
                    myputc(c);  myputc(d);
                } else {
                    c = d;  flag = 1;
                }
            } else if (c == 0x8e) {
                c = mygetc();
                if (ishankana(c)) {
                    myputc(c);
                } else {
                    flag = 1;
                }
            } else if (! text || c == '\t' || c == '\n' || c == '\r' ||
                       c == 12 || (c >= ' ' && c < 0x7f)) {
                myputc(c);
            }
        } else if (! text || c == '\t' || c == '\n' || c == '\r' ||
                   c == 12 || (c >= ' ' && c < 0x7f)) {
            myputc(c);
        } else if (issjis1(c)) {
            d = mygetc();
            if (issjis2(d)) {
                myputc(c);  myputc(d);
            }
        } else if (ishankana(c)) {
            myputc(c);
        }
    }
    myputc(EOF);
}

void kfm::tojis(void)
{
    int c, d, flag;
    enum {NORMAL, KANJI, HANKANA} mode = NORMAL;

    icode = default_icode;
    flag = 0;
    while (flag || (c = mygetc()) != EOF) {
        flag = 0;
        if (icode == UNKNOWN) {
            myputc(c);
        } else if (icode == SJIS) {
            if (issjis1(c)) {
                d = mygetc();
                if (issjis2(d)) {
                    sjis_to_jis(&c, &d);
                    if (mode != KANJI) {
                        mode = KANJI;
                        myputc(0x1b);  myputc('$');  myputc('B');
                    }
                    myputc(c);  myputc(d);
                } else {
                    c = d;  flag = 1;
                }
            } else if (ishankana(c)) {
                if (mode != HANKANA) {
                    mode = HANKANA;
                    myputc(0x1b);  myputc('(');  myputc('I');
                }
                myputc(c & 0x7f);
            } else {
                if (mode != NORMAL) {
                    myputc(0x1b);  myputc('(');  myputc('B');
                }
                mode = NORMAL;
                if (! text || c == '\t' || c == '\n' || c == '\r' ||
                    c == 12 || (c >= ' ' && c < 0x7f)) {
                    myputc(c);
                }
            }
        } else {  /* EUC */
            if (iseuc(c)) {
                d = mygetc();
                if (iseuc(d)) {
                    if (mode != KANJI) {
                        mode = KANJI;
                        myputc(0x1b);  myputc('$');  myputc('B');
                    }
                    myputc(c & 0x7f);  myputc(d & 0x7f);
                } else {
                    c = d;  flag = 1;
                }
            } else if (c == 0x8e) {
                c = mygetc();
                if (ishankana(c)) {
                    if (mode != HANKANA) {
                        mode = HANKANA;
                        myputc(0x1b);  myputc('(');  myputc('I');
                    }
                    myputc(c & 0x7f);
                } else {
                    flag = 1;
                }
            } else {
                if (mode != NORMAL) {
                    myputc(0x1b);  myputc('(');  myputc('B');
                }
                mode = NORMAL;
                if (! text || c == '\t' || c == '\n' || c == '\r' ||
                    c == 12 || (c >= ' ' && c < 0x7f)) {
                    myputc(c);
                }
            }
        }
    }
    myputc(EOF);
}

void kfm::toeuc(void)
{
    int c=0, d, flag;
    enum {NORMAL, KANJI, HANKANA} mode = NORMAL;

    icode = default_icode;
    flag = 0;
    while (flag || (c = mygetc()) != EOF) {
        flag = 0;
        if (c == 0x1b) {
            if ((c = mygetc()) == '$') {
                if ((c = mygetc()) == '@' || c == 'B') {
                    mode = KANJI;
                } else if (! text) {
                    myputc(0x1b);  myputc('$');  myputc(c);
                }
            } else if (c == '(') {
                if ((c = mygetc()) == 'B' || c == 'J') {
                    mode = NORMAL;
                } else if (c == 'I') {
                    mode = HANKANA;
                } else if (! text) {
                    myputc(0x1b);  myputc('(');  myputc(c);
                }
            } else if (! text) {
                myputc(0x1b);  myputc(c);
            }
        } else if (c == 0x0e) {
            mode = HANKANA;
        } else if (c == 0x0f) {
            mode = NORMAL;
        } else if (mode == KANJI && isjis(c)) {
            d = mygetc();
            if (isjis(d)) {
                myputc(c | 0x80);  myputc(d | 0x80);
            } else {
                c = d;  flag = 1;
            }
        } else if (mode == HANKANA && c >= 0x20 && c <= 0x5f) {
            myputc(0x8e);  myputc(c | 0x80);
        } else if (icode == SJIS) {
            if (issjis1(c)) {
                d = mygetc();
                if (issjis2(d)) {
                    sjis_to_jis(&c, &d);
                    myputc(c | 0x80);  myputc(d | 0x80);
                } else {
                    c = d;  flag = 1;
                }
            } else if (ishankana(c)) {
                myputc(0x8e);  myputc(c);
            } else if (! text || c == '\t' || c == '\n' || c == '\r' ||
                       c == 12 || (c >= ' ' && c < 0x7f)) {
                myputc(c);
            }
        } else if (! text || c == '\t' || c == '\n' || c == '\r' ||
                   c == 12 || (c >= ' ' && c < 0x7f)) {
            myputc(c);
        } else if (iseuc(c)) {
            d = mygetc();
            if (iseuc(d)) {
                myputc(c);  myputc(d);
            }
        } else if (c == 0x8e) {
            d = mygetc();
            if (ishankana(d)) {
                myputc(c);  myputc(d);
            }
        } else if (c == 0x8f) {
            c = mygetc();
            d = mygetc();
            if (iseuc(c) && iseuc(d)) {
                myputc(0x8f);  myputc(c);  myputc(d);
            }
        }
    }
    myputc(EOF);
}

/// UTF-8からMBCSへの変換
int utf8_to_mbcs( const kf_buf_type& input, kf_buf_type& output )
{
	LPSTR pszUTF8 = NULL;
	bool bUtf8withBOM = false;

	pszUTF8 = (LPSTR)calloc( input.size(), sizeof(char) );
	if ( pszUTF8 == NULL ) {
		return -1;
	}

	// BOM判定
	if ( (unsigned char)input[0] == 0xEF && (unsigned char)input[1] == 0xBB && (unsigned char)input[2] == 0xBF ) {
		bUtf8withBOM = true;
	} else {
		bUtf8withBOM = false;
	}

	CStringA strUtf8( bUtf8withBOM ? (LPSTR)&input[0]+3 : (LPSTR)&input[0], bUtf8withBOM ? input.size()-3 : input.size() );

	// UTF-8 から UCS-2 に変換した文字列の長さを計算
	int nUCS2Length = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0 );
	if (nUCS2Length > 0) {
		wchar_t * pszUCS2 = (wchar_t*)calloc( nUCS2Length, sizeof(wchar_t) );
		if ( pszUCS2 == NULL ) {
			free(pszUTF8);
			return -1;
		}

		// UTF-8 => UCS-2
		MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, pszUCS2, nUCS2Length );

		// UCS-2 から MBCS に変換した文字列の長さを計算
		// (CP-932)
		LPSTR pszAcp = NULL;
		int nAcpLength = WideCharToMultiByte(GetACP(), 0, pszUCS2, -1, pszAcp, 0, NULL, NULL);
		if ( nAcpLength > 0 ) {
			pszAcp = (LPSTR)calloc( nAcpLength, sizeof(char) );
			if ( pszAcp == NULL ) {
				free(pszUTF8);
				free(pszUCS2);
				return -1;
			}

			// UCS-2 => MBCS
			WideCharToMultiByte(GetACP(), 0, pszUCS2, -1, pszAcp, nAcpLength, NULL, NULL);

			// 結果を追加
			output.resize( nAcpLength );
			memcpy( &output[0], pszAcp, nAcpLength );

			free(pszAcp);
		}
		free(pszUCS2);
	}
	free(pszUTF8);

	return output.size();
}

}