/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/// �o�[�W����������
#define MZ3_VERSION_TEXT_SHORT		L"0.9.3.0"
#define MZ3_VERSION_TEXT			L"Version " MZ3_VERSION_TEXT_SHORT

/// ���r�W�����ԍ�
/// "get_mz3_trunk_revision.vbs" �� mz3.jp ����擾���A�������ꂽ�t�@�C���� include ����B
#include "mz3_revision.h"

/// MZ3 �p���[�e�B���e�B
namespace util
{

/**
 * ���r�W�����ԍ����擾����
 *
 * ��F"[94]"
 */
inline CString GetSourceRevision()
{
	CString rev = MZ3_SVN_REVISION;	// r998
	rev.Replace( L"r", L" [" );
	rev += L"]";
	return rev;
}

}
