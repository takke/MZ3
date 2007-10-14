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

#ifdef __AFX_H__
	bool replaceAll( CString& target, const std::wstring& replace ) {
		std::wstring buf = target;
		if( replaceAll( buf, replace ) ) {
			target = buf.c_str();
			return true;
		}else{
			return false;
		}
	}
#endif

	/**
	 * ���K�\���Ƀ}�b�`��������������̒u���B
	 *
	 * jreg_replace �ł͂Ȃ� jreg_exec �œƎ��Ɏ�������B
	 *
	 * @param target  [in/out] �u���Ώە�����B
	 * @param replace [in]     �u���p�^�[���B
	 *                         {1}�`{10} ��() �Ń}�b�`��������������ɒu������B
	 */
	bool replaceAll( std::wstring& target, const std::wstring& replace ) {

		std::wstring rest = target;
		target = L"";

		for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~
			if( !this->exec(rest.c_str()) ) {
				// �������B
				// �c��̕������ǉ����ďI���B
				target += rest;
				break;
			}

			// �����B

			// �}�b�`������S�̂̍������o��
			target.append( rest.c_str(), this->results[0].start );

			// replace �p�^�[���Œu��
			size_t len = replace.size();
			for( size_t i=0; i<len; i++ ) {
				if( replace[i] == '{' ) {
					// {N} �Ȃ�u��

					// N ���o
					std::wstring str_n;
					i++;
					for( ; i<len; i++ ) {
						int ch = replace[i];
						if( ch == '}' ) {
							// '}' �𔭌������̂ŏI��
							break;
						}
						str_n.push_back( ch );
					}
					if( i==len ) {
						// '}' �������Ȃ̂ł��̂܂ܒǉ�
						target.push_back( '{' );
						target.append( str_n );
					}else{
						// '}' �����B

						// str_n �� [0-9] �Ȃ琔�l�ϊ�
						size_t j=0;
						size_t str_n_len=str_n.size();
						for( ; j<str_n_len; j++ ) {
							if( !isdigit(str_n[j]) ) {
								break;
							}
						}
						if( j==str_n_len ) {
							// [0-9] �Ȃ̂� str_n �𐔒l�ϊ�
							size_t n = _wtoi(str_n.c_str());
							
							// n �� 0 �ł���΃}�b�`������S�̂ɒu���B
							// n ���}�b�`�����񐔖����ł���΁A�u���B
							if( n == 0 ) {
								// �}�b�`������S�̂�ǉ�
								target.append( results[0].str );
							}else if( n < results.size() ) {
								// �}�b�`�����񐔖����Ȃ̂ŁA�u������B
								target.append( results[n].str );
							}else{
								// �}�b�`�����񐔈ȏ�Ȃ̂ŁA�u�������ɒǉ�
								target.push_back( '{' );
								target.append( str_n );
								target.push_back( '}' );
							}
						}else{
							// [0-9] �ȊO���܂܂�Ă����̂Œu�������ɒǉ�
							target.push_back( '{' );
							target.append( str_n );
							target.push_back( '}' );
						}
					}
				}else{
					// '{' �ȊO�Ȃ̂ł��̂܂ܒǉ�
					target.push_back( replace[i] );
				}
			}

			// �^�[�Q�b�g���X�V�B
			rest.erase( 0, results[0].end );
		}

		return true;
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
