/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
/**
 * URL �G���R�[�h�p���[�e�B���e�B
 */
#pragma once
#include "kfm.h"

/// URL �G���R�[�_
class URLEncoder
{
private:
	static unsigned long UrlEncode( unsigned char *csource,
							 unsigned char *cbuffer,
							 unsigned long lbuffersize )
	{
		unsigned long   llength;		// csource �̃T�C�Y���i�[
		unsigned long   lcount = 0;		// csource �̓ǂݍ��݈ʒu�J�E���^
		unsigned char   cbyte;			// �����o���ꂽ 1 �o�C�g���̃f�[�^���i�[
		unsigned char   ctemp[4];		// �ϊ�����(1 ������)�ꎞ�i�[�o�b�t�@
		unsigned long   lresultcount = 0; // cbuffer �̏������݈ʒu�J�E���^

		llength = (unsigned long)strlen((char*)csource); // csource �̕����T�C�Y�𓾂�
		if (!llength) {
			return lresultcount;
		} // csource �� 0 �����̏ꍇ�A�֐��𔲂���
		if (lbuffersize < (llength * 3 + 1)) {
			return lresultcount;
		} // �o�b�t�@�T�C�Y������Ȃ��ꍇ�A�֐��𔲂���

		while (1) {
			cbyte = *(csource + lcount); // 1 �o�C�g�𔲂��o��

			if (cbyte == 0x20) {	// 1 �o�C�g���p�X�y�[�X(" ")�������ꍇ
				strncpy((char*)(cbuffer + lresultcount), "+", 2);	// "+" �� cbuffer �ɃR�s�[
				lcount++;											// �ǂݍ��݃J�E���^���C���N�������g
				lresultcount++;										// �������݃J�E���^���C���N�������g
			} else if (((cbyte >= 0x40) && (cbyte <= 0x5A)) ||		// @A-Z
				((cbyte >= 0x61) && (cbyte <= 0x7A)) ||				// a-z 
				((cbyte >= 0x30) && (cbyte <= 0x39)) ||				// 0-9 
				(cbyte == 0x2A) ||									// "*" 
				(cbyte == 0x2D) ||									// "-" 
				(cbyte == 0x2E) ||									// "." 
				(cbyte == 0x5F) )									// "_"
			{ 
				// ���ϊ������������ꍇ
				strncpy((char*)(cbuffer + lresultcount), (char*)(csource + lcount), 2); // ���̂܂� cbuffer �ɃR�s�[
				lcount++;											// �ǂݍ��݃J�E���^���C���N�������g
				lresultcount++;										// �������݃J�E���^���C���N�������g
			} else {
				// ���̑��̕����̏ꍇ
				sprintf((char*)ctemp, "%%%02X", cbyte);				// URL �G���R�[�h
				strncpy((char*)(cbuffer + lresultcount), (char*)ctemp, 4); // cbuffer �ɃR�s�[
				lcount++;											// �ǂݍ��݃J�E���^���C���N�������g
				lresultcount += 3;									// �������݃J�E���^�� 3 ���₷
			}
			if (lcount == llength) {
				break;
			} // ������̏I�[�ɒB�����ꍇ�A���[�v�𔲂���
		}
		return lresultcount;	// cbuffer �ɏ������񂾕�����̃T�C�Y��Ԃ�
	}

	/**
	 * URL �G���R�[�h(EUC_JP)���s���B
	 *
	 * wchar_t : �����Z�b�g�� UNICODE (UCS-2)
	 * ��
	 * wchar_t : �����Z�b�g�͑S��ASCII
	 * �ɕϊ�����B
	 *
	 * ��̓I�ɂ́A
	 * wchar_t -> sjis -> euc -> url-encoded euc -> wchar_t (�����Z�b�g�͑S��ASCII) ���s���B
	 */
	static void encode_euc( LPCTSTR msg, TCHAR* url_encoded_text, int len )
	{
	#define MY_BUFFER_MAX	10000
		// msg (UNICODE) �� sjis �ɕϊ�
		kfm::kf_buf_type buf_sjis;
		buf_sjis.resize( MY_BUFFER_MAX+1 );
		wcstombs( (char*)&buf_sjis[0], msg, MY_BUFFER_MAX );

		// sjis �� euc �ɕϊ�
		kfm::kf_buf_type buf_euc;
		kfm::kfm filter( buf_sjis, buf_euc );
		filter.set_default_input_code( kfm::kfm::SJIS );
		filter.toeuc();

		// URL�G���R�[�h
		// euc ������ �� URL �G���R�[�h
		char buf_euc_url[ MY_BUFFER_MAX+1 ];
		{
			memset(buf_euc_url, 0x00, sizeof(char) * (MY_BUFFER_MAX+1));
			UrlEncode((unsigned char*)&buf_euc[0], (unsigned char*)buf_euc_url, MY_BUFFER_MAX);
		}

		// URL�G���R�[�h�ςݕ���������C�h������֕ϊ�
		mbstowcs(url_encoded_text, buf_euc_url, len);
	}

public:
	/**
	 * URL �G���R�[�h(EUC_JP)���s���B
	 */
	static CString encode_euc( LPCTSTR msg )
	{
		TCHAR url_encoded_text[MY_BUFFER_MAX+1];

		encode_euc( msg, url_encoded_text, MY_BUFFER_MAX );

		return url_encoded_text;
	}

};
