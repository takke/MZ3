#pragma once

/// �o�[�W����������
#define MZ3_VERSION_TEXT			L"Version 0.8.2.0 Beta30" 

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
