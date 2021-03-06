/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。

#pragma once

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

// メモ - この値は、ターゲットになっている Windows CE OS バージョンと相互に強く関連付けられていません。
#ifdef WINCE
# define WINVER _WIN32_WCE
#else
# define WINVER 0x0500
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows ヘッダーから使用されていない部分を除外します。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 一部の CString コンストラクタは明示的です。
#ifdef _CE_DCOM
#define _ATL_APARTMENT_THREADED
#endif

// 一般的で無視しても安全な MFC の警告メッセージの一部の非表示を解除します。
#define _AFX_ALL_WARNINGS

#ifdef WINCE
#define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA
#endif

#ifdef WINCE
# include <ceconfig.h>
#endif

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
# define SHELL_AYGSHELL
#endif

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC の Internet Explorer 4 コモン コントロール サポート
#endif



#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef WINCE
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib") 
#endif

#if (_WIN32_WCE < 0x500) && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
	#pragma comment(lib, "ccrtrtti.lib")
	#ifdef _X86_	
		#if defined(_DEBUG)
			#pragma comment(lib, "libcmtx86d.lib")
		#else
			#pragma comment(lib, "libcmtx86.lib")
		#endif
	#endif
#endif

#ifdef WINCE
# include <altcecrt.h>
#endif


#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif

#include <afx.h>
#include <afxdlgs.h>
#include <afxctl.h>
#include "wininet.h"

// Win32 用依存ファイル
#ifndef WINCE
# include <math.h>
# include <shlobj.h>
// GDI+
# include <atlimage.h>
# include <gdiplus.h>
#endif

// MD5
#include "../../src_lib/md5-cc/md5.h"

// lua support
extern "C" {
#include "../../src_lib/lua/src/lua.h"
#include "../../src_lib/lua/src/lualib.h"
#include "../../src_lib/lua/src/lauxlib.h"
}

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <hash_map>
#include <map>
#include <hash_set>
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <list>

#include "MZ3.h"
