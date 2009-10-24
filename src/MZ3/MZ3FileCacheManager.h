/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

class MZ3FileCacheManager
{
public:
	int		m_nDepthMax;			// �ő�ċA�[�x
	LPCTSTR m_szDeleteFilePattern;

	MZ3FileCacheManager()
	{
		m_nDepthMax = 10;
		m_szDeleteFilePattern = L"*";
	}

	/// Countup �p�̃f�[�^�\��
	struct CountupResult
	{
		int		nFiles;		///< �t�@�C����
		DWORD	dwSize;		///< �t�@�C���T�C�Y

		CountupResult() : nFiles(0), dwSize(0) {}
	};

	/// _CountupCallback �p�̃f�[�^�\��
	struct CountupCallbackData
	{
		CountupResult result;		///< ����
		int			  nSecBack;		///< �X�V���������b�ȏ�O�̃t�@�C�����J�E���g���邩

		CountupCallbackData() : nSecBack(0) {}
	};

	/// _DeleteCallback �p�̃f�[�^�\��
	struct DeleteCallbackData
	{
		int		nFiles;			///< �t�@�C����
		int		nSecBack;		///< �X�V���������b�ȏ�O�̃t�@�C�����J�E���g���邩

		DeleteCallbackData() : nFiles(0), nSecBack(0)
		{}
	};

	/**
	 * �t�@�C�����̃J�E���g�A�b�v�p�R�[���o�b�N�֐�
	 */
	static int _CountupCallback( const TCHAR* szDirectory,
								 const WIN32_FIND_DATA* data,
								 CountupCallbackData* pData)
	{
		if (_tcscmp(data->cFileName, _T(".")) == 0 ||
			_tcscmp(data->cFileName, _T("..")) == 0)
		{
			// �ΏۊO�Ȃ̂ŃX�L�b�v
			return TRUE;
		}

		// �X�V���t�`�F�b�N
		CTime lastWriteTime = data->ftLastWriteTime;
		CTimeSpan ts = CTime::GetCurrentTime() - lastWriteTime;
		LONGLONG elapsed_sec = ts.GetTotalSeconds();
		if (elapsed_sec < pData->nSecBack) {
			// �ΏۊO�Ȃ̂ŃX�L�b�v
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
	 * �t�@�C���̍폜�p�R�[���o�b�N�֐�
	 */
	static int _DeleteCallback( const TCHAR* szDirectory,
							    const WIN32_FIND_DATA* data,
							    DeleteCallbackData* pData)
	{
		std::basic_string< TCHAR > strFile = szDirectory + std::basic_string< TCHAR >(data->cFileName);

		if (_tcscmp(data->cFileName, _T(".")) == 0 ||
			_tcscmp(data->cFileName, _T("..")) == 0)
		{
			// �ΏۊO�Ȃ̂ŃX�L�b�v
			return TRUE;
		}

		// �X�V���t�`�F�b�N
		CTime lastWriteTime = data->ftLastWriteTime;
		CTimeSpan ts = CTime::GetCurrentTime() - lastWriteTime;
		LONGLONG elapsed_sec = ts.GetTotalSeconds();
		if (elapsed_sec < pData->nSecBack) {
			// �ΏۊO�Ȃ̂ŃX�L�b�v
			return TRUE;
		}

		if( (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY ) {
			// �f�B���N�g��
#ifdef DEBUG
			wprintf( L"D:directory[%s], file[%s]\n", szDirectory, data->cFileName );
#endif
			if( RemoveDirectory( strFile.c_str() ) ) {
				pData->nFiles ++;
			}
		}else{
			// �t�@�C��
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
	 * �Ώۃt�@�C���̑������J�E���g����
	 */
	bool GetTargetFileCount( const CStringArray& targetFolders, CountupResult* cd, int nTargetFileLastWriteDaysBack )
	{
		CountupCallbackData data;
		data.nSecBack = nTargetFileLastWriteDaysBack * 24 * 3600;

		for (int i=0; i<targetFolders.GetSize(); i++) {
			CString strLogFolder = targetFolders.GetAt(i) + L"\\";

			// ���O�t�@�C�����J�E���g
			util::FindFileCallback( strLogFolder, m_szDeleteFilePattern, _CountupCallback, &data, m_nDepthMax );
		}

		*cd = data.result;

		return true;
	}

	/**
	 * �Ώۃt�@�C�����폜����
	 */
	bool DeleteFiles( const CStringArray& targetFolders, int* pnDeleted, int nTargetFileLastWriteDaysBack )
	{
		DeleteCallbackData data;
		data.nSecBack = nTargetFileLastWriteDaysBack * 24 * 3600;

		for (int i=0; i<targetFolders.GetSize(); i++) {
			CString strLogFolder = targetFolders.GetAt(i) + L"\\";

			// �t�@�C���폜
			util::FindFileCallback( strLogFolder, m_szDeleteFilePattern, _DeleteCallback, &data, m_nDepthMax );
		}

		*pnDeleted = data.nFiles;

		return true;
	}

};
