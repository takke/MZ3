#pragma once

#include "locale.h"
#include "JRegex20050421/JRegex.h"
#include <string>
#include <vector>

/**
 * ���K�\�����C�u���� JRegex �̃��b�p�[
 */
class MyRegex
{
public:
	/// ���K�\���̌������ʂ�\���N���X
	class Result {
	public:
		std::wstring str;	///< �}�b�`����������
		u_int start;		///< �}�b�`�����ʒu
		u_int end;			///< �}�b�`�I���ʒu

		/// �R���X�g���N�^
		Result( u_int start_, u_int end_, const std::wstring& str_ )
			: start(start_), end(end_), str(str_)
		{}
	};

private:
	// JRegex �̃I�u�W�F�N�g
	jreg* reg;

public:
	/**
	 * exec �̌���
	 */
	std::vector<Result> results;

	/// �R���X�g���N�^
	MyRegex() : reg(NULL) {}

	/// �f�X�g���N�^
	~MyRegex() {
		if( reg != NULL ) {
			jreg_free(reg);
		}
	}

	/**
	 * ���K�\�����R���p�C������
	 */
	bool compile( LPCTSTR pattern, jreg_opt opt=JREG_NORMAL )
	{
		// JRegex ���g���Ă݂�
		reg = jreg_comp( (const t_code*)pattern, opt );
		if( reg ) {
			return true;
		}else{
			return false;
		}
	}

	/**
	 * �R���p�C���ς݂��ǂ�����Ԃ�
	 */
	bool isCompiled() {
		return reg!=NULL;
	}

	/**
	 * �}�b�`���s�Bresults �Ɍ������ʂ��i�[�����B
	 */
	bool exec( LPCTSTR target )
	{
		if( reg == NULL ) {
			return false;
		}

		results.clear();

		PATTERN_STR pat;
		pat.PATTERN_TCODE = (t_code*)target;
		if( jreg_exec(reg,pat,0,(u_int)wcslen(target),JREG_NORMAL) ) {
			// �����B
			// �������ʂ� results �ɃZ�b�g�B
			for( u_int i=0; i<reg->backrefcnt; i++ ) {
				u_int start = reg->backref[i].start;
				u_int width = reg->backref[i].match-1;
				if( width <= 0 ) {
					results.push_back( Result( -1, -1, std::wstring() ) );
				}else{
					results.push_back( 
						Result( start,
								start+width,
								std::wstring(target+start,width) ) );
				}
			}

			return true;
		}else{
			return false;
		}
	}
};

