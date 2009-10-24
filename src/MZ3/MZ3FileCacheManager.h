/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

class MZ3FileCacheManager
{
public:
	int		m_nDepthMax;			// 最大再帰深度
	LPCTSTR m_szDeleteFilePattern;

	MZ3FileCacheManager()
	{
		m_nDepthMax = 10;
		m_szDeleteFilePattern = L"*";
	}

	/// Countup 用のデータ構造
	struct CountupResult
	{
		int		nFiles;		///< ファイル数
		DWORD	dwSize;		///< ファイルサイズ

		CountupResult() : nFiles(0), dwSize(0) {}
	};

	/// _CountupCallback 用のデータ構造
	struct CountupCallbackData
	{
		CountupResult result;		///< 結果
		int			  nSecBack;		///< 更新時刻が何秒以上前のファイルをカウントするか

		CountupCallbackData() : nSecBack(0) {}
	};

	/// _DeleteCallback 用のデータ構造
	struct DeleteCallbackData
	{
		int		nFiles;			///< ファイル数
		int		nSecBack;		///< 更新時刻が何秒以上前のファイルをカウントするか

		DeleteCallbackData() : nFiles(0), nSecBack(0)
		{}
	};

	/**
	 * ファイル数のカウントアップ用コールバック関数
	 */
	static int _CountupCallback( const TCHAR* szDirectory,
								 const WIN32_FIND_DATA* data,
								 CountupCallbackData* pData)
	{
		if (_tcscmp(data->cFileName, _T(".")) == 0 ||
			_tcscmp(data->cFileName, _T("..")) == 0)
		{
			// 対象外なのでスキップ
			return TRUE;
		}

		// 更新日付チェック
		CTime lastWriteTime = data->ftLastWriteTime;
		CTimeSpan ts = CTime::GetCurrentTime() - lastWriteTime;
		LONGLONG elapsed_sec = ts.GetTotalSeconds();
		if (elapsed_sec < pData->nSecBack) {
			// 対象外なのでスキップ
			return TRUE;
		}

#ifdef DEBUG
		wprintf( L"directory[%s], file[%s], elapsed_sec[%d]\n", szDirectory, data->cFileName, (int)elapsed_sec );
#endif

		pData->result.nFiles ++;
		pData->result.dwSize += (data->nFileSizeHigh * MAXDWORD) + data->nFileSizeLow;

		return TRUE;
	}

	/**
	 * ファイルの削除用コールバック関数
	 */
	static int _DeleteCallback( const TCHAR* szDirectory,
							    const WIN32_FIND_DATA* data,
							    DeleteCallbackData* pData)
	{
		std::basic_string< TCHAR > strFile = szDirectory + std::basic_string< TCHAR >(data->cFileName);

		if (_tcscmp(data->cFileName, _T(".")) == 0 ||
			_tcscmp(data->cFileName, _T("..")) == 0)
		{
			// 対象外なのでスキップ
			return TRUE;
		}

		// 更新日付チェック
		CTime lastWriteTime = data->ftLastWriteTime;
		CTimeSpan ts = CTime::GetCurrentTime() - lastWriteTime;
		LONGLONG elapsed_sec = ts.GetTotalSeconds();
		if (elapsed_sec < pData->nSecBack) {
			// 対象外なのでスキップ
			return TRUE;
		}

		if( (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY ) {
			// ディレクトリ
#ifdef DEBUG
			wprintf( L"D:directory[%s], file[%s]\n", szDirectory, data->cFileName );
#endif
			if( RemoveDirectory( strFile.c_str() ) ) {
				pData->nFiles ++;
			}
		}else{
			// ファイル
#ifdef DEBUG
			wprintf( L"F:directory[%s], file[%s]\n", szDirectory, data->cFileName );
#endif
			if( DeleteFile( strFile.c_str() ) ) {
				pData->nFiles ++;
			}
		}

		return TRUE;
	}

	/**
	 * 対象ファイルの総数をカウントする
	 */
	bool GetTargetFileCount( const CStringArray& targetFolders, CountupResult* cd, int nTargetFileLastWriteDaysBack )
	{
		CountupCallbackData data;
		data.nSecBack = nTargetFileLastWriteDaysBack * 24 * 3600;

		for (int i=0; i<targetFolders.GetSize(); i++) {
			CString strLogFolder = targetFolders.GetAt(i) + L"\\";

			// ログファイル数カウント
			util::FindFileCallback( strLogFolder, m_szDeleteFilePattern, _CountupCallback, &data, m_nDepthMax );
		}

		*cd = data.result;

		return true;
	}

	/**
	 * 対象ファイルを削除する
	 */
	bool DeleteFiles( const CStringArray& targetFolders, int* pnDeleted, int nTargetFileLastWriteDaysBack )
	{
		DeleteCallbackData data;
		data.nSecBack = nTargetFileLastWriteDaysBack * 24 * 3600;

		for (int i=0; i<targetFolders.GetSize(); i++) {
			CString strLogFolder = targetFolders.GetAt(i) + L"\\";

			// ファイル削除
			util::FindFileCallback( strLogFolder, m_szDeleteFilePattern, _DeleteCallback, &data, m_nDepthMax );
		}

		*pnDeleted = data.nFiles;

		return true;
	}

};
