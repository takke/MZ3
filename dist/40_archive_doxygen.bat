: ****************************************
: *** �\�[�X�h�L�������g�̃A�[�J�C�u�� ***
: ****************************************

: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat

@set dist_dir=%CD%
@set DOC=doc.zip
@set DOC_TO=..\dist\MZ3.%MZ3_VERSION%.srcdoc.zip

@echo �� �\�[�X�h�L�������g�t�@�C�������k���܂�
@cd ..\doxygen
@del %DOC%
@%_7Z_EXE% a -r -tzip %DOC% *

@echo �� ���k�����\�[�X�h�L�������g�t�@�C�����ړ����܂�
@echo %DOC% �� %DOC_TO%
@move %DOC% %DOC_TO%
@cd %dist_dir%
@pause
