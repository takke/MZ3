/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/// �o�[�W����������
#define MZ3_VERSION_TEXT_SHORT		L"0.9.2.0 Beta7"
#define MZ3_VERSION_TEXT			L"Version " MZ3_VERSION_TEXT_SHORT

/// ���r�W�����ԍ�
#define MZ3_SVN_REVISION			L"$Rev$"

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
	CString rev = MZ3_SVN_REVISION;	// $Rev$
	rev.Replace( L"$Rev: ", L" [" );
	rev.Replace( L" $", L"]" );
	return rev;
}

}
