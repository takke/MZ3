/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// ���r�W�����ԍ��F
// "get_mz3_trunk_revision.vbs" �� mz3.jp ����擾���A�������ꂽ�t�@�C���� include ����B
#include "mz3_revision_in.h"

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
