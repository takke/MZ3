/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

// メモ - この値は、ターゲットになっている Windows CE OS バージョンと相互に強く関連付けられていません。
#ifndef WINVER
# ifdef WINCE
#  define WINVER _WIN32_WCE
# else
#  define WINVER 0x0500
# endif
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows ヘッダーから使用されていない部分を除外します。
#endif


#ifdef WINCE

# include <ceconfig.h>
# if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#  define SHELL_AYGSHELL
# endif

/*
# ifdef _CE_DCOM
#  define _ATL_APARTMENT_THREADED
# endif
*/
# include <aygshell.h>
# pragma comment(lib, "aygshell.lib") 

#endif

// Windows ヘッダー ファイル:
#include <windows.h>

#ifdef WINCE

# if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#  ifndef _DEVICE_RESOLUTION_AWARE
#   define _DEVICE_RESOLUTION_AWARE
#  endif
# endif

/*
# ifdef _DEVICE_RESOLUTION_AWARE
#  include "DeviceResolutionAware.h"
# endif
*/

/*
# if _WIN32_WCE < 0x500 && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
	#pragma comment(lib, "ccrtrtti.lib")
	#ifdef _X86_	
		#if defined(_DEBUG)
			#pragma comment(lib, "libcmtx86d.lib")
		#else
			#pragma comment(lib, "libcmtx86.lib")
		#endif
	#endif
# endif

# include <altcecrt.h>
*/
#endif

#ifdef JREGEXDLL_EXPORTS
#define JREGEXDLL_API __declspec(dllexport)
#else
#define JREGEXDLL_API __declspec(dllimport)
#endif

extern JREGEXDLL_API int nJRegexDLL;

JREGEXDLL_API int fnJRegexDLL(void);
