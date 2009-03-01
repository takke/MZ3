#ifndef __INC_JREGEX__
#define __INC_JREGEX__
#include "jectlib.h"
#include "JChar.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	//- �m�[�h�^
typedef struct _JNODE  JNODE;
typedef struct _JLOOP  JLOOP;
typedef struct _JBM    JBM;

	//- ��r������^��`
typedef union
{
	t_code (*PATTERN_FUNC)(u_int,void*); // �����擾�֐�
	t_code *PATTERN_TCODE;               // ������|�C���^
}PATTERN_STR;

	//- ����Q�ƌ^��`
typedef struct _BACK_REF
{
	u_int start; // �}�b�`�J�n�ʒu
	u_int match; // �}�b�`��
}BACK_REF;

	//- ���K�\�����s�I�v�V����
typedef enum _JREG_OPT
{
	JREG_NONE       = 0x0000, // �I�v�V�����w��Ȃ�

	JREG_COMPOPT    = 0x00FF, // �R���p�C���I�v�V�����}�X�N
	JREG_STARTLINE  = 0x0001, // ���s�����擪�Ƃ��Ĉ���
	JREG_ENDLINE    = 0x0002, // ���s���O���I�[�Ƃ��Ĉ���
	JREG_BINARYMODE = 0x0004, // �o�C�i�����[�h�Ŏ��s
	JREG_DFA        = 0x0008, // DFA�R���p�C�����s��
	JREG_MULTILINE  = JREG_STARTLINE | JREG_ENDLINE, // �����s������

	JREG_EXECOPT    = 0xFF00, // ���s�I�v�V�����}�X�N
	JREG_ICASE      = 0x0100, // �啶������������
	JREG_SINGLESTEP = 0x0200, // �����ʒu�ł̌���/�u�����������Ȃ�
	JREG_STRINGFUNC = 0x0400, // ��������R�[���o�b�N�֐�����擾
	JREG_USELASTRET = 0x0800, // �O��̌������ʂ��g�p

	JREG_NORMAL     = JREG_ICASE, // �f�t�H���g�̐ݒ�œ���
}jreg_opt;

	//- ���K�\���^��`
typedef struct _JREG
{
	t_code      *regstr;    // ���K�\���̕ۑ��p�R�s�[
	JNODE       *node;      // NFA�擪�m�[�h
	bool         isnfaonly; // NFA�R���p�C���T�|�[�g�L��
	JNODE       *regd;      // DFA�e�[�u��
	u_int        nodemax;   // DFA�e�[�u���v�f��
	JBM         *bm;        // BM�@�e�[�u��
	u_int        reglen;    // �����Ώە�����
	PATTERN_STR  Pattern;   // �����Ώ� (������/�֐��|�C���^)
	jreg_opt     execopt;   // ���s���I�v�V����
	jreg_opt     compopt;   // �R���p�C���I�v�V����
	void        *usrval;    // ���[�U��`�l
	
	u_int        backrefcnt; // ����Q�Ɖ�
	BACK_REF    *backref;    // ����Q�ƕۑ��o�b�t�@
	u_int        closurecnt; // ���[�v��
	JLOOP       *closure;    // ���[�v�񐔕ۑ��o�b�t�@
}jreg;

	//- �u���������^��`
typedef struct _JREG_REP
{
	t_code *repstr;  // �u�������㕶����p�^�[��
	t_code *repbuf;  // �u�������㕶����擾�o�b�t�@
	u_int   bufsize; // �K�v�o�b�t�@��
	u_int   maxsize; // �o�b�t�@�ő咷
	u_int   start;   // �u�������J�n�ʒu
	u_int   match;   // �u�������}�b�`��
}jreg_rep;


jreg* jreg_comp   (const t_code *regstr, jreg_opt opt);
bool  jreg_exec   (jreg *reg,                PATTERN_STR Pattern, u_int start, u_int len, jreg_opt opt);
bool  jreg_replace(jreg *reg, jreg_rep *rep, PATTERN_STR Pattern, u_int start, u_int len, jreg_opt opt);
void  jreg_free   (jreg *reg);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INC_JREGEX__
