: ************************************
: *** �\�[�X�t�@�C���̃A�[�J�C�u�� ***
: ************************************

: �r���h�p���ϐ��̐ݒ�
@call 00_set_env.bat

@set dist_dir=%CD%

:----- src �t�@�C����cleanup -----
@echo �� src �t�@�C����cleanup���܂�
@cd ..\src
@call clean.bat
@cd %dist_dir%

:----- src �t�@�C���̈��k -----
@set archive=src.lzh
@echo �� src �t�@�C�������k���܂�
@cd ..
@del %archive%
%UNLHA_EXE% @%dist_dir%\response_src.txt

:----- �A�[�J�C�u�t�@�C���̈ړ� -----
@echo �� �A�[�J�C�u�t�@�C�����ړ����܂�
@set archive_to=dist\MZ3.%MZ3_VERSION%.src.lzh
@echo %archive% �� %archive_to%
@move %archive% %archive_to%
@cd %dist_dir%
