/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#include "stdafx.h"

#ifndef WINCE

#include "Ran2Image.h"
#include <process.h>

// ファイルからのコンストラクタ
Ran2Image::Ran2Image(const wchar_t* fileName, BOOL isColorManagement) 
 : Image(fileName, isColorManagement)
{
	Initialize();
	m_isInitialized = true;
	PrepareAnimation();

	m_strFilename = fileName;
}


Ran2Image::~Ran2Image()
{
	Destroy();
}


// アニメーションの初期化
bool Ran2Image::InitAnimation(Graphics* graphics, CPoint pt,int lineHeight)
{
	charHeight = lineHeight;
	m_graphics = graphics;
	m_pt = pt;

	if( !m_isInitialized ){
		TRACE(TEXT("GIFの初期化にしっぱい（　´Д｀）\r\n"));
		return false;
	};

	if( IsAnimatedGIF() ){
		if( m_hThread == NULL ){
			unsigned int threadID = 0;
			m_hThread = (HANDLE)_beginthreadex(NULL,0,_GoRan2Proc,this,CREATE_SUSPENDED,&threadID);
			
			if( !m_hThread ){
				TRACE(TEXT("スッドレ始動に失敗\r\n"));
				return(true);
			}else{ 
				ResumeThread(m_hThread);
			}
		}
	}else{
		// 普通に描画
		long hmWidth = GetWidth();
		long hmHeight = GetHeight();

		// 文字の大きさに合わせて拡大(絵文字以下のフォントの場合はそのまんまなので重なるかもね)
		if( hmWidth < charHeight ){
			double gaijiScale = (double)charHeight / (double)hmWidth;
			hmWidth  = (int)(hmWidth *gaijiScale);
			hmHeight = (int)(hmHeight*gaijiScale);
		}

		if( m_graphics != NULL ){
			m_graphics->DrawImage(this, m_pt.x, m_pt.y, hmWidth, hmHeight);
		}
	}
	return(false);	

}

CSize Ran2Image::GetSize()
{
	return CSize(GetWidth(), GetHeight());
}


// コマ数の確認を含めた準備
bool Ran2Image::PrepareAnimation()
{
   UINT count = 0;
   count = GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   GetFrameDimensionsList(pDimensionIDs, count);
   m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);
   int nSize = GetPropertyItemSize(PropertyTagFrameDelay);
   m_pPropertyItem = (PropertyItem*) malloc(nSize);
   GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);

   delete[] pDimensionIDs;

   return(m_nFrameCount > 1);
}


// メンバの初期化
void Ran2Image::Initialize()
{
	m_graphics = NULL;
	m_hThread = NULL;
	m_pPropertyItem = NULL;
	m_isInitialized = false;
	m_nFramePosition = 0;
	m_nFrameCount = 0;
	lastResult = InvalidParameter;

//	m_hInst = _Module.GetResourceInstance();
	m_hInst = AfxGetResourceHandle();

	m_bPause = false;
	m_hExitEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hPause = ::CreateEvent(NULL,TRUE,TRUE,NULL);
}


// スレッドの生成
UINT WINAPI Ran2Image::_GoRan2Proc(LPVOID pParam)
{
	ASSERT(pParam);
	Ran2Image *pImage = reinterpret_cast<Ran2Image *> (pParam);
	pImage->AnimationThread();

	return(0);
}


// パターンの描画
bool Ran2Image::DrawFrameGIF()
{
	::WaitForSingleObject(m_hPause, INFINITE);
	GUID   pageGuid = FrameDimensionTime;
	long hmWidth = GetWidth();
	long hmHeight = GetHeight();

	if( m_graphics != NULL ){
		// 文字の大きさに合わせて拡大(絵文字以下のフォントの場合はそのまんまなので重なるかもね)
		if( hmWidth < charHeight ){
			double gaijiScale = (double)charHeight / (double)hmWidth;
			hmWidth  = (int)(hmWidth *gaijiScale);
			hmHeight = (int)(hmHeight*gaijiScale);
		}

		m_graphics->DrawImage(this, m_pt.x, m_pt.y, hmWidth, hmHeight);
	}

	SelectActiveFrame(&pageGuid, m_nFramePosition++);		
	// 末端まで到達したら始点へ
	if (m_nFramePosition == m_nFrameCount){
		m_nFramePosition = 0;
	}

	long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
	DWORD dwErr = ::WaitForSingleObject(m_hExitEvent, lPause);

	return( dwErr == WAIT_OBJECT_0 );
}


// スレッドの始動
void Ran2Image::AnimationThread()
{
	m_nFramePosition = 0;

	bool isExit = false;
	while( isExit == false ){
		isExit = DrawFrameGIF();
	}
}


// 一時停止
void Ran2Image::SetPause(bool bPause)
{
	if( !IsAnimatedGIF() )
		return;

	if( bPause && !m_bPause ){
		::ResetEvent(m_hPause);
	}else{
		if( m_bPause && !bPause ){
			::SetEvent(m_hPause);
		}
	}

	m_bPause = bPause;
}


// メンバの破棄
void Ran2Image::Destroy()
{
	// 稼動中の場合は一時停止してから終了させる
	if( m_hThread ){
		SetPause(false);

		::SetEvent(m_hExitEvent);
		::WaitForSingleObject(m_hThread, INFINITE);
	}

	::CloseHandle(m_hThread);
	::CloseHandle(m_hExitEvent);
	::CloseHandle(m_hPause);

	free(m_pPropertyItem);

	m_pPropertyItem = NULL;
	m_hThread = NULL;
	m_hExitEvent = NULL;
	m_hPause = NULL;

}

#endif // #ifndef WINCE
